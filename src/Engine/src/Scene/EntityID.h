//
// Created by alexl on 02.06.2023.
//

#pragma once
#include "entt/entt.hpp"
struct EntityID
{
    constexpr EntityID()
    : ID(entt::null)
    {}
    constexpr explicit EntityID(entt::entity id) : ID(id) {}
    entt::entity ID;
    operator entt::entity() const { return ID; }
    operator entt::entity&() { return ID; }
    operator const entt::entity&() const { return ID; }
    bool operator == (const EntityID& other) const { return ID == other.ID; }
    bool operator != (const EntityID& other) const { return ID != other.ID; }
    EntityID& operator =(uint32_t id) { ID = static_cast<entt::entity>(id); return *this;}
    EntityID& operator =(entt::entity id) { ID = id; return *this;}
};