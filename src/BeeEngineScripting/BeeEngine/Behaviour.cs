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

        public Entity FindEntityByName(string name)
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
    }
}