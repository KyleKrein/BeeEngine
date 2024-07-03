//
// Created by alexl on 02.06.2023.
//

#pragma once

#include "Core/CodeSafety/Expects.h"
#include "Core/Logging/Log.h"
#include "Core/TypeDefines.h"
#include "Core/UUID.h"
#include "EntityID.h"
#include "Scene.h"
// #include "Components.h"

namespace BeeEngine
{
    class Entity
    {
        friend class PrefabImporter;
        friend class Scene;

    public:
        constexpr Entity() = default;
        Entity(EntityID id, WeakRef<Scene> scene) : m_ID(id), m_Scene(std::move(scene)) {}

        UUID GetUUID();

        template <typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            BeeExpects(IsValid());
            BeeCoreAssert(!HasComponent<T>(), "Entity already has component!");
            return m_Scene.lock()->m_Registry.emplace<T>(m_ID, std::forward<Args>(args)...);
        }

        template <typename T>
        T& GetComponent()
        {
            BeeExpects(IsValid());
            BeeCoreAssert(HasComponent<T>(), "Entity does not have component!");
            return m_Scene.lock()->m_Registry.get<T>(m_ID);
        }

        template <typename T>
        void RemoveComponent()
        {
            BeeExpects(IsValid());
            BeeCoreAssert(HasComponent<T>(), "Entity does not have component!");
            m_Scene.lock()->m_Registry.remove<T>(m_ID);
        }

        template <typename T>
        bool HasComponent()
        {
            BeeExpects(IsValid());
            return m_Scene.lock()->m_Registry.all_of<T>(m_ID);
        }

        void Destroy()
        {
            BeeExpects(IsValid());
            BeeCoreAssert(*this != Entity::Null, "Entity is null!");
            m_Scene.lock()->DestroyEntity(*this);
            m_ID = Null.ID;
            m_Scene = Null.Scene;
        }
        [[nodiscard]] bool IsValid() const { return !m_Scene.expired() && m_Scene.lock()->IsEntityValid(*this); }

        void RemoveParent();
        void SetParent(Entity& parent);
        bool HasChild(Entity& child);
        Entity GetParent();
        bool HasParent();
        const std::vector<Entity>& GetChildren();
        // Add check for validity?
        operator bool() const { return m_ID.ID != entt::null; }
        operator EntityID() const { return m_ID; }
        operator uint32_t() const { return (uint32_t)m_ID.ID; }
        operator entt::entity() const { return (entt::entity)m_ID.ID; }

        bool operator==(const Entity& other) const
        {
            return m_ID == other.m_ID && ((m_Scene.expired() && other.m_Scene.expired()) ||
                                          (m_Scene.lock().get() == other.m_Scene.lock().get()));
        }
        bool operator!=(const Entity& other) const { return !(*this == other); }

        struct EntityInit
        {
            EntityID ID;
            WeakRef<Scene> Scene;
            operator Entity() const { return Entity(ID, Scene); }
        };

        template <typename Archive>
        void Serialize(Archive& serializer)
        {
            serializer& GetUUID();
        }

        inline static EntityInit const Null{};

    private:
        EntityID m_ID{};
        WeakRef<Scene> m_Scene{};
        void SetParentWithoutChecks(Entity& parent);
    };
} // namespace BeeEngine
