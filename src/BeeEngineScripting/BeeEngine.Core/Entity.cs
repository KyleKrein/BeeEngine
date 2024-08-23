using BeeEngine.Events;
using BeeEngine.Internal;
using BeeEngine.Renderer;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace BeeEngine
{
    public struct MouseClickEventArgs
    {
        public Entity Entity;
        public MouseButton MouseButton;
    }
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

        /// <summary>
        /// Gets or sets the name of the entity.
        /// </summary>
        /// <exception cref="InvalidOperationException">Thrown if the entity is destroyed.</exception>
        /// <exception cref="ArgumentNullException">Thrown if the value is null.</exception>
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

        /// <summary>
        /// Gets a reference to the transform component of the entity.
        /// The transform component represents the position, rotation, and scale of the entity in the 3D space.
        /// </summary>
        /// <returns>A reference to the transform component of the entity.</returns>
        /// <exception cref="InvalidOperationException">Thrown if the entity is destroyed.</exception>
        public ref Transform Transform
        {
            get
            {
                ThrowIfDestroyed();
                return ref GetComponent<TransformComponent>()!.Transform;
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

        private static WeakEvent<MouseClickEventArgs> m_OnMouseClick = new WeakEvent<MouseClickEventArgs>();

        public static WeakEvent<MouseClickEventArgs> OnMouseClick
        {
            get
            {
                return m_OnMouseClick;
            }
            set
            {
                Log.AssertAndThrow(value != null, "OnMouseClick can't be null");
                Log.AssertAndThrow(ReferenceEquals(m_OnMouseClick, value), "This must be the same event");
            }
        }
        private static WeakEvent<Entity> m_OnMouseEnter = new WeakEvent<Entity>();

        public static WeakEvent<Entity> OnMouseEnter
        {
            get
            {
                return m_OnMouseEnter;
            }
            set
            {
                Log.AssertAndThrow(value != null, "OnMouseEnter can't be null");
                Log.AssertAndThrow(ReferenceEquals(m_OnMouseEnter, value), "This must be the same event");
            }
        }
        private static WeakEvent<Entity> m_OnMouseLeave = new WeakEvent<Entity>();
        public static WeakEvent<Entity> OnMouseLeave
        {
            get
            {
                return m_OnMouseLeave;
            }
            set
            {
                Log.AssertAndThrow(value != null, "OnMouseLeave can't be null");
                Log.AssertAndThrow(ReferenceEquals(m_OnMouseLeave, value), "This must be the same event");
            }
        }

        /// <summary>
        /// Attaches a behavior to the entity.
        /// </summary>
        /// <param name="behaviour">The behavior to attach.</param>
        /// <exception cref="InvalidOperationException">Thrown if the entity is destroyed.</exception>
        internal void SetBehaviour(Behaviour behaviour)
        {
            ThrowIfDestroyed();
            m_Behaviour = behaviour;
            if (m_Behaviour != null)
            {
                m_Behaviour.ThisEntity = this;
            }
        }

        /// <summary>
        /// Gets the attached behavior of the specified type, if any.
        /// </summary>
        /// <typeparam name="T">The type of behavior to get.</typeparam>
        /// <returns>The attached behavior of the specified type, or <c>null</c> if none is attached.</returns>
        /// <exception cref="InvalidOperationException">Thrown if the entity is destroyed.</exception>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public T? GetBehaviour<T>() where T : Behaviour
        {
            ThrowIfDestroyed();
            if (HasBehaviour<T>())
                return (T?)m_Behaviour;
            return null;
        }

        /// <summary>
        /// Checks if the entity has a behavior attached.
        /// </summary>
        /// <returns><c>true</c> if a behavior is attached; otherwise, <c>false</c>.</returns>
        /// /// <exception cref="InvalidOperationException">Thrown if the entity is destroyed.</exception>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public bool HasBehaviour()
        {
            ThrowIfDestroyed();
            return m_Behaviour != null;
        }

        /// <summary>
        /// Checks if the entity has a behavior of the specified type attached.
        /// </summary>
        /// <typeparam name="T">The type of behavior to check.</typeparam>
        /// <returns><c>true</c> if a behavior of the specified type is attached; otherwise, <c>false</c>.</returns>
        /// /// <exception cref="InvalidOperationException">Thrown if the entity is destroyed.</exception>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public bool HasBehaviour<T>() where T : Behaviour
        {
            ThrowIfDestroyed();
            return HasBehaviour() && m_Behaviour is T;
        }

        /// <summary>
        /// Creates and attaches a new component of the specified type to the entity.
        /// </summary>
        /// <typeparam name="T">The type of component to create.</typeparam>
        /// <returns>The created component.</returns>
        /// <exception cref="InvalidOperationException">Thrown if the entity is destroyed.</exception>
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
                while ((childID = InternalCalls.Entity_GetNextChild(ID, childID)) != 0)
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