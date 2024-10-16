//
// Created by alexl on 25.10.2023.
//

#include "SceneRenderer.h"
#include "BindingSet.h"
#include "Core/Application.h"
#include "Core/Logging/Log.h"
#include "Debug/Instrumentor.h"
#include "IBindable.h"
#include "Renderer.h"
#include "RenderingQueue.h"
#include "Scene/Components.h"
#include "Scripting/ScriptingEngine.h"
#include "UniformBuffer.h"
#include "gtc/type_ptr.hpp"
#include <cmath>
#include <glm/glm.hpp>

namespace BeeEngine
{
    std::vector<glm::vec4> GetFrustumPlanes(const glm::mat4& viewProj)
    {
        // x, y, z, and w represent A, B, C and D in the plane equation
        // where ABC are the xyz of the planes normal, and D is the plane constant
        std::vector<glm::vec4> tempFrustumPlane(6);

        // Left Frustum Plane
        // Add first column of the matrix to the fourth column
        tempFrustumPlane[0].x = viewProj[0][3] + viewProj[0][0];
        tempFrustumPlane[0].y = viewProj[1][3] + viewProj[1][0];
        tempFrustumPlane[0].z = viewProj[2][3] + viewProj[2][0];
        tempFrustumPlane[0].w = viewProj[3][3] + viewProj[3][0];

        // Right Frustum Plane
        // Subtract first column of matrix from the fourth column
        tempFrustumPlane[1].x = viewProj[0][3] - viewProj[0][0];
        tempFrustumPlane[1].y = viewProj[1][3] - viewProj[1][0];
        tempFrustumPlane[1].z = viewProj[2][3] - viewProj[2][0];
        tempFrustumPlane[1].w = viewProj[3][3] - viewProj[3][0];

        // Top Frustum Plane
        // Subtract second column of matrix from the fourth column
        tempFrustumPlane[2].x = viewProj[0][3] - viewProj[0][1];
        tempFrustumPlane[2].y = viewProj[1][3] - viewProj[1][1];
        tempFrustumPlane[2].z = viewProj[2][3] - viewProj[2][1];
        tempFrustumPlane[2].w = viewProj[3][3] - viewProj[3][1];

        // Bottom Frustum Plane
        // Add second column of the matrix to the fourth column
        tempFrustumPlane[3].x = viewProj[0][3] + viewProj[0][1];
        tempFrustumPlane[3].y = viewProj[1][3] + viewProj[1][1];
        tempFrustumPlane[3].z = viewProj[2][3] + viewProj[2][1];
        tempFrustumPlane[3].w = viewProj[3][3] + viewProj[3][1];

        // Near Frustum Plane
        // We could add the third column to the fourth column to get the near plane,
        // but we don't have to do this because the third column IS the near plane
        tempFrustumPlane[4].x = viewProj[0][2];
        tempFrustumPlane[4].y = viewProj[1][2];
        tempFrustumPlane[4].z = viewProj[2][2];
        tempFrustumPlane[4].w = viewProj[3][2];

        // Far Frustum Plane
        // Subtract third column of matrix from the fourth column
        tempFrustumPlane[5].x = viewProj[0][3] - viewProj[0][2];
        tempFrustumPlane[5].y = viewProj[1][3] - viewProj[1][2];
        tempFrustumPlane[5].z = viewProj[2][3] - viewProj[2][2];
        tempFrustumPlane[5].w = viewProj[3][3] - viewProj[3][2];

        // Normalize plane normals (A, B and C (xyz))
        // Also take note that planes face inward
        for (int i = 0; i < 6; ++i)
        {
            // float length = sqrt((tempFrustumPlane[i].x * tempFrustumPlane[i].x) + (tempFrustumPlane[i].y *
            // tempFrustumPlane[i].y) + (tempFrustumPlane[i].z * tempFrustumPlane[i].z));
            tempFrustumPlane[i] = glm::normalize(tempFrustumPlane[i]); ///= length;
            /*tempFrustumPlane[i].y /= length;
            tempFrustumPlane[i].z /= length;
            tempFrustumPlane[i].w /= length;*/
        }

        return tempFrustumPlane;
    }

    struct AABB
    {
        glm::vec3 min = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
        glm::vec3 max = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);
    };
    bool IsInFrustum(const std::vector<glm::vec4>& frustumPlanes, const AABB& boundingBox)
    {
        // Loop through each frustum plane
        for (int planeID = 0; planeID < 6; ++planeID)
        {
            auto planeNormal = glm::vec3(frustumPlanes[planeID]);
            float planeConstant = frustumPlanes[planeID].w;
            // Check each axis (x,y,z) to get the AABB vertex furthest away from the direction the plane is facing
            // (plane normal)
            glm::vec3 axisVert;
            // x-axis
            if (frustumPlanes[planeID].x <
                0.0f) // Which AABB vertex is furthest down (plane normals direction) the x axis
                axisVert.x = boundingBox.min.x; // min x plus tree positions x
            else
                axisVert.x = boundingBox.max.x; // max x plus tree positions x// y-axis
            if (frustumPlanes[planeID].y <
                0.0f) // Which AABB vertex is furthest down (plane normals direction) the y axis
                axisVert.y = boundingBox.min.y; // min y plus tree positions y
            else
                axisVert.y = boundingBox.max.y; // max y plus tree positions y// z-axis
            if (frustumPlanes[planeID].z <
                0.0f) // Which AABB vertex is furthest down (plane normals direction) the z axis
                axisVert.z = boundingBox.min.z; // min z plus tree positions z
            else
                axisVert.z = boundingBox.max.z; // max z plus tree positions z// Now we get the signed distance from the
                                                // AABB vertex that's furthest down the frustum planes normal,// and if
                                                // the signed distance is negative, then the entire bounding box is
                                                // behind the frustum plane, which means// that it should be culled
            if (glm::dot(planeNormal, axisVert) + planeConstant < 0.0f)
            {
                return false;
            }
        }
        return true;
    }

    AABB CreateAABB(const std::vector<glm::vec3>& vertPosArray)
    {
        AABB boundingBox;
        auto& minVertex = boundingBox.min;
        auto& maxVertex = boundingBox.max;
        for (size_t i = 0; i < vertPosArray.size(); i++)
        {
            // The minVertex and maxVertex will most likely not be actual vertices in the model, but vertices
            // that use the smallest and largest x, y, and z values from the model to be sure ALL vertices are
            // covered by the bounding volume

            // Get the smallest vertex
            minVertex.x = std::min(minVertex.x, vertPosArray[i].x); // Find smallest x value in model
            minVertex.y = std::min(minVertex.y, vertPosArray[i].y); // Find smallest y value in model
            minVertex.z = std::min(minVertex.z, vertPosArray[i].z); // Find smallest z value in model

            // Get the largest vertex
            maxVertex.x = std::max(maxVertex.x, vertPosArray[i].x); // Find largest x value in model
            maxVertex.y = std::max(maxVertex.y, vertPosArray[i].y); // Find largest y value in model
            maxVertex.z = std::max(maxVertex.z, vertPosArray[i].z); // Find largest z value in model
        }

        return boundingBox;
    }
    Model* SceneRenderer::s_RectModel = nullptr;
    Model* SceneRenderer::s_CircleModel = nullptr;
    Texture2D* SceneRenderer::s_BlankTexture = nullptr;
    bool IsSphereInFrustum(const Math::Cameras::Sphere& sphere, const Math::Cameras::Frustum& frustum)
    {
        Math::Cameras::Plane planes[6] = {frustum.NearFace,
                                          frustum.FarFace,
                                          frustum.BottomFace,
                                          frustum.TopFace,
                                          frustum.RightFace,
                                          frustum.LeftFace};
        for (int i = 0; i < 6; ++i)
        {
            const Math::Cameras::Plane& plane = planes[i];
            float distance = glm::dot(plane.Normal, sphere.center) - plane.Distance;
            if (distance < -sphere.radius)
            {
                // Сфера полностью за плоскостью, следовательно, вне фрустума
                return false;
            }
        }
        // Сфера пересекает все плоскости фрустума или полностью внутри фрустума
        return true;
    }
    void
    SceneRenderer::RenderScene(Scene& scene,
                               CommandBuffer& commandBuffer,
                               const Locale::Localization& localization,
                               const glm::mat4& viewProjectionMatrix,
                               const std::vector<glm::vec4>& frustumPlanes /* const Math::Cameras::Frustum& frustum*/)
    {
        BEE_PROFILE_FUNCTION();
        auto& locale = localization.GetLanguageString();
        auto& sceneRendererData = scene.GetSceneRendererData();
        sceneRendererData.CameraUniformBuffer->SetData(glm::value_ptr(viewProjectionMatrix), sizeof(glm::mat4));

        Scene::GPUSceneData sceneData{};
        sceneData.viewproj = viewProjectionMatrix;
        sceneData.ambientColor = Color4{Color4::Green};
        sceneData.sunlightDirection = glm::vec4{0.0f, 1.0f, 0.0f, 1.0f};
        sceneData.sunlightColor = Color4{Color4::Yellow};

        sceneRendererData.MeshSceneDataUniformBuffer->SetData(&sceneData, sizeof(Scene::GPUSceneData));

        SceneTreeRenderer sceneTreeRenderer(viewProjectionMatrix, sceneRendererData.CameraBindingSet.get());

        // auto frustumPlanes = GetFrustumPlanes(viewProjectionMatrix);
        {
            BEE_PROFILE_SCOPE("SceneTreeRenderer::AddEntities");
            auto spriteView = scene.m_Registry.view<SpriteRendererComponent>();
            for (auto entity : spriteView)
            {
                Entity e = {entity, scene.weak_from_this()};
                glm::mat4 transform = Math::ToGlobalTransform(e);
                auto& spriteComponent = spriteView.get<SpriteRendererComponent>(entity);
                SpriteInstanceBufferData data{
                    transform, spriteComponent.Color, spriteComponent.TilingFactor, static_cast<int32_t>(entity) + 1};
                std::vector<BindingSet*> bindingSets{sceneRendererData.CameraBindingSet.get(),
                                                     (spriteComponent.HasTexture
                                                          ? &spriteComponent.Texture(locale)->GetBindingSet()
                                                          : &s_BlankTexture->GetBindingSet())};
                sceneTreeRenderer.AddEntity(data.Model,
                                            data.Color.A() < 0.95f || spriteComponent.HasTexture,
                                            *s_RectModel,
                                            bindingSets,
                                            {(byte*)&data, sizeof(SpriteInstanceBufferData)});
            }

            auto circleGroup = scene.m_Registry.view<CircleRendererComponent>();
            std::vector<BindingSet*> circleBindingSets{sceneRendererData.CameraBindingSet.get()};
            for (auto entity : circleGroup)
            {
                Entity e = {entity, scene.weak_from_this()};
                auto& transformComponent = e.GetComponent<TransformComponent>();
                glm::mat4 transform = Math::ToGlobalTransform(e);
                auto& circleComponent = circleGroup.get<CircleRendererComponent>(entity);
                CircleInstanceBufferData data{transform,
                                              circleComponent.Color,
                                              circleComponent.Thickness,
                                              circleComponent.Fade,
                                              static_cast<int32_t>(entity) + 1};
                sceneTreeRenderer.AddEntity(data.Model,
                                            true,
                                            *s_CircleModel,
                                            circleBindingSets,
                                            {(byte*)&data, sizeof(CircleInstanceBufferData)});
            }

            auto textGroup = scene.m_Registry.view<TextRendererComponent>();
            for (auto entity : textGroup)
            {
                auto& textComponent = textGroup.get<TextRendererComponent>(entity);
                sceneTreeRenderer.AddText(textComponent.Text,
                                          &textComponent.Font(locale),
                                          Math::ToGlobalTransform(Entity{entity, scene.weak_from_this()}),
                                          textComponent.Configuration,
                                          static_cast<int32_t>(entity) + 1);
            }

            auto meshGroup = scene.m_Registry.view<MeshComponent>();

            for (auto entity : meshGroup)
            {
                auto& meshComponent = meshGroup.get<MeshComponent>(entity);
                if (!meshComponent.HasMeshes)
                    continue;
                Entity e = {entity, scene.weak_from_this()};
                glm::mat4 transform = Math::ToGlobalTransform(e);

                struct MeshInstancedData
                {
                    glm::mat4 Model;
                    int32_t EntityID;
                } meshInstancedData{transform, static_cast<int32_t>(entity) + 1};

                meshComponent.MaterialInstance.LoadData();
                std::vector bindingSets{sceneRendererData.MeshSceneDataBindingSet.get(),
                                        meshComponent.MaterialInstance.bindingSet.get()};
                for (auto& model : meshComponent.MeshSource()->GetModels())
                {
                    sceneTreeRenderer.AddEntity(
                        transform, false, model, bindingSets, {(byte*)&meshInstancedData, sizeof(MeshInstancedData)});
                }
            }

            auto scriptGroup = scene.m_Registry.view<ScriptComponent>();
            for (auto entity : scriptGroup)
            {
                auto& scriptComponent = scriptGroup.get<ScriptComponent>(entity);
                Entity e = {entity, scene.weak_from_this()};
                if (scriptComponent.Class)
                {
                    ScriptingEngine::OnEntityRender(e, commandBuffer);
                }
            }
        }
        BeeCoreTrace("SceneTreeRenderer::AddEntities done");
        // auto& statistics = Internal::RenderingQueue::GetInstance().m_Statistics;
        // statistics.OpaqueInstanceCount += sceneTreeRenderer.m_NotTransparent.size();
        // statistics.TransparentInstanceCount += sceneTreeRenderer.m_Transparent.size();
        // auto& tlas = scene.GetTLAS();
        // tlas.UpdateInstances(std::move(sceneTreeRenderer.GetAllEntities()));

        // TODO: this is temporary
        for (auto& entity : sceneTreeRenderer.m_Opaque)
        {
            commandBuffer.SubmitInstance(*entity.Model, entity.BindingSets, entity.InstancedData);
        }
        commandBuffer.Flush();
        for (auto& entity : sceneTreeRenderer.m_Transparent)
        {
            commandBuffer.SubmitInstance(*entity.Model, entity.BindingSets, entity.InstancedData);
        }
        commandBuffer.Flush();
        BeeCoreTrace("Finished Rendering scene");
    }

    void SceneRenderer::RenderScene(Scene& scene, CommandBuffer& commandBuffer, const Locale::Localization& locale)
    {
        SceneCamera* mainCamera = nullptr;
        glm::mat4 cameraTransform;
        glm::vec3 cameraPosition;

        auto camerasGroup = scene.m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
        for (auto entity : camerasGroup)
        {
            auto [transform, camera] = camerasGroup.get<TransformComponent, CameraComponent>(entity);
            if (camera.Primary)
            {
                mainCamera = &camera.Camera;
                cameraTransform = Math::ToGlobalTransform(Entity{entity, scene.weak_from_this()});
                auto [translation, rotation, scale] = Math::DecomposeTransform(cameraTransform);
                cameraPosition = translation;
                break;
            }
        }

        if (mainCamera)
        {
            auto cameraViewMatrix = glm::inverse(cameraTransform);
            auto cameraViewProj = mainCamera->GetProjectionMatrix() * cameraViewMatrix;
            /* glm::vec3 forward = -glm::normalize(glm::vec3(cameraViewMatrix[0][2], cameraViewMatrix[1][2],
             cameraViewMatrix[2][2])); glm::vec3 right = glm::normalize(glm::vec3(cameraViewMatrix[0][0],
             cameraViewMatrix[1][0], cameraViewMatrix[2][0])); glm::vec3 up =
             glm::normalize(glm::vec3(cameraViewMatrix[0][1], cameraViewMatrix[1][1], cameraViewMatrix[2][1])); auto
             frustum = Math::Cameras::CreateFrustumFromCamera(cameraPosition, forward, right, up,
             mainCamera->GetAspectRatio(), glm::degrees(mainCamera->GetVerticalFOV()), mainCamera->GetNearClip(),
             mainCamera->GetFarClip());*/
            auto frustumPlanes = GetFrustumPlanes(cameraViewProj);
            RenderScene(scene, commandBuffer, locale, cameraViewProj, frustumPlanes);
        }
    }

    void SceneRenderer::Init()
    {
        s_RectModel = {&Application::GetInstance().GetAssetManager().GetModel("Renderer2D_Rectangle")};
        s_CircleModel = {&Application::GetInstance().GetAssetManager().GetModel("Renderer2D_Circle")};
        s_BlankTexture = {&Application::GetInstance().GetAssetManager().GetTexture("Blank")};
    }

    void SceneRenderer::RenderPhysicsColliders(Scene& scene,
                                               CommandBuffer& commandBuffer,
                                               const glm::mat4& viewProjectionMatrix)
    {
        Ref<UniformBuffer> cameraUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4));
        Ref<BindingSet> cameraBindingSet = BindingSet::Create({{0, *cameraUniformBuffer}});
        cameraUniformBuffer->SetData(glm::value_ptr(viewProjectionMatrix), sizeof(glm::mat4));
        RenderPhysicsColliders(scene, commandBuffer, *cameraBindingSet);
    }

    void SceneRenderer::RenderPhysicsColliders(Scene& scene, CommandBuffer& commandBuffer, BindingSet& cameraBindingSet)
    {
        auto& registry = scene.m_Registry;
        auto view = registry.view<BoxCollider2DComponent>();
        {
            for (auto entity : view)
            {
                auto bc2d = view.get<BoxCollider2DComponent>(entity);
                auto [translation, rotation, scale] =
                    Math::DecomposeTransform(Math::ToGlobalTransform(Entity{entity, scene.weak_from_this()}));
                if (bc2d.Type == BoxCollider2DComponent::ColliderType::Box)
                {
                    translation = translation + glm::vec3(bc2d.Offset, 0.001f);
                    scale = scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) *
                                          glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
                                          glm::translate(glm::mat4(1.0f), glm::vec3(bc2d.Offset, 0.001f)) *
                                          glm::scale(glm::mat4(1.0f), scale);

                    commandBuffer.DrawRect(transform, Color4::DarkGreen, cameraBindingSet, 0.1f);
                }
                else if (bc2d.Type == BoxCollider2DComponent::ColliderType::Circle)
                {
                    translation = translation + glm::vec3(bc2d.Offset, -0.001f);
                    // scale = scale * glm::vec3(bc2d.Radius * 2.0f);

                    glm::mat4 transform =
                        glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), scale);
                    std::vector<BindingSet*> bindingSets{&cameraBindingSet};
                    CircleInstanceBufferData data{
                        transform, Color4::DarkGreen, 0.05f, 0.005f, static_cast<int32_t>(entity) + 1};
                    commandBuffer.SubmitInstance(
                        *s_CircleModel, bindingSets, {(byte*)&data, sizeof(CircleInstanceBufferData)});
                }
            }
        }
    }
} // namespace BeeEngine