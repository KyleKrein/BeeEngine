using BeeEngine.Internal;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace BeeEngine
{
    public sealed class Entity
    {
        internal readonly ulong ID;
        private Behaviour m_Behaviour = null;

        private bool m_Destroyed = false;

        private Dictionary<Type, Component> m_ComponentCache = new Dictionary<Type, Component>();

        internal Entity(ulong id)
        {
            ID = id;
        }

        internal void SetBehaviour(Behaviour behaviour)
        {
            ThrowIfDestroyed();
            m_Behaviour = behaviour;
            if (m_Behaviour != null)
            {
                m_Behaviour.ThisEntity = this;
                DebugLog.Debug("Setting behaviour script for {0}", ID);
            }
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public T GetBehaviour<T>() where T : Behaviour
        {
            ThrowIfDestroyed();
            if (HasBehaviour<T>())
                return (T)m_Behaviour;
            return null;
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public bool HasBehaviour()
        {
            ThrowIfDestroyed();
            return m_Behaviour != null;
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public bool HasBehaviour<T>() where T : Behaviour
        {
            ThrowIfDestroyed();
            return HasBehaviour() && m_Behaviour is T;
        }

        public unsafe T CreateComponent<T>() where T : Component, new()
        {
            ThrowIfDestroyed();
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
            ThrowIfDestroyed();
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
            cmp.Construct();
            m_ComponentCache.Add(componentType, cmp);
            return cmp;
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public bool HasComponent<T>()
        {
            ThrowIfDestroyed();
            return InternalCalls.Entity_HasComponent(ID, typeof(T));
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public bool HasComponent(Type type)
        {
            ThrowIfDestroyed();
            return InternalCalls.Entity_HasComponent(ID, type);
        }

        public void RemoveComponent<T>()
        {
            ThrowIfDestroyed();
            Type type = typeof(T);
            if (!HasComponent(type))
                return;
            InternalCalls.Entity_RemoveComponent(ID, type);
            m_ComponentCache.Remove(type);
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public void Destroy()
        {
            ThrowIfDestroyed();
            InternalCalls.Entity_Destroy(ID);
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public bool IsAlive()
        {
            return !m_Destroyed;
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private void ThrowIfDestroyed()
        {
            Log.AssertAndThrow(!m_Destroyed, "Trying to access data of a destroyed entity");
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        internal void EntityWasRemoved()
        {
            m_Destroyed = true;
            if (m_Behaviour != null)
            {
                m_Behaviour.ThisEntity = null;
            }
            foreach (var component in m_ComponentCache)
            {
                component.Value.Destroyed = true;
            }
            m_ComponentCache.Clear();
        }

        internal unsafe void Invalidate()
        {
            foreach (var component in m_ComponentCache)
            {
                component.Value.ComponentHandle = InternalCalls.Entity_GetComponent(ID, component.Key);
                component.Value.Construct();
            }
        }
    }
}