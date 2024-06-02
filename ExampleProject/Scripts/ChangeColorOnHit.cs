using BeeEngine;
using System;

namespace Example
{
	public class ChangeColorOnHit: Behaviour
	{
		void OnCreate()
		{
			ThisEntity.OnCollisionStart += (object? sender, Entity other) =>
			{
				if(sender is null)
				{
					Log.Error("Sender is null!");
					return;
				}
				Entity self = (Entity)sender;
				Span<byte> buffer = stackalloc byte[3];
				Random.Shared.NextBytes(buffer);
				self.GetComponent<CircleRendererComponent>()!.Color = new Color(buffer[0], buffer[1], buffer[2], 255);
				Log.Info("Color changed!");
			};
		}
	}
}
