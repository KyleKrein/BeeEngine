//
// Created by alexl on 02.06.2023.
//

#include "Scene.h"
#include "Components.h"
#include "Core/Application.h"
#include "Core/CodeSafety/Expects.h"
#include "Core/UUID.h"
#include "Entity.h"
#include "NativeScriptFactory.h"
#include "Prefab.h"
#include "Renderer/Renderer.h"
#include "Scripting/ScriptingEngine.h"
#include "gtc/type_ptr.hpp"
#include <glm/glm.hpp>

#include <box2d/b2_body.h>
#include <box2d/box2d.h>

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
        for (auto entity : nativeScriptsView)
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
#if defined(BEE_ENABLE_SCRIPTING)
        auto view = m_Registry.view<ScriptComponent>();
        for (auto e : view)
        {
            Entity entity{EntityID{e}, this};
            auto& scriptComponent = entity.GetComponent<ScriptComponent>();
            if (scriptComponent.Class)
            {
                ScriptingEngine::OnEntityCreated(entity, scriptComponent.Class);
            }
        }
        for (auto e : view)
        {
            Entity entity{EntityID{e}, this};
            auto& scriptComponent = entity.GetComponent<ScriptComponent>();
            if (scriptComponent.Class)
            {
                ScriptingEngine::GetEntityScriptInstance(entity.GetUUID())->InvokeOnCreate();
            }
        }
        for (auto e : view)
        {
            Entity entity{EntityID{e}, this};
            auto& scriptComponent = entity.GetComponent<ScriptComponent>();
            if (scriptComponent.Class)
            {
                ScriptingEngine::OnEntityUpdate(entity);
            }
        }
#endif
    }

    void Scene::DestroyEntity(Entity entity)
    {
        UUID uuid = entity.GetUUID();
        auto& hierarchy = entity.GetComponent<HierarchyComponent>();
        for (auto e : hierarchy.Children)
        {
            DestroyEntity(e);
        }
        if (hierarchy.Parent)
        {
            entity.RemoveParent();
        }
        m_UUIDMap.erase(uuid);
        m_Registry.destroy(entity);
#if defined(BEE_ENABLE_SCRIPTING)
        if (m_IsRuntime)
            ScriptingEngine::OnEntityDestroyed(uuid);
#endif
    }

    void Scene::Clear()
    {
        m_Registry.clear();
    }

    void Scene::UpdateRuntime()
    {
        UpdateScripts();
        Update2DPhysics();
    }

    void Scene::StartRuntime()
    {
        /*if(m_NativeScripts == nullptr)
        {
            m_NativeScripts = &NativeScriptFactory::GetInstance().GetNativeScripts();
        }*/
        m_IsRuntime = true;
        StartPhysicsWorld();
#if defined(BEE_ENABLE_SCRIPTING)
        ScriptingEngine::OnRuntimeStart(this);
#endif
    }

    void Scene::StopRuntime()
    {
        m_IsRuntime = false;
#if defined(BEE_ENABLE_SCRIPTING)
        ScriptingEngine::OnRuntimeStop();
#endif
        StopPhysicsWorld();
        // DestroyScripts();
    }

    Scene::Scene() = default;

    void Scene::DestroyScripts()
    {
        auto view = m_Registry.view<NativeScriptComponent>();
        for (auto entity : view)
        {
            auto& scriptComponent = view.get<NativeScriptComponent>(entity);
            if (scriptComponent.Instance)
            {
                scriptComponent.Instance->OnDestroy();
                scriptComponent.DestroyScript(scriptComponent);
            }
        }
    }

    Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
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
        for (auto entity : view)
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
        m_2DPhysicsWorld = new b2World({0.0f, -9.81f}); // TODO: make gravity configurable

        auto view = m_Registry.view<RigidBody2DComponent>();
        for (auto e : view)
        {
            Entity entity{EntityID{e}, this};
            auto& rigidBody = entity.GetComponent<RigidBody2DComponent>();
            auto& transform = entity.GetComponent<TransformComponent>();
            b2Body* body = (b2Body*)CreateRuntimeRigidBody2D(rigidBody, transform);

            if (!entity.HasComponent<BoxCollider2DComponent>())
            {
                continue;
            }
            auto& boxCollider = entity.GetComponent<BoxCollider2DComponent>();

            CreateRuntimeBoxCollider2DFixture(transform, body, boxCollider);
        }
    }

    void Scene::CreateRuntimeBoxCollider2DFixture(const TransformComponent& transform,
                                                  b2Body* body,
                                                  BoxCollider2DComponent& boxCollider) const
    {
        if (boxCollider.Type == BoxCollider2DComponent::ColliderType::Box)
        {
            b2PolygonShape boxShape;
            boxShape.SetAsBox(boxCollider.Size.x * transform.Scale.x,
                              boxCollider.Size.y * transform.Scale.y,
                              b2Vec2(boxCollider.Offset.x, boxCollider.Offset.y),
                              0.0f);

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &boxShape;
            fixtureDef.density = boxCollider.Density;
            fixtureDef.friction = boxCollider.Friction;
            fixtureDef.restitution = boxCollider.Restitution;
            fixtureDef.restitutionThreshold = boxCollider.RestitutionThreshold;
            boxCollider.RuntimeFixture = static_cast<void*>(body->CreateFixture(&fixtureDef));
        }
        else if (boxCollider.Type == BoxCollider2DComponent::ColliderType::Circle)
        {
            b2CircleShape circleShape;
            circleShape.m_radius = boxCollider.Size.x * transform.Scale.x;
            circleShape.m_p.Set(boxCollider.Offset.x * transform.Scale.x, boxCollider.Offset.y * transform.Scale.y);

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &circleShape;
            fixtureDef.density = boxCollider.Density;
            fixtureDef.friction = boxCollider.Friction;
            fixtureDef.restitution = boxCollider.Restitution;
            fixtureDef.restitutionThreshold = boxCollider.RestitutionThreshold;
            boxCollider.RuntimeFixture = static_cast<void*>(body->CreateFixture(&fixtureDef));
        }
    }

    void* Scene::CreateRuntimeRigidBody2D(RigidBody2DComponent& rigidBody, const TransformComponent& transform)
    {
        b2BodyDef bodyDef;
        bodyDef.type = ConvertRigidBodyTypeToBox2D(rigidBody.Type);
        bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
        bodyDef.angle = transform.Rotation.z;

        b2Body* body = m_2DPhysicsWorld->CreateBody(&bodyDef);
        body->SetFixedRotation(rigidBody.FixedRotation);
        rigidBody.RuntimeBody = body;
        return body;
    }

    void Scene::StopPhysicsWorld()
    {
        delete m_2DPhysicsWorld;
        m_2DPhysicsWorld = nullptr;
    }

    void Scene::Update2DPhysics()
    {
        static constexpr int32_t velocityIterations = 6;
        static constexpr int32_t positionIterations = 2; // TODO: expose to editor
        m_2DPhysicsWorld->Step(
            gsl::narrow_cast<float>(Time::secondsD(Time::DeltaTime()).count()), velocityIterations, positionIterations);

        auto view = m_Registry.view<RigidBody2DComponent>();
        for (auto e : view)
        {
            Entity entity{EntityID{e}, this};
            auto& rigidBody = entity.GetComponent<RigidBody2DComponent>();
            auto& transform = entity.GetComponent<TransformComponent>();
            b2Body* body = (b2Body*)rigidBody.RuntimeBody;
            if (body == nullptr)
            {
                body = (b2Body*)CreateRuntimeRigidBody2D(rigidBody, transform);
            }
            if (entity.HasComponent<BoxCollider2DComponent>())
            {
                auto& boxCollider = entity.GetComponent<BoxCollider2DComponent>();
                if (!boxCollider.RuntimeFixture)
                {
                    CreateRuntimeBoxCollider2DFixture(transform, body, boxCollider);
                }
            }
            const auto& position = body->GetPosition();
            transform.Translation = {position.x, position.y, transform.Translation.z};
            transform.Rotation.z = body->GetAngle();
        }
    }
    // Good example of variadic templates
    template <typename Component>
    static void
    CopyComponent(entt::registry& dst, const entt::registry& src, entt::entity srcEntity, entt::entity dstEntity)
    {
        if (!src.all_of<Component>(srcEntity))
        {
            return;
        }
        auto& component = src.get<Component>(srcEntity);
        dst.emplace_or_replace<Component>(dstEntity, component);
    }
    template <>
    static void CopyComponent<HierarchyComponent>(entt::registry& dst,
                                                  const entt::registry& src,
                                                  entt::entity srcEntity,
                                                  entt::entity dstEntity)
    {
    }
    template <>
    static void CopyComponent<UUIDComponent>(entt::registry& dst,
                                             const entt::registry& src,
                                             entt::entity srcEntity,
                                             entt::entity dstEntity)
    {
    }

    template <typename... Component>
    static void CopyComponents(TypeSequence<Component...>,
                               entt::registry& dst,
                               const entt::registry& src,
                               entt::entity srcEntity,
                               entt::entity dstEntity)
    {
        (CopyComponent<Component>(dst, src, srcEntity, dstEntity), ...);
    }

    Ref<Scene> Scene::Copy(Scene& scene)
    {
        Ref<Scene> newScene = CreateRef<Scene>();

        auto& srcRegistry = scene.m_Registry;
        auto& dstRegistry = newScene->m_Registry;
        auto idView = srcRegistry.view<HierarchyComponent, UUIDComponent>();
        for (auto e : idView)
        {
            if (idView.get<HierarchyComponent>(e).Parent)
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

    Entity Scene::CopyEntity(Entity entity, Scene& targetScene, Entity parent, bool preserveUUID)
    {
        Entity newEntity = targetScene.CreateEntityWithUUID(preserveUUID ? entity.GetUUID() : UUID{},
                                                            entity.GetComponent<TagComponent>().Tag);
        CopyComponents(
            AllComponents{}, targetScene.m_Registry, m_Registry, (entt::entity)entity, (entt::entity)newEntity);
        // Copy Hierarchies
        auto& hierarchy = entity.GetComponent<HierarchyComponent>();
        BeeCoreAssert(!(parent == Entity::Null && hierarchy.Parent), "Entity has parent but parent is null");
        if (parent != Entity::Null)
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

    Entity Scene::InstantiatePrefab(Prefab& prefab, Entity parent)
    {
        Entity newEntity = Prefab::GetPrefabScene()->CopyEntity(prefab.m_RootEntity, *this, Entity::Null, false);
        if (parent)
        {
            newEntity.SetParent(parent);
        }
        return newEntity;
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (cameraComponent.Primary)
            {
                return {EntityID{entity}, this};
            }
        }
        return Entity::Null;
    }
    class RayCast2DCallback : public b2RayCastCallback
    {
    public:
        float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override
        {
            m_Fixture = fixture;
            m_Point = point;
            m_Normal = normal;
            m_Fraction = fraction;
            return fraction;
        }
        b2Fixture* GetFixture() const { return m_Fixture; }
        b2Vec2 GetPoint() const { return m_Point; }
        b2Vec2 GetNormal() const { return m_Normal; }
        float GetFraction() const { return m_Fraction; }

    private:
        b2Fixture* m_Fixture = nullptr;
        b2Vec2 m_Point;
        b2Vec2 m_Normal;
        float m_Fraction;
    };

    Entity Scene::RayCast2D(glm::vec2 start, glm::vec2 end)
    {
        RayCast2DCallback callback;
        m_2DPhysicsWorld->RayCast(&callback, {start.x, start.y}, {end.x, end.y});
        if (auto fixture = callback.GetFixture())
        {
            auto view = m_Registry.view<BoxCollider2DComponent>();
            auto it = std::ranges::find_if(view,
                                           [fixture, &view](auto entity)
                                           {
                                               auto& boxCollider = view.get<BoxCollider2DComponent>(entity);
                                               return boxCollider.RuntimeFixture == fixture;
                                           });
            if (it != view.end())
            {
                return {EntityID{*it}, this};
            }
        }
        return Entity::Null;
    }
} // namespace BeeEngine
