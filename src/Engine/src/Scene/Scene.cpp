//
// Created by alexl on 02.06.2023.
//

#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Renderer/Renderer2D.h"
#include "Debug/DebugUtils.h"

namespace BeeEngine
{

    Entity Scene::CreateEntity(std::string_view name)
    {
        Entity entity(EntityID{m_Registry.create()}, this);
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name;
        return entity;
    }

    void Scene::OnUpdate()
    {
        UpdateScripts();


        SceneCamera* mainCamera = nullptr;
        glm::mat4 cameraTransform;

        auto camerasGroup = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
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
            Renderer2D::BeginScene(*mainCamera, cameraTransform);

            auto group = m_Registry.group<TransformComponent>(entt::get<Texture2DComponent>);
            for( auto entity : group )
            {
                auto [transform, texture] = group.get<TransformComponent, Texture2DComponent>(entity);
                Renderer2D::DrawImage(transform.GetTransform(), texture, Color4::White, 1.0f);
            }

            Renderer2D::EndScene();
        }
    }

    void Scene::OnViewPortResize(uint32_t width, uint32_t height)
    {
        BeeExpects(width > 0 && height > 0);

        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio)
            {
                cameraComponent.Camera.SetViewportSize(width, height);
            }
        }
    }

    void Scene::UpdateScripts()
    {
        auto view = m_Registry.view<NativeScriptComponent>();
        for (auto entity:view)
        {
            auto& scriptComponent = view.get<NativeScriptComponent>(entity);
            if (!scriptComponent.Instance)
            {
                scriptComponent.Instance = scriptComponent.InstantiateScript();
                scriptComponent.Instance->m_Entity = Entity(EntityID{entity}, this);
                scriptComponent.Instance->OnCreate();
            }

            scriptComponent.Instance->OnUpdate();
        }
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity);
    }

    void Scene::Clear()
    {
        m_Registry.clear();
    }
}
