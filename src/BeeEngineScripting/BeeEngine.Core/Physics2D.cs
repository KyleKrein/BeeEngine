using BeeEngine.Math;
using System;
using System.Collections.Generic;
using BeeEngine.Internal;

namespace BeeEngine
{
    public static class Physics2D
    {
        public static Entity? CastRay(Vector2 startPoint, Vector2 endPoint)
        {
            ulong id = InternalCalls.Physics2D_CastRay(ref startPoint, ref endPoint);
            if(id == 0)
            {
                return null;
            }
            return LifeTimeManager.GetEntity(id);
        }
    }
}
