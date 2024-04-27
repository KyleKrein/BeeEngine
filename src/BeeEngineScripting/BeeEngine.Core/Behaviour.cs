using BeeEngine.Math;
using BeeEngine.Internal;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace BeeEngine
{
    public abstract class Behaviour
    {
        //internal readonly ulong ID;
        private Entity m_Entity;

        public Entity ThisEntity
        {
            get => m_Entity;
            internal set => m_Entity = value;
        }

        public Entity? FindEntityByName(string name)
        {
            Log.Debug("Searching for entity {0}", name);
            ulong entityID = InternalCalls.Entity_FindEntityByName(name);
            if (entityID == 0)
                return null;
            return LifeTimeManager.GetEntity(entityID);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        protected T GetComponent<T>() where T : Component, new()
        {
            return ThisEntity.GetComponent<T>();
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        protected T CreateComponent<T>() where T : Component, new()
        {
            return ThisEntity.CreateComponent<T>();
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        protected bool HasComponent<T>() where T : Component, new()
        {
            return ThisEntity.HasComponent<T>();
        }

        protected Entity Instantiate(Entity entity)
        {
            ThrowIfDestroyed(entity);
            ulong newEntityId = InternalCalls.Entity_Duplicate(entity.ID);
            return LifeTimeManager.GetEntity(newEntityId);
        }

        protected Entity Instantiate(Prefab prefab, Entity? parent = null)
        {
            if (prefab == null)
                throw new ArgumentNullException(nameof(prefab));
            ulong newEntityId = InternalCalls.Entity_InstantiatePrefab(ref prefab.m_Handle, parent is null ? 0 : parent.ID);
            return LifeTimeManager.GetEntity(newEntityId);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private void ThrowIfDestroyed()
        {
            Log.AssertAndThrow(ThisEntity is null || !ThisEntity.IsAlive(), "Trying to access data of a destroyed entity");
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private void ThrowIfDestroyed(Entity entity)
        {
            Log.AssertAndThrow(entity is null || !entity.IsAlive(), "Trying to access data of a destroyed entity");
        }
    }
}