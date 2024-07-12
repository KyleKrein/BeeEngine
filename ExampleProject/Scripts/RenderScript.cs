using BeeEngine;
using BeeEngine.Math;
using BeeEngine.Renderer;
using System;

namespace Example
{
	public class RenderScript : Behaviour
	{
		public Texture2D Image;
		void OnRender(Graphics graphics)
		{
			Matrix4 transform = Matrix4.Identity;
			graphics.DrawSprite(ref transform, Color.White, Image);
		}
	}
}
