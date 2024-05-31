//
// Created by alexl on 03.06.2023.
//

#pragma once

#include "Entity.h"

namespace BeeEngine
{
    class ScriptableEntity
    {
    public:
        virtual ~ScriptableEntity() = default;

        template <typename T>
        T& GetComponent()
        {
            return m_Entity.GetComponent<T>();
        }

    protected:
        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate() {}

    private:
        Entity m_Entity;
        friend class Scene;
    };
} // namespace BeeEngine
