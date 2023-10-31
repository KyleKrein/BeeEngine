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

    void SceneRenderer::RenderScene(Scene &scene, FrameBuffer &frameBuffer, const String& locale, const glm::mat4 &viewProjectionMatrix)
    {
        BEE_PROFILE_FUNCTION();
        Ref<UniformBuffer> cameraUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4));
        Ref<BindingSet> cameraBindingSet = BindingSet::Create({{0, *cameraUniformBuffer}});
        cameraUniformBuffer->SetData(const_cast<float*>(glm::value_ptr(viewProjectionMatrix)), sizeof(glm::mat4));
#if 1 //use SceneTreeRenderer
        SceneTreeRenderer sceneTreeRenderer(viewProjectionMatrix, cameraBindingSet.get());

        {
            BEE_PROFILE_SCOPE("SceneTreeRenderer::AddEntities");
            constexpr static float epsilon = 1e-6;
            auto spriteView = scene.m_Registry.view<SpriteRendererComponent>();
            for( auto entity : spriteView )
            {
                auto& spriteComponent = spriteView.get<SpriteRendererComponent>(entity);
                SpriteInstanceBufferData data {Math::ToGlobalTransform(Entity{entity, &scene}), spriteComponent.Color, spriteComponent.TilingFactor,
                                               static_cast<int32_t>(entity)+1};
                std::vector<BindingSet*> bindingSets {cameraBindingSet.get(), (spriteComponent.HasTexture ? spriteComponent.Texture(locale)->GetBindingSet() : s_BlankTexture->GetBindingSet())};
                sceneTreeRenderer.AddEntity(data.Model, data.Color.A() < 0.95f || spriteComponent.HasTexture, *s_RectModel, bindingSets, {(byte*)&data, sizeof(SpriteInstanceBufferData)});
            }

            auto circleGroup = scene.m_Registry.view<CircleRendererComponent>();
            std::vector<BindingSet*> circleBindingSets {cameraBindingSet.get()};
            for( auto entity : circleGroup )
            {
                auto& circleComponent = circleGroup.get<CircleRendererComponent>(entity);
                CircleInstanceBufferData data {Math::ToGlobalTransform(Entity{entity, &scene}), circleComponent.Color, circleComponent.Thickness, circleComponent.Fade,
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
#else
        auto spriteView = scene.m_Registry.view<SpriteRendererComponent>();
        for( auto entity : spriteView )
        {
            auto& spriteComponent = spriteView.get<SpriteRendererComponent>(entity);
            SpriteInstanceBufferData data {Math::ToGlobalTransform(Entity{entity, &scene}), spriteComponent.Color, spriteComponent.TilingFactor,
                                           static_cast<int32_t>(entity)+1};
            std::vector<BindingSet*> bindingSets {cameraBindingSet.get(), (spriteComponent.HasTexture ? spriteComponent.Texture(locale)->GetBindingSet() : s_BlankTexture->GetBindingSet())};
            Renderer::SubmitInstance(*s_RectModel, bindingSets, {(byte*)&data, sizeof(SpriteInstanceBufferData)});
        }

        auto circleGroup = scene.m_Registry.view<CircleRendererComponent>();
        std::vector<BindingSet*> circleBindingSets {cameraBindingSet.get()};
        for( auto entity : circleGroup )
        {
            auto& circleComponent = circleGroup.get<CircleRendererComponent>(entity);
            CircleInstanceBufferData data {Math::ToGlobalTransform(Entity{entity, &scene}), circleComponent.Color, circleComponent.Thickness, circleComponent.Fade,
                                           static_cast<int32_t>(entity)+1};
            Renderer::SubmitInstance(*s_CircleModel, circleBindingSets, {(byte*)&data, sizeof(CircleInstanceBufferData)});
        }

        auto textGroup = scene.m_Registry.view<TextRendererComponent>();
        SceneTreeRenderer sceneTreeRenderer(viewProjectionMatrix, cameraBindingSet.get());
        for( auto entity : textGroup )
        {
            auto& textComponent = textGroup.get<TextRendererComponent>(entity);
            sceneTreeRenderer.AddText(textComponent.Text, &textComponent.Font(locale), Math::ToGlobalTransform(Entity{entity, &scene}), textComponent.Configuration,
                                      static_cast<int32_t>(entity)+1);
        }
        sceneTreeRenderer.Render();
#endif
    }

    void SceneRenderer::RenderScene(Scene &scene, FrameBuffer &frameBuffer, const String& locale)
    {
        SceneCamera* mainCamera = nullptr;
        glm::mat4 cameraTransform;

        auto camerasGroup = scene.m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
        for (auto entity:camerasGroup)
        {
            auto [transform, camera] = camerasGroup.get<TransformComponent, CameraComponent>(entity);
            if(camera.Primary)
            {
                mainCamera = &camera.Camera;
                cameraTransform = transform.GetTransform();
                break;
            }
        }

        if(mainCamera)
        {
            auto cameraViewProj = mainCamera->GetProjectionMatrix() * glm::inverse(cameraTransform);
            RenderScene(scene, frameBuffer, locale, cameraViewProj);
        }
    }

    void SceneRenderer::Init()
    {
        s_RectModel = {&Application::GetInstance().GetAssetManager().GetModel("Renderer2D_Rectangle")};
        s_CircleModel = {&Application::GetInstance().GetAssetManager().GetModel("Renderer2D_Circle")};
        s_BlankTexture = {&Application::GetInstance().GetAssetManager().GetTexture("Blank")};
    }
} // BeeEngine