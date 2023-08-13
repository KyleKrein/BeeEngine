using BeeEngine;
using System;
using System.Security.Cryptography;

namespace Example
{
    public class Destroyer : Behaviour
    {
        private Entity enemy = null;

        private void OnCreate()
        {
            Log.Info("OnCreate: {0}", nameof(Destroyer));
            enemy = FindEntityByName("Mouth");
        }

        private void OnUpdate()
        {
            if (enemy == null)
            {
                return;
            }

            if (!enemy.IsAlive())
            {
                enemy = null;
                return;
            }
            if (Input.IsKeyDown(Key.Space))
            {
                enemy.Destroy();
                FindEntityByName("Score").GetBehaviour<SetScore>().IncreaseScore();
            }
        }

        private void OnDestroy()
        {
            Log.Info("OnDestroy: {0}", nameof(Destroyer));
        }
    }
}