using BeeEngine.Events;
using BeeEngine.Internal;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace BeeEngine
{
    /// <summary>
    /// Represents an entity in the scene.
    /// Each entity can have several components, associated with it.
    /// Each entity has a name, uuid and a hierarchy component.
    /// The name can be accessed using the Name property.
    /// The uuid can not be accessed directly, but is used internally to identify the entity.
    /// The hierarchy component can be accessed using the Parent and Children properties.
    /// To access, add or remove other components, use the CreateComponent<T>, GetComponent<T> and RemoveComponent<T> methods.
    /// Use HasComponent<T> to check if a component is attached to the entity.
    /// Each entity can have a behaviour (script) attached to it. The behaviour can be accessed using the GetBehaviour<T> method.
    /// IMPORTANT: The entity is destroyed, when the Destroy method is called. After that, the entity can not be used anymore.
    /// </summary>
    public sealed class Entity
    {
        internal readonly ulong ID;
        private Behaviour? m_Behaviour = null;

        private bool m_Destroyed = false;

        private Dictionary<Type, Component> m_ComponentCache = new Dictionary<Type, Component>();

        internal Entity(ulong id)
        {
            ID = id;
        }

        public string Name
        {
            get
            {
                ThrowIfDestroyed();
                return InternalCalls.Entity_GetName(ID);
            }
            set
            {
                ThrowIfDestroyed();
                if (value == null)
                    throw new ArgumentNullException(nameof(value));
                InternalCalls.Entity_SetName(ID, value);
            }
        }

        private WeakEvent<Entity> m_OnCollisionStart = new WeakEvent<Entity>();

        public WeakEvent<Entity> OnCollisionStart
        {
            get
            {
                ThrowIfDestroyed();
                return m_OnCollisionStart;
            }
            set
            {
                ThrowIfDestroyed();
                Log.AssertAndThrow(value != null, "OnCollisionStart can't be null");
                Log.AssertAndThrow(ReferenceEquals(m_OnCollisionStart, value), "This must be the same event");
            }
        }

        private WeakEvent<Entity> m_OnCollisionEnd = new WeakEvent<Entity>();

        public WeakEvent<Entity> OnCollisionEnd
        {
            get
            {
                ThrowIfDestroyed();
                return m_OnCollisionEnd;
            }
            set
            {
                ThrowIfDestroyed();
                Log.AssertAndThrow(value != null, "OnCollisionEnd can't be null");
                Log.AssertAndThrow(ReferenceEquals(m_OnCollisionEnd, value), "This must be the same event");
            }
        }

        internal void SetBehaviour(Behaviour behaviour)
        {
            ThrowIfDestroyed();
            m_Behaviour = behaviour;
            if (m_Behaviour != null)
            {
                m_Behaviour.ThisEntity = this;
            }
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public T? GetBehaviour<T>() where T : Behaviour
        {
            ThrowIfDestroyed();
            if (HasBehaviour<T>())
                return (T?)m_Behaviour;
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
                return GetComponent<T>()!;
            void* componentPtr = InternalCalls.Entity_CreateComponent(ID, type);
            T component = new T { EntityID = ID, ComponentHandle = componentPtr };
            m_ComponentCache.Add(type, component);
            return component;
        }

        public unsafe T? GetComponent<T>() where T : Component, new()
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
                return (T?)component;
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

        public Entity Parent
        {
            get
            {
                ThrowIfDestroyed();
                ulong id = InternalCalls.Entity_GetParent(ID);
                if (id == 0)
                {
                    return null;
                }

                return LifeTimeManager.GetEntity(id);
            }
            set
            {
                ThrowIfDestroyed();
                ulong parentID = value == null ? 0 : value.ID;
                InternalCalls.Entity_SetParent(ID, parentID);
            }
        }

        public IEnumerable<Entity> Children
        {
            get
            {
                ThrowIfDestroyed();
                ulong childID = 0;
                while ((childID = InternalCalls.Entity_GetNextChild(ID, childID))!= 0)
                {
                    ThrowIfDestroyed();
                    yield return LifeTimeManager.GetEntity(childID);
                }
            }
        }

        public void AddChild(Entity child)
        {
            ThrowIfDestroyed();
            if (child is null)
                throw new ArgumentException("Child can't be null");
            InternalCalls.Entity_AddChild(ID, child.ID);
        }

        public void RemoveChild(Entity child)
        {
            ThrowIfDestroyed();
            if (child is null)
                throw new ArgumentException("Child can't be null");
            InternalCalls.Entity_RemoveChild(ID, child.ID);
        }

        public bool HasChild(Entity child)
        {
            ThrowIfDestroyed();
            if (child is null)
                throw new ArgumentException("Child can't be null");
            return InternalCalls.Entity_HasChild(ID, child.ID);
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