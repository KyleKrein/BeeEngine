using BeeEngine;
using BeeEngine.Math;
using System;

namespace Example
{
	public class EntityGenerator: Behaviour
	{
        public Prefab EntityPrefab;
        public int X = 0;
        public int Y = 0;
        void OnCreate()
        {
            if (EntityPrefab is null)
                return;
            for (int x = 0; x < X; x++)
            {
                for (int y = 0; y < Y; y++)
                {
                    Entity newEntity = Instantiate(EntityPrefab, ThisEntity);
                    var transform = newEntity.GetComponent<TransformComponent>();
                    transform.Translation.X = x;
                    transform.Translation.Y = y;
                }
            }
        }
    }
}
