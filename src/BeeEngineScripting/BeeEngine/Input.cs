using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeeEngine.Internal;

namespace BeeEngine
{
    public static class Input
    {
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