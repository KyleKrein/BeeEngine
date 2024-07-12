using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using BeeEngine.Internal;
using BeeEngine.Math;

namespace BeeEngine.Renderer;
public struct Graphics
{
    public unsafe void DrawSprite(ref Matrix4 transform, Color color, Texture2D? texture = null, float tilingFactor = 1.0f, Entity? entity = null)
    {
        SpriteInstanceBufferData data = new SpriteInstanceBufferData { Model = transform, Color = color, TilingFactor = tilingFactor, EntityID = entity is not null ? (int)entity.ID : 0 };
        if (texture is null)
        {
            InternalCalls.Renderer_SubmitInstance(this, InternalCalls.ModelType.Rectangle, ref Unsafe.NullRef<AssetHandle>(), &data, (ulong)sizeof(SpriteInstanceBufferData));
            return;
        }
        InternalCalls.Renderer_SubmitInstance(this, InternalCalls.ModelType.Rectangle, ref texture.m_Handle, &data, (ulong)sizeof(SpriteInstanceBufferData));
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