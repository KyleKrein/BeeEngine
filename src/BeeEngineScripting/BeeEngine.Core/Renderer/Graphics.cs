using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using BeeEngine.Internal;
using BeeEngine.Math;

namespace BeeEngine.Renderer;
public struct Graphics
{
    public unsafe void DrawSprite(ref Matrix4 transform, Color color, Texture2D? texture = null, float tilingFactor = 1.0f, Entity? entity = null)
    {
        SpriteInstanceBufferData data = new SpriteInstanceBufferData { Model = transform, Color = color, TilingFactor = tilingFactor, EntityID = entity is null ? 0 : (int)LifeTimeManager.GetEntityEnttID(entity) + 1 };
        if (texture is null)
        {
            InternalCalls.Renderer_SubmitInstance(this, InternalCalls.ModelType.Rectangle, ref Unsafe.NullRef<AssetHandle>(), &data, (ulong)sizeof(SpriteInstanceBufferData));
            return;
        }
        InternalCalls.Renderer_SubmitInstance(this, InternalCalls.ModelType.Rectangle, ref texture.m_Handle, &data, (ulong)sizeof(SpriteInstanceBufferData));
    }
    public unsafe void DrawCircle(ref Matrix4 transform, Color color, float thickness = 1.0f, float fade = 0.005f, Entity? entity = null)
    {
        CircleInstanceBufferData data = new CircleInstanceBufferData { Model = transform, Color = color, Thickness = thickness, Fade = fade, EntityID = entity is null ? 0 : (int)LifeTimeManager.GetEntityEnttID(entity) + 1 };
        InternalCalls.Renderer_SubmitInstance(this, InternalCalls.ModelType.Circle, ref Unsafe.NullRef<AssetHandle>(), &data, (ulong)sizeof(CircleInstanceBufferData));
    }
    public void DrawText(string text, ref Matrix4 transform, Font font, Color color, float kerningOffset = 0.0f, float spacing = 0.0f, Entity? entity = null)
    {
        Log.AssertAndThrow(font is not null, "Font cannot be null");
        TextConfig config = new TextConfig { ForegroundColor = color, KerningOffset = kerningOffset, LineSpacing = spacing };
        InternalCalls.Renderer_SubmitText(this, ref font.m_Handle, text, ref transform, ref config, entity is null ? -1 : (int)LifeTimeManager.GetEntityEnttID(entity));
    }
    [StructLayout(LayoutKind.Sequential)]
    private struct SpriteInstanceBufferData
    {
        public Matrix4 Model = Matrix4.Identity;
        public Color Color = Color.White;
        public float TilingFactor = 1.0f;
        public int EntityID = -1;

        public SpriteInstanceBufferData()
        {
        }
    }
    [StructLayout(LayoutKind.Sequential)]
    private struct CircleInstanceBufferData
    {
        public Matrix4 Model = Matrix4.Identity;
        public Color Color = Color.White;
        public float Thickness = 1.0f;
        public float Fade = 0.005f;
        public int EntityID = -1;

        public CircleInstanceBufferData()
        {
        }
    }

    internal IntPtr CommandBufferHandle;
    internal IntPtr RenderingQueueHandle;
}