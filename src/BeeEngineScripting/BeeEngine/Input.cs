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
        private static bool IsKeyDown(Key key)
        {
            return InternalCalls.Input_IsKeyDown(key);
        }

        private static bool IsMouseButtonDown(MouseButton button)
        {
            return InternalCalls.Input_IsMouseButtonDown(button);
        }
    }
}