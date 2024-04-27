using BeeEngine;
using System;

namespace Example
{
	public sealed class BeeGenerator: Behaviour
	{
		public Prefab BeePrefab;
		public int BeesX = 0;
		public int BeesY = 0;
		void OnCreate()
		{
			if (BeePrefab is null)
				return;
			for (int x = 0; x < BeesX; x++)
			{
				for(int y = 0; y < BeesY; y++)
				{
					Entity newBee = Instantiate(BeePrefab, ThisEntity);
					var transform = newBee.GetComponent<TransformComponent>();
					transform.Translation.X = x;
					transform.Translation.Y = y;
				}
			}
		}
	}
}
