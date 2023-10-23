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
#include "Prefab.h"

#include <box2d/box2d.h>
#include <box2d/b2_body.h>

namespace BeeEngine
{
    static b2BodyType ConvertRigidBodyTypeToBox2D(RigidBody2DComponent::BodyType type)
    {
        switch (type)
        {

            case RigidBody2DComponent::BodyType::Static:
                return b2BodyType::b2_staticBody;
            case RigidBody2DComponent::BodyType::Dynamic:
                return b2BodyType::b2_dynamicBody;
            case RigidBody2DComponent::BodyType::Kinematic:
                return b2BodyType::b2_kinematicBody;
        }
        BeeCoreAssert(false, "Unknown RigidBody2DComponent::BodyType");
        return b2BodyType::b2_staticBody;
    }

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
                BeeCoreTrace("Instantiating Script: {0}", scriptComponent.Name);
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
            }
        }
        for (auto e : view)
        {
            Entity entity {EntityID{e}, this};
            auto& scriptComponent = entity.GetComponent<ScriptComponent>();
            if(scriptComponent.Class)
            {
                ScriptingEngine::GetEntityScriptInstance(entity.GetUUID())->InvokeOnCreate();
            }
        }
        for( auto e: view)
        {
            Entity entity {EntityID{e}, this};
            auto& scriptComponent = entity.GetComponent<ScriptComponent>();
            if(scriptComponent.Class)
            {
                ScriptingEngine::OnEntityUpdate(entity);
            }
        }
    }

    void Scene::DestroyEntity(Entity entity)
    {
        UUID uuid = entity.GetUUID();
        auto& hierarchy = entity.GetComponent<HierarchyComponent>();
        for (auto e: hierarchy.Children)
        {
            DestroyEntity(e);
        }
        if(hierarchy.Parent)
        {
            entity.RemoveParent();
        }
        m_UUIDMap.erase(uuid);
        m_Registry.destroy(entity);
        if(m_IsRuntime)
            ScriptingEngine::OnEntityDestroyed(uuid);
    }

    void Scene::Clear()
    {
        m_Registry.clear();
    }
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
    void Scene::UpdateRuntime(const String& locale)
    {
        UpdateScripts();
        Update2DPhysics();

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
            SceneTreeRenderer renderer(cameraViewProj, m_CameraBindingSet.get());
            RenderScene(renderer, locale);

        }
    }

    void Scene::UpdateEditor(EditorCamera& camera, const String& locale)
    {
        auto cameraViewProj = camera.GetViewProjection();
        m_CameraUniformBuffer->SetData(glm::value_ptr(cameraViewProj), sizeof(glm::mat4));//Renderer2D::BeginScene(camera);
        SceneTreeRenderer renderer(cameraViewProj, m_CameraBindingSet.get());
        RenderScene(renderer, locale);
    }

    void Scene::RenderScene(SceneTreeRenderer& renderer, const String& locale)
    {
        constexpr static float epsilon = 1e-6;
        auto spriteView = m_Registry.view<SpriteRendererComponent>();
        for( auto entity : spriteView )
        {
            auto& spriteComponent = spriteView.get<SpriteRendererComponent>(entity);
            SpriteInstanceBufferData data {Math::ToGlobalTransform(Entity{entity, this}), spriteComponent.Color, spriteComponent.TilingFactor,
                                           static_cast<int32_t>(entity)+1};
            std::vector<BindingSet*> bindingSets {m_CameraBindingSet.get(), (spriteComponent.HasTexture ? spriteComponent.Texture(locale)->GetBindingSet() : m_BlankTexture->GetBindingSet())};
            renderer.AddEntity(data.Model, data.Color.A() < 0.95f || spriteComponent.HasTexture, *m_RectModel, bindingSets, {(byte*)&data, sizeof(SpriteInstanceBufferData)});
        }

        auto circleGroup = m_Registry.view<CircleRendererComponent>();
        std::vector<BindingSet*> circleBindingSets {m_CameraBindingSet.get()};
        for( auto entity : circleGroup )
        {
            auto& circleComponent = circleGroup.get<CircleRendererComponent>(entity);
            CircleInstanceBufferData data {Math::ToGlobalTransform(Entity{entity, this}), circleComponent.Color, circleComponent.Thickness, circleComponent.Fade,
                                           static_cast<int32_t>(entity)+1};
            renderer.AddEntity(data.Model, true, *m_CircleModel, circleBindingSets, {(byte*)&data, sizeof(CircleInstanceBufferData)});
        }

        auto textGroup = m_Registry.view<TextRendererComponent>();
        for( auto entity : textGroup )
        {
            auto& textComponent = textGroup.get<TextRendererComponent>(entity);
            renderer.AddText(textComponent.Text, &textComponent.Font(locale), Math::ToGlobalTransform(Entity{entity, this}), textComponent.Configuration,
                             static_cast<int32_t>(entity)+1);
        }
        renderer.Render();
    }

    void Scene::StartRuntime()
    {
        /*if(m_NativeScripts == nullptr)
        {
            m_NativeScripts = &NativeScriptFactory::GetInstance().GetNativeScripts();
        }*/
        m_IsRuntime = true;
        StartPhysicsWorld();
        ScriptingEngine::OnRuntimeStart(this);
    }

    void Scene::StopRuntime()
    {
        m_IsRuntime = false;
        ScriptingEngine::OnRuntimeStop();
        StopPhysicsWorld();
        //DestroyScripts();
    }

    Scene::Scene()
    : m_CameraUniformBuffer(UniformBuffer::Create(sizeof(glm::mat4))), m_RectModel(&Application::GetInstance().GetAssetManager().GetModel("Renderer2D_Rectangle")), m_CircleModel(&Application::GetInstance().GetAssetManager().GetModel("Renderer2D_Circle")), m_BlankTexture(&Application::GetInstance().GetAssetManager().GetTexture("Blank"))
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
        entity.AddComponent<HierarchyComponent>();
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

    Entity Scene::GetEntityByName(std::string_view name)
    {
        auto view = m_Registry.view<TagComponent>();
        for (auto entity:view)
        {
            auto& tag = view.get<TagComponent>(entity);
            if (tag.Tag == name)
            {
                return {EntityID{entity}, this};
            }
        }
        return {};
    }

    void Scene::StartPhysicsWorld()
    {
        m_2DPhysicsWorld = new b2World({0.0f, -9.81f}); //TODO: make gravity configurable

        auto view = m_Registry.view<RigidBody2DComponent>();
        for (auto e:view)
        {
            Entity entity{EntityID{e}, this};
            auto& rigidBody = entity.GetComponent<RigidBody2DComponent>();
            auto& transform = entity.GetComponent<TransformComponent>();
            b2BodyDef bodyDef;
            bodyDef.type = ConvertRigidBodyTypeToBox2D(rigidBody.Type);
            bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
            bodyDef.angle = transform.Rotation.z;

            b2Body* body = m_2DPhysicsWorld->CreateBody(&bodyDef);
            body->SetFixedRotation(rigidBody.FixedRotation);
            rigidBody.RuntimeBody = body;


            if(!entity.HasComponent<BoxCollider2DComponent>())
            {
                continue;
            }
            auto& boxCollider = entity.GetComponent<BoxCollider2DComponent>();

            if(boxCollider.Type == BoxCollider2DComponent::ColliderType::Box)
            {
                b2PolygonShape boxShape;
                boxShape.SetAsBox(boxCollider.Size.x * transform.Scale.x, boxCollider.Size.y * transform.Scale.y, b2Vec2(boxCollider.Offset.x, boxCollider.Offset.y), 0.0f);

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &boxShape;
                fixtureDef.density = boxCollider.Density;
                fixtureDef.friction = boxCollider.Friction;
                fixtureDef.restitution = boxCollider.Restitution;
                fixtureDef.restitutionThreshold = boxCollider.RestitutionThreshold;
                body->CreateFixture(&fixtureDef);
                continue;
            }

            b2CircleShape circleShape;
            circleShape.m_radius = boxCollider.Size.x * transform.Scale.x;
            circleShape.m_p.Set(boxCollider.Offset.x * transform.Scale.x, boxCollider.Offset.y * transform.Scale.y);

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &circleShape;
            fixtureDef.density = boxCollider.Density;
            fixtureDef.friction = boxCollider.Friction;
            fixtureDef.restitution = boxCollider.Restitution;
            fixtureDef.restitutionThreshold = boxCollider.RestitutionThreshold;
            body->CreateFixture(&fixtureDef);
        }
    }

    void Scene::StopPhysicsWorld()
    {
        delete m_2DPhysicsWorld;
        m_2DPhysicsWorld = nullptr;
    }

    void Scene::Update2DPhysics()
    {
        static constexpr int32_t velocityIterations = 6;
        static constexpr int32_t positionIterations = 2; //TODO: expose to editor
        m_2DPhysicsWorld->Step(gsl::narrow_cast<float>(Time::DeltaTime()), velocityIterations, positionIterations);

        auto view = m_Registry.view<RigidBody2DComponent>();
        for (auto e:view)
        {
            Entity entity{EntityID{e}, this};
            auto& rigidBody = entity.GetComponent<RigidBody2DComponent>();
            auto& transform = entity.GetComponent<TransformComponent>();
            b2Body* body = (b2Body*)rigidBody.RuntimeBody;
            if(body == nullptr)
            {
                continue;
            }
            const auto& position = body->GetPosition();
            transform.Translation = {position.x, position.y, transform.Translation.z};
            transform.Rotation.z = body->GetAngle();
        }
    }
    //Good example of variadic templates
    template<typename Component>
    static void CopyComponent(entt::registry& dst, const entt::registry& src, entt::entity srcEntity, entt::entity dstEntity)
    {
        if(!src.all_of<Component>(srcEntity))
        {
            return;
        }
        auto& component = src.get<Component>(srcEntity);
        dst.emplace_or_replace<Component>(dstEntity, component);
    }
    template<>
    static void CopyComponent<HierarchyComponent>(entt::registry& dst, const entt::registry& src, entt::entity srcEntity, entt::entity dstEntity)
    {}
    template<>
    static void CopyComponent<UUIDComponent>(entt::registry& dst, const entt::registry& src, entt::entity srcEntity, entt::entity dstEntity)
    {}

    template<typename ...Component>
    static void CopyComponents(TypeSequence<Component...>,entt::registry& dst, const entt::registry& src, entt::entity srcEntity, entt::entity dstEntity)
    {
        (CopyComponent<Component>(dst, src, srcEntity, dstEntity), ...);
    }

    Ref<Scene> Scene::Copy(Scene &scene)
    {
        Ref<Scene> newScene = CreateRef<Scene>();
        newScene->m_CameraUniformBuffer = scene.m_CameraUniformBuffer;
        newScene->m_CameraBindingSet = scene.m_CameraBindingSet;
        newScene->m_RectModel = scene.m_RectModel;
        newScene->m_BlankTexture = scene.m_BlankTexture;

        auto& srcRegistry = scene.m_Registry;
        auto& dstRegistry = newScene->m_Registry;
        auto idView = srcRegistry.view<HierarchyComponent, UUIDComponent>();
        for (auto e : idView)
        {
            if(idView.get<HierarchyComponent>(e).Parent)
            {
                continue;
            }
            UUID uuid = idView.get<UUIDComponent>(e).ID;
            const auto& name = srcRegistry.get<TagComponent>(e).Tag;
            Entity entity = scene.CopyEntity({e, &scene}, *newScene, Entity::Null, true);
        }
        return newScene;
    }

    Entity Scene::DuplicateEntity(Entity entity)
    {
        return CopyEntity(entity, *this, entity.GetComponent<HierarchyComponent>().Parent, false);
    }

    Entity Scene::CopyEntity(Entity entity, Scene &targetScene, Entity parent, bool preserveUUID)
    {
        Entity newEntity = targetScene.CreateEntityWithUUID(preserveUUID ? entity.GetUUID() : UUID{}, entity.GetComponent<TagComponent>().Tag);
        CopyComponents(AllComponents{}, targetScene.m_Registry, m_Registry, (entt::entity)entity, (entt::entity)newEntity);
        //Copy Hierarchies
        auto& hierarchy = entity.GetComponent<HierarchyComponent>();
        BeeCoreAssert(!(parent == Entity::Null && hierarchy.Parent), "Entity has parent but parent is null");
        if(parent != Entity::Null)
        {
            auto& parentHierarchy = parent.GetComponent<HierarchyComponent>();
            parentHierarchy.Children.push_back(newEntity);
            newEntity.GetComponent<HierarchyComponent>().Parent = parent;
        }
        for (auto child : hierarchy.Children)
        {
            auto newChild = CopyEntity(child, targetScene, newEntity, preserveUUID);
            newChild.GetComponent<HierarchyComponent>().Parent = newEntity;
        }
        BeeEnsures(entity.GetUUID() != newEntity.GetUUID() || preserveUUID);
        return newEntity;
    }

    Entity Scene::InstantiatePrefab(Prefab &prefab, Entity parent)
    {
        Entity newEntity = Prefab::GetPrefabScene()->CopyEntity(prefab.m_RootEntity, *this, Entity::Null, false);
        if(parent)
        {
            newEntity.SetParent(parent);
        }
        return newEntity;
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity:view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (cameraComponent.Primary)
            {
                return {EntityID{entity}, this};
            }
        }
        return Entity::Null;
    }
}
