using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace BeeEngine.Internal
{
    internal static class LifeTimeManager
    {
        private static Dictionary<ulong, Entity> s_Entities = new();
        private static Dictionary<ulong, ulong> s_EnttIdsCache = new();


        internal static Entity GetEntity(ulong entityId)
        {
            CreateEntityIfNotExists(entityId);
            return s_Entities[entityId];
        }

        internal static ulong GetEntityEnttID(Entity entity)
        {
            if (s_EnttIdsCache.ContainsKey(entity.ID))
            {
                return s_EnttIdsCache[entity.ID];
            }
            ulong enttID = InternalCalls.Entity_GetEnttID(entity.ID);
            s_EnttIdsCache[entity.ID] = enttID;
            return enttID;
        }

        //For calling from C++
        private static void AddEntityScript(ulong entityID, Behaviour behaviour)
        {
            CreateEntityIfNotExists(entityID);
            Log.Info("Adding Entity {0}", entityID);
            s_Entities[entityID].SetBehaviour(behaviour);
            DebugLog.AssertAndThrow(behaviour == null || s_Entities[entityID].HasBehaviour(), "Behaviour for {0} is not set!", entityID);
        }

        //For calling from C++
        private static void EntityWasRemoved(ulong entityID)
        {
            Log.Info("Removing Entity {0}", entityID);
            s_Entities[entityID].EntityWasRemoved();
            s_Entities.Remove(entityID);
            /*
             * TODO: need to test if invalidation of components at destruction is
             * more preferable than copying data from c++ every time you need access
             * to data in components
             *
             * If invalidation is better, then it's a MUST to firstly destroy
             * every object pending for destruction and then invalidate once
             */
            DebugLog.Debug("Alive: {0}", s_Entities.Count);
            foreach (var entity in s_Entities)
            {
                entity.Value.Invalidate();
            }
        }

        //For calling from C++
        private static void EndScene()
        {
            Log.Info("Finishing Scene");
            s_Entities.Clear();
            s_EnttIdsCache.Clear();
            Localization.ClearCache();
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private static void CreateEntityIfNotExists(ulong entityId)
        {
            if (!s_Entities.ContainsKey(entityId))
            {
                //Log.Debug("Unable to found {0} in C#. Creating...", entityId);
                s_Entities[entityId] = new Entity(entityId);
            }
        }
    }
}