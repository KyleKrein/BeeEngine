//
// Created by alexl on 02.06.2023.
//

#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Renderer/Renderer.h"
#include "Core/CodeSafety/Expects.h"
#include "Core/Application.h"
#include "gtc/type_ptr.hpp"
#include <glm/glm.hpp>
#include "NativeScriptFactory.h"
#include "Core/UUID.h"
#include "Scripting/ScriptingEngine.h"

namespace BeeEngine
{

    Entity Scene::CreateEntity(const std::string& name)
    {
        return CreateEntityWithUUID(UUID(), std::string(name));
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
        auto nativeScriptsView = m_Registry.view<NativeScriptComponent>();
        for (auto entity:nativeScriptsView)
        {
            auto& scriptComponent = nativeScriptsView.get<NativeScriptComponent>(entity);
            if (!scriptComponent.Instance)
            {
                BeeCoreTrace("Instanciating Script: {0}", scriptComponent.Name);
                scriptComponent.Instance = scriptComponent.InstantiateScript(scriptComponent.Name.c_str());
                scriptComponent.Instance->m_Entity = Entity(EntityID{entity}, this);
                scriptComponent.Instance->OnCreate();
            }

            scriptComponent.Instance->OnUpdate();
        }

        auto view = m_Registry.view<ScriptComponent>();
        for (auto e : view)
        {
            Entity entity {EntityID{e}, this};
            auto& scriptComponent = entity.GetComponent<ScriptComponent>();
            if(scriptComponent.Class)
            {
                ScriptingEngine::OnEntityCreated(entity, scriptComponent.Class);
                ScriptingEngine::OnEntityUpdate(entity);
            }
        }
    }

    void Scene::DestroyEntity(Entity entity)
    {
        ScriptingEngine::OnEntityDestroyed(entity);
        m_UUIDMap.erase(entity.GetUUID());
        m_Registry.destroy(entity);
    }

    void Scene::Clear()
    {
        m_Registry.clear();
    }
    struct InstanceBufferData
    {
        /*alignas(alignof(glm::mat4))*/ glm::mat4 Model {1.0f};
        /*alignas(alignof(glm::mat4))*/ BeeEngine::Color4 Color {BeeEngine::Color4::White};
        /*alignas(alignof(glm::mat4))*/ float TilingFactor = 1.0f;
        int32_t EntityID = -1;
    };
    void Scene::UpdateRuntime()
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
            auto cameraViewProj = mainCamera->GetProjectionMatrix() * glm::inverse(cameraTransform);
            m_CameraUniformBuffer->SetData(&cameraViewProj, sizeof(glm::mat4));//Renderer2D::BeginScene(camera);

            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for( auto entity : group )
            {
                auto [transform, spriteComponent] = group.get<TransformComponent, SpriteRendererComponent>(entity);
                //auto textureBindingSet = BindingSet::CreateFrameScope({{0, spriteComponent.Texture ? *spriteComponent.Texture : *m_BlankTexture}});
                InstanceBufferData data {transform.GetTransform(), spriteComponent.Color, spriteComponent.TilingFactor, (int32_t)entity};
                std::vector<BindingSet*> bindingSets {m_CameraBindingSet.get(), (spriteComponent.Texture ? spriteComponent.Texture->GetBindingSet() : m_BlankTexture->GetBindingSet())};
                Renderer::SubmitInstance(*m_RectModel, bindingSets, {(byte*)&data, sizeof(InstanceBufferData)});//Renderer2D::DrawRectangleWithID(transform.GetTransform(), spriteComponent.Color, gsl::narrow<int>(entity));
            }
        }
    }
    void Scene::UpdateEditor(EditorCamera& camera)
    {
        auto cameraViewProj = camera.GetViewProjection();
        m_CameraUniformBuffer->SetData(glm::value_ptr(cameraViewProj), sizeof(glm::mat4));//Renderer2D::BeginScene(camera);

        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for( auto entity : group )
        {
            auto [transform, spriteComponent] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            //auto textureBindingSet = BindingSet::CreateFrameScope({{0, spriteComponent.Texture ? *spriteComponent.Texture : *m_BlankTexture}});
            InstanceBufferData data {transform.GetTransform(), spriteComponent.Color, spriteComponent.TilingFactor, (int32_t)entity};
            std::vector<BindingSet*> bindingSets {m_CameraBindingSet.get(), (spriteComponent.Texture ? spriteComponent.Texture->GetBindingSet() : m_BlankTexture->GetBindingSet())};
            Renderer::SubmitInstance(*m_RectModel, bindingSets, {(byte*)&data, sizeof(InstanceBufferData)});//Renderer2D::DrawRectangleWithID(transform.GetTransform(), spriteComponent.Color, gsl::narrow<int>(entity));
        }
    }

    void Scene::StartRuntime()
    {
        /*if(m_NativeScripts == nullptr)
        {
            m_NativeScripts = &NativeScriptFactory::GetInstance().GetNativeScripts();
        }*/
        m_IsRuntime = true;
        ScriptingEngine::OnRuntimeStart(this);
    }

    void Scene::StopRuntime()
    {
        m_IsRuntime = false;
        ScriptingEngine::OnRuntimeStop();
        //DestroyScripts();
    }

    Scene::Scene()
    : m_CameraUniformBuffer(UniformBuffer::Create(sizeof(glm::mat4))), m_RectModel(&Application::GetInstance().GetAssetManager().GetModel("Rectangle")), m_BlankTexture(&Application::GetInstance().GetAssetManager().GetTexture("Blank"))
    {
        m_CameraBindingSet = BindingSet::Create({{0,*m_CameraUniformBuffer}});
    }

    void Scene::DestroyScripts()
    {
        auto view = m_Registry.view<NativeScriptComponent>();
        for (auto entity:view)
        {
            auto& scriptComponent = view.get<NativeScriptComponent>(entity);
            if (scriptComponent.Instance)
            {
                scriptComponent.Instance->OnDestroy();
                scriptComponent.DestroyScript(scriptComponent);
            }
        }
    }

    Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string &name)
    {
        Entity entity(EntityID{m_Registry.create()}, this);
        entity.AddComponent<UUIDComponent>(uuid);
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name;
        m_UUIDMap[uuid] = entity;
        return entity;
    }

    Entity Scene::GetEntityByUUID(UUID uuid)
    {
        BeeExpects(m_UUIDMap.contains(uuid));
        return {m_UUIDMap.at(uuid), this};
    }
}
