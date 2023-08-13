using BeeEngine;
using System;

namespace Example
{
    public class Helper : Behaviour
    {
        private void OnUpdate()
        {
            if (Input.IsKeyDown(Key.P))
            {
                throw new Exception("Warum ist die Banane krum?");
            }

            if (Input.IsKeyDown(Key.O))
            {
                GC.Collect();
            }
        }
    }
}