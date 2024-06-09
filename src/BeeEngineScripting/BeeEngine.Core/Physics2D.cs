using BeeEngine.Math;
using System;
using System.Collections.Generic;
using BeeEngine.Internal;
using System.Net;

namespace BeeEngine
{
    public static class Physics2D
    {
        public static Entity? CastRay(Vector2 startPoint, Vector2 endPoint)
        {
            ulong id = InternalCalls.Physics2D_CastRay(ref startPoint, ref endPoint);
            if (id == 0)
            {
                return null;
            }
            return LifeTimeManager.GetEntity(id);
        }
        //To call from C++
        internal static void OnCollisionStart(ulong entity1, ulong entity2)
        {
            var entity1Obj = LifeTimeManager.GetEntity(entity1);
            var entity2Obj = LifeTimeManager.GetEntity(entity2);
            DebugLog.Debug($"Collision Start: {entity1Obj.Name} and {entity2Obj.Name}");
            entity1Obj.OnCollisionStart.Invoke(entity1Obj, entity2Obj);
            entity2Obj.OnCollisionStart.Invoke(entity2Obj, entity1Obj);
        }
        //To call from C++
        internal static void OnCollisionEnd(ulong entity1, ulong entity2)
        {
            var entity1Obj = LifeTimeManager.GetEntity(entity1);
            var entity2Obj = LifeTimeManager.GetEntity(entity2);
            DebugLog.Debug($"Collision End: {entity1Obj.Name} and {entity2Obj.Name}");
            entity1Obj.OnCollisionEnd.Invoke(entity1Obj, entity2Obj);
            entity2Obj.OnCollisionEnd.Invoke(entity2Obj, entity1Obj);
        }

        //To call from C++
        //entity1 - primary camera
        //entity2 - entity, that was clicked by the player
        internal static void OnMouseClick(ulong entity1, ulong entity2, int button)
        {
            var entity1Obj = LifeTimeManager.GetEntity(entity1);
            var entity2Obj = LifeTimeManager.GetEntity(entity2);
            var eventArgs = new MouseClickEventArgs { Entity = entity2Obj, MouseButton = (MouseButton)button };
            Entity.OnMouseClick.Invoke(entity1Obj, eventArgs);
        }
        //To call from C++
        //entity1 - primary camera
        //entity2 - entity, that was clicked by the player
        internal static void OnMouseEnter(ulong entity1, ulong entity2)
        {
            var entity1Obj = LifeTimeManager.GetEntity(entity1);
            var entity2Obj = LifeTimeManager.GetEntity(entity2);
            Entity.OnMouseEnter.Invoke(entity1Obj, entity2Obj);
        }
        //To call from C++
        //entity1 - primary camera
        //entity2 - entity, that was clicked by the player
        internal static void OnMouseLeave(ulong entity1, ulong entity2)
        {
            var entity1Obj = LifeTimeManager.GetEntity(entity1);
            var entity2Obj = LifeTimeManager.GetEntity(entity2);
            Entity.OnMouseLeave.Invoke(entity1Obj, entity2Obj);
        }
    }
}
