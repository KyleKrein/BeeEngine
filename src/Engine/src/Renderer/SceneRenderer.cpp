//
// Created by alexl on 25.10.2023.
//

#include "SceneRenderer.h"
#include "IBindable.h"
#include "UniformBuffer.h"
#include "BindingSet.h"
#include "Renderer.h"
#include "RenderingQueue.h"
#include "Scene/Components.h"
#include "Core/Application.h"
#include "gtc/type_ptr.hpp"
#include "Debug/Instrumentor.h"
#include <cmath>
#include <glm/glm.hpp>

namespace BeeEngine
{
    Model* SceneRenderer::s_RectModel = nullptr;
    Model* SceneRenderer::s_CircleModel = nullptr;
    Texture2D* SceneRenderer::s_BlankTexture = nullptr;
    struct SpriteInstanceBufferData
    {
        glm::mat4 Model {1.0f};
        BeeEngine::Color4 Color {BeeEngine::Color4::White};
        float TilingFactor = 1.0f;
        int32_t EntityID = -1;
    };
    struct CircleInstanceBufferData
    {
        glm::mat4 Model {1.0f};
        BeeEngine::Color4 Color {BeeEngine::Color4::White};
        float Thickness = 1.0f;
        float Fade = 0.005f;
        int32_t EntityID = -1;
    };

    void SceneRenderer::RenderScene(Scene &scene, FrameBuffer &frameBuffer, const String& locale, const glm::mat4 &viewProjectionMatrix, const Math::Cameras::Frustum& frustum)
    {
        BEE_PROFILE_FUNCTION();
        Ref<UniformBuffer> cameraUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4));
        Ref<BindingSet> cameraBindingSet = BindingSet::Create({{0, *cameraUniformBuffer}});
        cameraUniformBuffer->SetData(const_cast<float*>(glm::value_ptr(viewProjectionMatrix)), sizeof(glm::mat4));
        SceneTreeRenderer sceneTreeRenderer(viewProjectionMatrix, cameraBindingSet.get());

        {
            BEE_PROFILE_SCOPE("SceneTreeRenderer::AddEntities");
            auto spriteView = scene.m_Registry.view<SpriteRendererComponent>();
            for( auto entity : spriteView )
            {
                Entity e = {entity, &scene};
                auto& transformComponent = e.GetComponent<TransformComponent>();
                Math::Cameras::Sphere sphere{};
                sphere.radius = 0.5f * transformComponent.Scale.x;
                glm::mat4 transform = Math::ToGlobalTransform(e);
                if(!sphere.IsOnFrustum(frustum, transform))
                {
                    //continue;
                }
                auto& spriteComponent = spriteView.get<SpriteRendererComponent>(entity);
                SpriteInstanceBufferData data {transform, spriteComponent.Color, spriteComponent.TilingFactor,
                                               static_cast<int32_t>(entity)+1};
                std::vector<BindingSet*> bindingSets {cameraBindingSet.get(), (spriteComponent.HasTexture ? spriteComponent.Texture(locale)->GetBindingSet() : s_BlankTexture->GetBindingSet())};
                sceneTreeRenderer.AddEntity(data.Model, data.Color.A() < 0.95f || spriteComponent.HasTexture, *s_RectModel, bindingSets, {(byte*)&data, sizeof(SpriteInstanceBufferData)});
            }

            auto circleGroup = scene.m_Registry.view<CircleRendererComponent>();
            std::vector<BindingSet*> circleBindingSets {cameraBindingSet.get()};
            for( auto entity : circleGroup )
            {
                Entity e = {entity, &scene};
                auto& transformComponent = e.GetComponent<TransformComponent>();
                Math::Cameras::Sphere sphere{};
                sphere.radius = 0.5f * transformComponent.Scale.x;
                glm::mat4 transform = Math::ToGlobalTransform(e);
                if(!sphere.IsOnFrustum(frustum, transform))
                {
                    //continue;
                }
                auto& circleComponent = circleGroup.get<CircleRendererComponent>(entity);
                CircleInstanceBufferData data {transform, circleComponent.Color, circleComponent.Thickness, circleComponent.Fade,
                                               static_cast<int32_t>(entity)+1};
                sceneTreeRenderer.AddEntity(data.Model, true, *s_CircleModel, circleBindingSets, {(byte*)&data, sizeof(CircleInstanceBufferData)});
            }

            auto textGroup = scene.m_Registry.view<TextRendererComponent>();
            for( auto entity : textGroup )
            {
                auto& textComponent = textGroup.get<TextRendererComponent>(entity);
                sceneTreeRenderer.AddText(textComponent.Text, &textComponent.Font(locale), Math::ToGlobalTransform(Entity{entity, &scene}), textComponent.Configuration,
                                 static_cast<int32_t>(entity)+1);
            }
        }

        auto& statistics = Internal::RenderingQueue::GetInstance().m_Statistics;
        statistics.OpaqueInstanceCount += sceneTreeRenderer.m_NotTransparent.size();
        statistics.TransparentInstanceCount += sceneTreeRenderer.m_Transparent.size();

        {
            BEE_PROFILE_SCOPE("SceneTreeRenderer::Sort");
            std::ranges::sort(sceneTreeRenderer.m_Transparent,
                              [&viewProjectionMatrix](const SceneTreeRenderer::Entity &a,
                                                      const SceneTreeRenderer::Entity &b)
                              {
                                  auto aDistance = glm::distance(a.Transform[3], viewProjectionMatrix[3]);
                                  auto bDistance = glm::distance(b.Transform[3], viewProjectionMatrix[3]);
                                  return aDistance > bDistance;
                              });
        }
        {
            BEE_PROFILE_SCOPE("SceneTreeRenderer::SubmitToRendering");
            for (auto &entity: sceneTreeRenderer.m_NotTransparent)
            {
                Renderer::SubmitInstance(*entity.Model, entity.BindingSets, entity.InstancedData);
            }
            Renderer::Flush();
            /*frameBuffer.Flush([transparent = std::move(sceneTreeRenderer.m_Transparent), cameraBuffer = std::move(cameraUniformBuffer), cameraSet = std::move(cameraBindingSet)]()mutable {
                for(auto& entity: transparent)
                {
                    Renderer::SubmitInstance(*entity.Model,entity.BindingSets, entity.InstancedData);
                }
            });*/
            for (auto &entity: sceneTreeRenderer.m_Transparent)
            {
                Renderer::SubmitInstance(*entity.Model, entity.BindingSets, entity.InstancedData);
            }
            Renderer::Flush();
        }
    }

    void SceneRenderer::RenderScene(Scene &scene, FrameBuffer &frameBuffer, const String& locale)
    {
        SceneCamera* mainCamera = nullptr;
        glm::mat4 cameraTransform;
        glm::vec3 cameraPosition;

        auto camerasGroup = scene.m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
        for (auto entity:camerasGroup)
        {
            auto [transform, camera] = camerasGroup.get<TransformComponent, CameraComponent>(entity);
            if(camera.Primary)
            {
                mainCamera = &camera.Camera;
                cameraTransform = Math::ToGlobalTransform(Entity{entity, &scene});
                auto[translation, rotation, scale] = Math::DecomposeTransform(cameraTransform);
                cameraPosition = translation;
                break;
            }
        }

        if(mainCamera)
        {
            auto cameraViewMatrix = glm::inverse(cameraTransform);
            auto cameraViewProj = mainCamera->GetProjectionMatrix() * cameraViewMatrix;
            glm::vec3 forward = -glm::vec3(cameraViewMatrix[0][2], cameraViewMatrix[1][2], cameraViewMatrix[2][2]);
            glm::vec3 right = glm::vec3(cameraViewMatrix[0][0], cameraViewMatrix[1][0], cameraViewMatrix[2][0]);
            glm::vec3 up = glm::vec3(cameraViewMatrix[0][1], cameraViewMatrix[1][1], cameraViewMatrix[2][1]);
            RenderScene(scene, frameBuffer, locale, *mainCamera, cameraViewProj, cameraPosition, forward, up, right);
        }
    }

    void SceneRenderer::Init()
    {
        s_RectModel = {&Application::GetInstance().GetAssetManager().GetModel("Renderer2D_Rectangle")};
        s_CircleModel = {&Application::GetInstance().GetAssetManager().GetModel("Renderer2D_Circle")};
        s_BlankTexture = {&Application::GetInstance().GetAssetManager().GetTexture("Blank")};
    }

    void
    SceneRenderer::RenderPhysicsColliders(Scene &scene, FrameBuffer &frameBuffer, const glm::mat4 &viewProjectionMatrix)
    {
        Ref<UniformBuffer> cameraUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4));
        Ref<BindingSet> cameraBindingSet = BindingSet::Create({{0, *cameraUniformBuffer}});
        cameraUniformBuffer->SetData(const_cast<float*>(glm::value_ptr(viewProjectionMatrix)), sizeof(glm::mat4));
        RenderPhysicsColliders(scene, frameBuffer, *cameraBindingSet);
    }

    void SceneRenderer::RenderPhysicsColliders(Scene &scene, FrameBuffer &frameBuffer, BindingSet &cameraBindingSet)
    {
        auto& registry = scene.m_Registry;
        auto view = registry.view<BoxCollider2DComponent>();
        {
            for (auto entity : view)
            {
                auto bc2d = view.get<BoxCollider2DComponent>(entity);
                auto[translation, rotation, scale] = Math::DecomposeTransform(Math::ToGlobalTransform(Entity{entity, &scene}));
                if(bc2d.Type == BoxCollider2DComponent::ColliderType::Box)
                {
                    translation = translation + glm::vec3(bc2d.Offset, 0.001f);
                    scale = scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                                          * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
                                          * glm::translate(glm::mat4(1.0f), glm::vec3(bc2d.Offset, 0.001f))
                                          * glm::scale(glm::mat4(1.0f), scale);

                    Renderer::DrawRect(transform, Color4::DarkGreen, cameraBindingSet, 0.1f);
                }
                else if(bc2d.Type == BoxCollider2DComponent::ColliderType::Circle)
                {
                    translation = translation + glm::vec3(bc2d.Offset, -0.001f);
                    //scale = scale * glm::vec3(bc2d.Radius * 2.0f);

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                                          * glm::scale(glm::mat4(1.0f), scale);
                    std::vector<BindingSet*> bindingSets {&cameraBindingSet};
                    CircleInstanceBufferData data {transform, Color4::DarkGreen, 0.05f, 0.005f, static_cast<int32_t>(entity)+1};
                    Renderer::SubmitInstance(*s_CircleModel, bindingSets, {(byte*)&data, sizeof(CircleInstanceBufferData)});
                }
            }
        }
    }
} // BeeEngine