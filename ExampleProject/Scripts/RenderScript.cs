using BeeEngine;
using BeeEngine.Math;
using BeeEngine.Renderer;

namespace Example
{
	public class RenderScript : Behaviour
	{
		public Texture2D Image;
		public Font Font;
		public Font Font2;
		private Color Color1 = Color.ForestGreen;
		private Color Color2 = Color.Red;
		void OnCreate()
		{
			Entity.OnMouseEnter += (sender, entity) =>
			{
				if (entity == ThisEntity)
				{
					SwapColors();
				}
			};
			Entity.OnMouseLeave += (sender, entity) =>
			{
				if (entity == ThisEntity)
				{
					SwapColors();
				}
			};
		}
		void OnRender(Graphics graphics)
		{
			Matrix4 transform = Matrix4.Identity;
			graphics.DrawText("Hello, World!", ref transform, Font, Color1, 0, 0, ThisEntity);
			transform = transform * Matrix4.CreateTranslation(6, 1, 0);
			graphics.DrawText("This is an example!", ref transform, Font2, Color2, 0, 0, ThisEntity);
			transform = Matrix4.CreateTranslation(0, -2, 0);
			graphics.DrawSprite(ref transform, Color.White, Image, 1, null);
		}

		void SwapColors()
		{
			var temp = Color1;
			Color1 = Color2;
			Color2 = temp;
		}
	}
}
