using BeeEngine;
using System;

namespace Example
{
	public class Bee : Behaviour
	{
		void OnCreate()
		{
			Entity.OnMouseClick += (sender, args) =>
			{
				if (args.Entity == ThisEntity && args.MouseButton == MouseButton.Left)
				{
					ThisEntity.Destroy();
				}
			};
			Entity.OnMouseEnter += (sender, entity) =>
			{
				if (entity == ThisEntity)
				{
					GetComponent<SpriteRendererComponent>().Color = Color.Green;
				}
			};
			Entity.OnMouseLeave += (sender, entity) =>
			{
				if (entity == ThisEntity)
				{
					GetComponent<SpriteRendererComponent>().Color = Color.White;
				}
			};
		}
	}
}
