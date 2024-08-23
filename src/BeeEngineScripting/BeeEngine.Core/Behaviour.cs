using BeeEngine.Math;
using BeeEngine.Internal;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;
using BeeEngine.Renderer;

namespace BeeEngine
{
    /// <summary>
    /// Represents the base class for all behaviors that can be attached to entities in the BeeEngine.
    /// Provides methods for interacting with components and managing entities.
    /// </summary>
    public abstract class Behaviour
    {
        private Entity m_Entity;

        /// <summary>
        /// Gets the entity that this behavior is attached to.
        /// </summary>
        public Entity ThisEntity
        {
            get => m_Entity;
            internal set => m_Entity = value;
        }

        /// <summary>
        /// Finds an entity in the scene by its name.
        /// </summary>
        /// <param name="name">The name of the entity to find.</param>
        /// <returns>The entity with the specified name, or <c>null</c> if no entity with that name is found.</returns>
        public Entity? FindEntityByName(string name)
        {
            Log.Debug("Searching for entity {0}", name);
            ulong entityID = InternalCalls.Entity_FindEntityByName(name);
            if (entityID == 0)
                return null;
            return LifeTimeManager.GetEntity(entityID);
        }

        /// <summary>
        /// Retrieves the component of type <typeparamref name="T"/> attached to the current entity.
        /// </summary>
        /// <typeparam name="T">The type of the component to retrieve.</typeparam>
        /// <returns>The component of the specified type.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        protected T GetComponent<T>() where T : Component, new()
        {
            return ThisEntity.GetComponent<T>();
        }

        /// <summary>
        /// Creates a new component of type <typeparamref name="T"/> and attaches it to the current entity.
        /// </summary>
        /// <typeparam name="T">The type of the component to create.</typeparam>
        /// <returns>The created component.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        protected T CreateComponent<T>() where T : Component, new()
        {
            return ThisEntity.CreateComponent<T>();
        }

        /// <summary>
        /// Checks if the current entity has a component of type <typeparamref name="T"/>.
        /// </summary>
        /// <typeparam name="T">The type of the component to check.</typeparam>
        /// <returns><c>true</c> if the entity has the component; otherwise, <c>false</c>.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        protected bool HasComponent<T>() where T : Component, new()
        {
            return ThisEntity.HasComponent<T>();
        }

        /// <summary>
        /// Provides a reference to the transform component of the current entity.
        /// The transform component represents the position, rotation, and scale of the entity in the 3D space.
        /// </summary>
        /// <returns>A reference to the transform component of the current entity.</returns>
        /// /// <exception cref="InvalidOperationException">Thrown if this entity is destroyed.</exception>
        protected ref Transform Transform => ref ThisEntity.Transform;

        /// <summary>
        /// Instantiates a copy of the specified entity.
        /// </summary>
        /// <param name="entity">The entity to instantiate.</param>
        /// <returns>The newly instantiated entity.</returns>
        /// <exception cref="InvalidOperationException">Thrown if the entity has been destroyed.</exception>
        protected Entity Instantiate(Entity entity)
        {
            ThrowIfDestroyed(entity);
            ulong newEntityId = InternalCalls.Entity_Duplicate(entity.ID);
            return LifeTimeManager.GetEntity(newEntityId);
        }

        /// <summary>
        /// Instantiates an entity from the specified prefab.
        /// </summary>
        /// <param name="prefab">The prefab to instantiate.</param>
        /// <param name="parent">The optional parent entity to attach the instantiated entity to.</param>
        /// <returns>The newly instantiated entity.</returns>
        /// <exception cref="ArgumentNullException">Thrown if the prefab is null.</exception>
        protected Entity Instantiate(Prefab prefab, Entity? parent = null)
        {
            if (prefab == null)
                throw new ArgumentNullException(nameof(prefab));
            ulong newEntityId = InternalCalls.Entity_InstantiatePrefab(ref prefab.m_Handle, parent is null ? 0 : parent.ID);
            return LifeTimeManager.GetEntity(newEntityId);
        }

        /// <summary>
        /// Throws an exception if the current entity has been destroyed.
        /// </summary>
        /// <exception cref="InvalidOperationException">Thrown if the entity has been destroyed.</exception>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private void ThrowIfDestroyed()
        {
            Log.AssertAndThrow(ThisEntity is null || !ThisEntity.IsAlive(), "Trying to access data of a destroyed entity");
        }

        /// <summary>
        /// Throws an exception if the specified entity has been destroyed.
        /// </summary>
        /// <param name="entity">The entity to check.</param>
        /// <exception cref="InvalidOperationException">Thrown if the entity has been destroyed.</exception>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private void ThrowIfDestroyed(Entity entity)
        {
            Log.AssertAndThrow(entity is null || !entity.IsAlive(), "Trying to access data of a destroyed entity");
        }
    }
}
