using BeeEngine.Math;
using BeeEngine.Internal;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BeeEngine
{
    public abstract class Entity
    {
        internal readonly ulong ID;
        private Dictionary<Type, Component> m_ComponentCache = new Dictionary<Type, Component>();

        internal Entity(ulong id)
        {
            Log.Info("ID set to {0}", id);
            ID = id;
        }

        protected Entity()
        { ID = 0; }

        public Vector3 Translation
        {
            get
            {
                InternalCalls.Entity_GetTranslation(ID, out var vec);
                return vec;
            }
            set
            {
                InternalCalls.Entity_SetTranslation(ID, ref value);
            }
        }

        public unsafe T CreateComponent<T>() where T : Component, new()
        {
            Type type = typeof(T);
            if (HasComponent(type))
                return GetComponent<T>();
            void* componentPtr = InternalCalls.Entity_CreateComponent(ID, type);
            T component = new T { EntityID = ID, ComponentHandle = componentPtr };
            m_ComponentCache.Add(type, component);
            return component;
        }

        public unsafe T GetComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);

            if (!HasComponent(componentType))
            {
                if (m_ComponentCache.ContainsKey(componentType))
                    m_ComponentCache.Remove(componentType);
                return null;
            }
            if (m_ComponentCache.ContainsKey(componentType))
            {
                m_ComponentCache.TryGetValue(componentType, out var component);
                DebugLog.AssertAndThrow(component != null, "Component {0} is null in dictionary", componentType.Name);
                return (T)component;
            }
            void* componentPtr = InternalCalls.Entity_GetComponent(ID, componentType);
            DebugLog.AssertAndThrow(componentPtr != (void*)0, "Component {0} is nullptr", componentType.Name);
            T cmp = new T { EntityID = ID, ComponentHandle = componentPtr };
            cmp.Contruct();
            m_ComponentCache.Add(componentType, cmp);
            return cmp;
        }

        public bool HasComponent<T>()
        {
            return InternalCalls.Entity_HasComponent(ID, typeof(T));
        }

        public bool HasComponent(Type type)
        {
            return InternalCalls.Entity_HasComponent(ID, type);
        }

        public void RemoveComponent<T>()
        {
            Type type = typeof(T);
            if (!HasComponent(type))
                return;
            InternalCalls.Entity_RemoveComponent(ID, type);
            m_ComponentCache.Remove(type);
        }
    }
}