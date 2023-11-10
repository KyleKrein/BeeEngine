using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeeEngine.Internal;
using BeeEngine.Math;

namespace BeeEngine
{
    public static class Input
    {
        public static Vector2 MouseCoords
        {
            get
            {
                Vector2 coords = new Vector2();
                InternalCalls.Input_GetMousePosition(ref coords);
                return coords;
            }
        }

        public static Vector2 GetMouseCoordsInWorldSpace(Entity camera)
        {
            if(camera == null)
                throw new ArgumentNullException(nameof(camera));
            if (!camera.IsAlive())
                throw new ArgumentException("Entity was already destroyed");
            Vector2 coords = new Vector2();
            InternalCalls.Input_GetMousePositionInWorldSpace(camera.ID, ref coords);
            return coords;
        }

        public static bool IsKeyDown(Key key)
        {
            return InternalCalls.Input_IsKeyDown(key);
        }

        public static bool IsMouseButtonDown(MouseButton button)
        {
            return InternalCalls.Input_IsMouseButtonDown(button);
        }
    }
}