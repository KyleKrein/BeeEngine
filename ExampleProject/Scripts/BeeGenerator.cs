using BeeEngine;
using BeeEngine.Math;
using BeeEngine.Renderer;
using System;

namespace Example
{
	public sealed class BeeGenerator : Behaviour
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
				for (int y = 0; y < BeesY; y++)
				{
					Entity newBee = Instantiate(BeePrefab, ThisEntity);
					var transform = newBee.GetComponent<TransformComponent>();
					transform.Translation.X = x;
					transform.Translation.Y = y;
				}
			}
		}
	}

	public sealed class BeeGeneratorRenderer : Behaviour
	{
		public Texture2D BeeTexture;
		public int BeesX = 0;
		public int BeesY = 0;
		private Matrix4[] beeTransforms;
		void OnCreate()
		{
			beeTransforms = new Matrix4[BeesX * BeesY];
			for (int x = 0; x < BeesX; x++)
			{
				for (int y = 0; y < BeesY; y++)
				{
					beeTransforms[x * BeesY + y] = Matrix4.CreateTranslation(x, y, 0);
				}
			}
		}

		void OnRender(Graphics graphics)
		{
			for (int x = 0; x < BeesX; x++)
			{
				for (int y = 0; y < BeesY; y++)
				{
					graphics.DrawSprite(ref beeTransforms[x * BeesY + y], Color.White, BeeTexture, 1, ThisEntity);
				}
			}
		}
	}
}
