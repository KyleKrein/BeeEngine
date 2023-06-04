//
// Created by alexl on 02.06.2023.
//

#pragma once

#include "EntityID.h"
#include "Scene.h"
#include "Core/Logging/Log.h"

namespace BeeEngine
{
    class Entity
    {
    public:
        constexpr Entity() = default;
        constexpr Entity(EntityID id, Scene* scene)
            : m_ID(id), m_Scene(scene)
        {}

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            BeeCoreAssert(!HasComponent<T>(), "Entity already has component!");
            return m_Scene->m_Registry.emplace<T>(m_ID, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent()
        {
            BeeCoreAssert(HasComponent<T>(), "Entity does not have component!");
            return m_Scene->m_Registry.get<T>(m_ID);
        }

        template<typename T>
        void RemoveComponent()
        {
            BeeCoreAssert(HasComponent<T>(), "Entity does not have component!");
            m_Scene->m_Registry.remove<T>(m_ID);
        }

        template<typename T>
        bool HasComponent()
        {
            return m_Scene->m_Registry.all_of<T>(m_ID);
        }

        operator bool() const { return m_ID.ID != entt::null; }
        operator EntityID() const { return m_ID; }
        operator uint32_t() const { return (uint32_t)m_ID.ID; }
        operator entt::entity() const { return (entt::entity)m_ID.ID; }

        bool operator==(const Entity& other) const { return m_ID == other.m_ID && m_Scene == other.m_Scene; }
        bool operator!=(const Entity& other) const { return !(*this == other); }

        struct EntityInit
        {
            EntityID ID;
            Scene* Scene;
            constexpr operator Entity() const { return Entity(ID, Scene); }
        };

        constexpr static EntityInit const Null{};

    private:
        EntityID m_ID { {entt::null} };
        Scene* m_Scene = nullptr;
    };
}
