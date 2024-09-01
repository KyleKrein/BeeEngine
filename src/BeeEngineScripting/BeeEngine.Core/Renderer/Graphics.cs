using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using BeeEngine.Internal;
using BeeEngine.Math;

namespace BeeEngine.Renderer
{
    /// <summary>
    /// The Graphics struct provides methods to render 2D sprites, circles, and text.
    /// </summary>
    public struct Graphics
    {
        /*public enum Transparency
        {
            Transparent,
            Opaque
        }*/
        /// <summary>
        /// Draws a sprite using a transformation, color, optional texture, tiling factor, and an optional associated entity.
        /// </summary>
        /// <param name="transform">The transformation for positioning, scaling, and rotating the sprite.</param>
        /// <param name="color">The color to apply to the sprite.</param>
        /// <param name="texture">The optional texture to apply to the sprite. Default is null.</param>
        /// <param name="tilingFactor">The tiling factor for the texture. Default is 1.0f.</param>
        /// <param name="entity">The optional entity associated with the sprite for identification. Default is null.</param>
        public void DrawSprite(ref Transform transform, Color color, Texture2D? texture = null, float tilingFactor = 1.0f, Entity? entity = null)
        {
            var matrix = transform.Matrix;
            DrawSprite(ref matrix, color, texture, tilingFactor, entity);
        }

        public void DrawSprite(ref Matrix4 transform, Framebuffer framebuffer/*, Transparency transparency = Transparency.Transparent*/)
        {
            DrawSprite(null, ref transform, framebuffer/*, transparency*/);
        }

        public unsafe void DrawSprite(SceneCameraBuffer? camera, ref Matrix4 transform, Framebuffer framebuffer/*, Transparency transparency = Transparency.Transparent*/)
        {
            var data = new FramebufferInstanceData { Model = transform, FramebufferHandle = framebuffer.Handle };
            InternalCalls.Renderer_SubmitInstance(camera, this, InternalCalls.ModelType.Framebuffer, ref Unsafe.NullRef<AssetHandle>(), &data, (ulong)sizeof(Matrix4));
        }

        /// <summary>
        /// Draws a sprite using a transformation matrix, color, optional texture, tiling factor, and an optional associated entity.
        /// </summary>
        /// <param name="transform">The transformation matrix for positioning, scaling, and rotating the sprite.</param>
        /// <param name="color">The color to apply to the sprite.</param>
        /// <param name="texture">The optional texture to apply to the sprite.</param>
        /// <param name="tilingFactor">The tiling factor for the texture. Default is 1.0f.</param>
        /// <param name="entity">The optional entity associated with the sprite for identification.</param>
        public void DrawSprite(ref Matrix4 transform, Color color, Texture2D? texture = null, float tilingFactor = 1.0f, Entity? entity = null)
        {
            DrawSprite(null, ref transform, color, texture, tilingFactor, entity);
        }
        public unsafe void DrawSprite(SceneCameraBuffer? camera, ref Matrix4 transform, Color color, Texture2D? texture = null, float tilingFactor = 1.0f, Entity? entity = null)
        {
            SpriteInstanceBufferData data = new SpriteInstanceBufferData { Model = transform, Color = color, TilingFactor = tilingFactor, EntityID = entity is null ? 0 : (int)LifeTimeManager.GetEntityEnttID(entity) + 1 };
            if (texture is null)
            {
                InternalCalls.Renderer_SubmitInstance(camera, this, InternalCalls.ModelType.Rectangle, ref Unsafe.NullRef<AssetHandle>(), &data, (ulong)sizeof(SpriteInstanceBufferData));
                return;
            }
            InternalCalls.Renderer_SubmitInstance(camera, this, InternalCalls.ModelType.Rectangle, ref texture.m_Handle, &data, (ulong)sizeof(SpriteInstanceBufferData));
        }

        /// <summary>
        /// Draws a sprite with specified position, scale, rotation, color, optional texture, and optional entity.
        /// </summary>
        /// <param name="position">The position of the sprite.</param>
        /// <param name="scale">The scale of the sprite.</param>
        /// <param name="rotation">The rotation of the sprite in radians.</param>
        /// <param name="color">The color to apply to the sprite.</param>
        /// <param name="texture">The optional texture to apply to the sprite.</param>
        /// <param name="tilingFactor">The tiling factor for the texture. Default is 1.0f.</param>
        /// <param name="entity">The optional entity associated with the sprite for identification.</param>
        public void DrawSprite(Vector3 position, Vector3 scale, float rotation, Color color, Texture2D? texture = null, float tilingFactor = 1.0f, Entity? entity = null)
        {
            Matrix4 transform = Matrix4.CreateTransform(position, new Vector3(0.0f, 0.0f, rotation), scale);
            DrawSprite(ref transform, color, texture, tilingFactor, entity);
        }

        /// <summary>
        /// Draws a sprite with specified position, scale, color, optional texture, and optional entity.
        /// </summary>
        /// <param name="position">The position of the sprite.</param>
        /// <param name="scale">The scale of the sprite.</param>
        /// <param name="color">The color to apply to the sprite.</param>
        /// <param name="texture">The optional texture to apply to the sprite.</param>
        /// <param name="tilingFactor">The tiling factor for the texture. Default is 1.0f.</param>
        /// <param name="entity">The optional entity associated with the sprite for identification.</param>
        public void DrawSprite(Vector3 position, Vector3 scale, Color color, Texture2D? texture = null, float tilingFactor = 1.0f, Entity? entity = null)
        {
            Matrix4 transform = Matrix4.CreateTransform(position, Vector3.Zero, scale); // No rotation
            DrawSprite(ref transform, color, texture, tilingFactor, entity);
        }

        /// <summary>
        /// Draws a sprite with specified position, scale, and optional texture.
        /// The sprite is drawn with a default white color.
        /// </summary>
        /// <param name="position">The position of the sprite.</param>
        /// <param name="scale">The scale of the sprite.</param>
        /// <param name="texture">The optional texture to apply to the sprite.</param>
        /// <param name="tilingFactor">The tiling factor for the texture. Default is 1.0f.</param>
        public void DrawSprite(Vector3 position, Vector3 scale, Texture2D? texture = null, float tilingFactor = 1.0f)
        {
            DrawSprite(position, scale, Color.White, texture, tilingFactor, null);
        }

        /// <summary>
        /// Draws a sprite with specified position, scale, and optional texture.
        /// The position and scale are specified as Vector2. The sprite is drawn with a default white color.
        /// </summary>
        /// <param name="position">The position of the sprite as Vector2.</param>
        /// <param name="scale">The scale of the sprite as Vector2.</param>
        /// <param name="texture">The optional texture to apply to the sprite.</param>
        /// <param name="tilingFactor">The tiling factor for the texture. Default is 1.0f.</param>
        public void DrawSprite(Vector2 position, Vector2 scale, Texture2D? texture = null, float tilingFactor = 1.0f)
        {
            DrawSprite(new Vector3(position), new Vector3(scale), Color.White, texture, tilingFactor, null);
        }

        /// <summary>
        /// Draws a circle using a transformation matrix, color, thickness, fade, and an optional associated entity.
        /// </summary>
        /// <param name="transform">The transformation matrix for positioning, scaling, and rotating the circle.</param>
        /// <param name="color">The color to apply to the circle.</param>
        /// <param name="thickness">The thickness of the circle's outline. Default is 1.0f.</param>
        /// <param name="fade">The amount of fade to apply to the circle's edges. Default is 0.005f.</param>
        /// <param name="entity">The optional entity associated with the circle for identification. Default is null.</param>
        public void DrawCircle(ref Transform transform, Color color, float thickness = 1.0f, float fade = 0.005f, Entity? entity = null)
        {
            Matrix4 matrix = transform.Matrix;
            DrawCircle(ref matrix, color, thickness, fade, entity);
        }

        /// <summary>
        /// Draws a circle using a transformation matrix, color, thickness, fade, and an optional associated entity.
        /// </summary>
        /// <param name="transform">The transformation matrix for positioning, scaling, and rotating the circle.</param>
        /// <param name="color">The color to apply to the circle.</param>
        /// <param name="thickness">The thickness of the circle's outline. Default is 1.0f.</param>
        /// <param name="fade">The amount of fade to apply to the circle's edges. Default is 0.005f.</param>
        /// <param name="entity">The optional entity associated with the circle for identification.</param>
        public void DrawCircle(ref Matrix4 transform, Color color, float thickness = 1.0f, float fade = 0.005f, Entity? entity = null)
        {
            DrawCircle(null, ref transform, color, thickness, fade, entity);
        }

        public unsafe void DrawCircle(SceneCameraBuffer? camera, ref Matrix4 transform, Color color, float thickness = 1.0f, float fade = 0.005f, Entity? entity = null)
        {
            CircleInstanceBufferData data = new CircleInstanceBufferData { Model = transform, Color = color, Thickness = thickness, Fade = fade, EntityID = entity is null ? 0 : (int)LifeTimeManager.GetEntityEnttID(entity) + 1 };
            InternalCalls.Renderer_SubmitInstance(camera, this, InternalCalls.ModelType.Circle, ref Unsafe.NullRef<AssetHandle>(), &data, (ulong)sizeof(CircleInstanceBufferData));
        }

        /// <summary>
        /// Draws a circle with specified position, radius, color, thickness, fade, and optional entity.
        /// </summary>
        /// <param name="position">The position of the circle.</param>
        /// <param name="radius">The radius of the circle.</param>
        /// <param name="color">The color to apply to the circle.</param>
        /// <param name="thickness">The thickness of the circle's outline. Default is 1.0f.</param>
        /// <param name="fade">The amount of fade to apply to the circle's edges. Default is 0.005f.</param>
        /// <param name="entity">The optional entity associated with the circle for identification.</param>
        public void DrawCircle(Vector3 position, float radius, Color color, float thickness = 1.0f, float fade = 0.005f, Entity? entity = null)
        {
            Matrix4 transform = Matrix4.CreateTransform(position, Vector3.Zero, new Vector3(radius * 2)); // Uniform scaling based on radius
            DrawCircle(ref transform, color, thickness, fade, entity);
        }
        /// <summary>
        /// Draws text with specified text content, transformation matrix, font, color, kerning offset, line spacing, and optional entity.
        /// </summary>
        /// <param name="text">The text content to render.</param>
        /// <param name="transform">The transformation matrix for positioning and scaling the text.</param>
        /// <param name="font">The font to use for rendering the text.</param>
        /// <param name="color">The color to apply to the text.</param>
        /// <param name="kerningOffset">The kerning offset to adjust spacing between characters. Default is 0.0f.</param>
        /// <param name="spacing">The line spacing to adjust spacing between lines of text. Default is 0.0f.</param>
        /// <param name="entity">The optional entity associated with the text for identification. Default is null.</param>
        public void DrawText(string text, ref Transform transform, Font font, Color color, float kerningOffset = 0.0f, float spacing = 0.0f, Entity? entity = null)
        {
            var matrix = transform.Matrix;
            DrawText(text, ref matrix, font, color, kerningOffset, spacing, entity);
        }

        /// <summary>
        /// Draws text with specified text content, transformation matrix, font, color, kerning offset, line spacing, and optional entity.
        /// </summary>
        /// <param name="text">The text content to render.</param>
        /// <param name="transform">The transformation matrix for positioning and scaling the text.</param>
        /// <param name="font">The font to use for rendering the text.</param>
        /// <param name="color">The color to apply to the text.</param>
        /// <param name="kerningOffset">The kerning offset to adjust spacing between characters. Default is 0.0f.</param>
        /// <param name="spacing">The line spacing to adjust spacing between lines of text. Default is 0.0f.</param>
        /// <param name="entity">The optional entity associated with the text for identification.</param>
        public void DrawText(string text, ref Matrix4 transform, Font font, Color color, float kerningOffset = 0.0f, float spacing = 0.0f, Entity? entity = null)
        {
            DrawText(text, null, ref transform, font, color, kerningOffset, spacing, entity);
        }
        public void DrawText(string text, SceneCameraBuffer? camera, ref Matrix4 transform, Font font, Color color, float kerningOffset = 0.0f, float spacing = 0.0f, Entity? entity = null)
        {
            Log.AssertAndThrow(font is not null, "Font cannot be null");
            TextConfig config = new TextConfig { ForegroundColor = color, KerningOffset = kerningOffset, LineSpacing = spacing };
            InternalCalls.Renderer_SubmitText(camera, this, ref font.m_Handle, text, ref transform, ref config, entity is null ? -1 : (int)LifeTimeManager.GetEntityEnttID(entity));
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct SpriteInstanceBufferData
        {
            public Matrix4 Model;
            public Color Color;
            public float TilingFactor;
            public int EntityID;

            public SpriteInstanceBufferData()
            {
                Model = Matrix4.Identity;
                Color = Color.White;
                TilingFactor = 1.0f;
                EntityID = -1;
            }
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct FramebufferInstanceData
        {
            public Matrix4 Model;
            public IntPtr FramebufferHandle;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct CircleInstanceBufferData
        {
            public Matrix4 Model;
            public Color Color;
            public float Thickness;
            public float Fade;
            public int EntityID;

            public CircleInstanceBufferData()
            {
                Model = Matrix4.Identity;
                Color = Color.White;
                Thickness = 1.0f;
                Fade = 0.005f;
                EntityID = -1;
            }
        }

        internal IntPtr CommandBufferHandle;
        internal IntPtr RenderingQueueHandle;
    }
}
