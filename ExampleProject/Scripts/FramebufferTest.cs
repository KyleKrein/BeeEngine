using System.Reflection.Metadata;
using BeeEngine;
using BeeEngine.Math;
using BeeEngine.Renderer;

namespace Example
{
	public class FramebufferTest : Behaviour
	{
		public Texture2D Texture1;
		public Texture2D Texture2;
		public Font TextFont;
		public Prefab ChildPrefab;
		private string m_Text = string.Empty;
		private Framebuffer m_Framebuffer;
		private SceneCameraBuffer m_SceneCameraBuffer;
		void OnCreate()
		{
			ThisEntity.Name = "FramebufferTest Parent";
			m_Framebuffer = Framebuffer.Create(300, 300, Color.Cyan);
			m_SceneCameraBuffer = SceneCameraBuffer.CreateOrthographic(4, 4);
			Entity.OnMouseEnter += (sender, entity) =>
			{
				m_Text = entity.Name;
			};
			Entity.OnMouseLeave += (sender, entity) =>
			{
				m_Text = string.Empty;
			};
			var child = Instantiate(ChildPrefab);
			ThisEntity.AddChild(child);
			child.Name = string.Format("FramebufferTest Child [{0}]", child.Name);

			var graphics = m_Framebuffer.Bind();
			Transform transform = new Transform { };
			transform.Translation.Y -= 1;
			graphics.DrawSprite(m_SceneCameraBuffer, ref transform, Color.White, Texture1, 1, ThisEntity);
			transform.Translation.Y += 2;
			graphics.DrawSprite(m_SceneCameraBuffer, ref transform, Color.Red, Texture2, 1, child);
			m_Framebuffer.Unbind();
		}

		void OnRender(Graphics graphics)
		{
			var transform = new Transform { Scale = new Vector3(3, 3, 1) };
			graphics.DrawSprite(ref transform, m_Framebuffer);
			if (m_Text != string.Empty)
			{
				transform.Translation.Y -= 1;
				transform.Scale /= 2;
				graphics.DrawText(m_Text, ref transform, TextFont, Color.GreenYellow);
			}
		}
	}
}
