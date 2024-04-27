//
// Created by alexl on 02.06.2023.
//

#include "Entity.h"
#include "Components.h"

namespace BeeEngine
{

    UUID Entity::GetUUID()
    {
        return GetComponent<UUIDComponent>().ID;
    }

    void Entity::RemoveParent()
    {
        auto& hierarchy = GetComponent<HierarchyComponent>();
        if(!hierarchy.Parent)
            return;
        GetComponent<TransformComponent>().SetTransform(Math::ToGlobalTransform(*this));
        auto& parentHierarchy = hierarchy.Parent.GetComponent<HierarchyComponent>();
        auto it = std::find(parentHierarchy.Children.begin(), parentHierarchy.Children.end(), *this);
        BeeCoreAssert(it != parentHierarchy.Children.end(), "Entity is not a child of its parent");
        parentHierarchy.Children.erase(it);
        hierarchy.Parent = Entity::Null;
    }
    std::optional<Entity> IfEntityPresentInChildren(Entity who, Entity where)
    {
        auto& hierarchy = where.GetComponent<HierarchyComponent>();
        for (auto child : hierarchy.Children)
        {
            if(child == who)
                return child;
            auto r = IfEntityPresentInChildren(who, child);
            if(r.has_value())
                return r;
        }
        return std::nullopt;
    }
    void Entity::SetParent(Entity &parent)
    {
        if(parent == *this)
            return;
        if(parent == Entity::Null)
        {
            RemoveParent();
            return;
        }
        auto& parentHierarchy = parent.GetComponent<HierarchyComponent>();
        auto& child = *this;
        BeeExpects(parent != child);
        BeeExpects((entt::entity)parent != entt::null);
        BeeExpects((entt::entity)child != entt::null);
        auto& childHierarchy = child.GetComponent<HierarchyComponent>();
        auto isPresentedInChildren = IfEntityPresentInChildren(parent, child);
        if(isPresentedInChildren.has_value())
        {
            Entity prevParent = isPresentedInChildren.value();
            Entity currentParent = prevParent.GetComponent<HierarchyComponent>().Parent;
            while (currentParent != child)
            {
                prevParent = currentParent;
                currentParent = prevParent.GetComponent<HierarchyComponent>().Parent;
            }
            if(childHierarchy.Parent)
            {
                auto tempParent = childHierarchy.Parent;
                prevParent.SetParentWithoutChecks(tempParent);
            }
            else
            {
                prevParent.RemoveParent();
            }
        }
        child.RemoveParent();
        childHierarchy.Parent = parent;
        parentHierarchy.Children.push_back(child);
        GetComponent<TransformComponent>().SetTransform(Math::ToLocalTransform(*this));
        BeeEnsures(childHierarchy.Parent == parent);
        BeeEnsures(std::find(parentHierarchy.Children.begin(), parentHierarchy.Children.end(), child) != parentHierarchy.Children.end());
    }

    void Entity::SetParentWithoutChecks(Entity &parent)
    {
        RemoveParent();
        auto& parentHierarchy = parent.GetComponent<HierarchyComponent>();
        auto& child = *this;
        auto& childHierarchy = child.GetComponent<HierarchyComponent>();
        childHierarchy.Parent = parent;
        parentHierarchy.Children.push_back(child);
    }

    bool Entity::HasChild(Entity &child)
    {
        return IfEntityPresentInChildren(child, *this).has_value();
    }

    Entity Entity::GetParent()
    {
        return GetComponent<HierarchyComponent>().Parent;
    }

    const std::vector<Entity> &Entity::GetChildren()
    {
        return GetComponent<HierarchyComponent>().Children;
    }

    bool Entity::HasParent()
    {
        return GetComponent<HierarchyComponent>().Parent != Entity::Null;
    }
}
