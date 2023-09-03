using BeeEngine.Internal;
using BeeEngine.Math;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace BeeEngine
{
    public abstract class Component
    {
        internal unsafe void* ComponentHandle;
        internal ulong EntityID;

        internal bool Destroyed = false;

        internal abstract unsafe void Construct();

        internal void CheckIfDestroyed()
        {
            Log.AssertAndThrow(!Destroyed, "Trying to access data of a destroyed entity");
        }
    }

    public sealed class TransformComponent : Component
    {
        private unsafe Vector3* m_Translation;
        private unsafe Vector3* m_Rotation;
        private unsafe Vector3* m_Scale;

        public unsafe ref Vector3 Translation
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Vector3>(m_Translation);
            }
        }

        public unsafe ref Vector3 Rotation
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Vector3>(m_Rotation);
            }
        }

        public unsafe ref Vector3 Scale
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Vector3>(m_Scale);
            }
        }

        internal override unsafe void Construct()
        {
            m_Translation = (Vector3*)ComponentHandle;
            m_Rotation = m_Translation + 1;
            m_Scale = m_Rotation + 1;
        }
    }

    public sealed class SpriteRendererComponent : Component
    {
        private unsafe Color* m_Color;
        private unsafe AssetHandle* m_AssetHandle;
        private unsafe float* m_TilingFactor;
        private unsafe bool* m_HasTexture;

        private Texture2D m_Texture2D = new Texture2D();

        public unsafe ref Color Color
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Color>(m_Color);
            }
        }

        public unsafe Texture2D Texture
        {
            get
            {
                CheckIfDestroyed();
                if (*m_HasTexture == false) return null;
                m_Texture2D.m_Handle = *m_AssetHandle;
                return m_Texture2D;
            }
            set
            {
                CheckIfDestroyed();
                if (value == null)
                {
                    *m_HasTexture = false;
                    return;
                }
                Log.AssertAndThrow(value.IsValid(), "Texture2D is invalid");

                *m_AssetHandle = value.m_Handle;
                *m_HasTexture = true;
                m_Texture2D.m_Handle = value.m_Handle;

                DebugLog.Assert(*m_AssetHandle == value.m_Handle, "Asset handle was not copied");
                DebugLog.Assert(*m_HasTexture == true, "HasTexture was not copied");
                DebugLog.Assert(m_Texture2D.IsValid(), "Invalid asset handle");
            }
        }

        public unsafe ref float TilingFactor
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_TilingFactor);
            }
        }

        internal override unsafe void Construct()
        {
            m_Color = (Color*)(ComponentHandle);
            m_AssetHandle = (AssetHandle*)(m_Color + 1);
            m_TilingFactor = (float*)(m_AssetHandle + 1);
            //m_HasTexture = (bool*)((byte*)ComponentHandle + 36);
            m_HasTexture = (bool*)(m_TilingFactor + 1);
            m_Texture2D.m_Handle = *m_AssetHandle;
        }
    }

    public sealed class CircleRendererComponent : Component
    {
        private unsafe Color* m_Color;
        private unsafe float* m_Thickness;
        private unsafe float* m_Fade;

        public unsafe ref Color Color
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Color>(m_Color);
            }
        }

        public unsafe ref float Thickness
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_Thickness);
            }
        }

        public unsafe ref float Fade
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_Fade);
            }
        }

        internal override unsafe void Construct()
        {
            m_Color = (Color*)ComponentHandle;
            m_Thickness = (float*)(m_Color + 1);
            m_Fade = m_Thickness + 1;
        }
    }

    public enum RigidBodyType
    {
        Static = 0,
        Dynamic = 1,
        Kinematic = 2,
    }

    public sealed class RigidBody2DComponent : Component
    {
        private unsafe RigidBodyType* m_BodyType;
        private unsafe bool* m_FixedRotation;

        public unsafe ref RigidBodyType Type
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<RigidBodyType>(m_BodyType);
            }
        }

        public unsafe ref bool FixedRotation
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<bool>(m_FixedRotation);
            }
        }

        internal override unsafe void Construct()
        {
            m_BodyType = (RigidBodyType*)ComponentHandle;
            m_FixedRotation = (bool*)(m_BodyType + 1);
        }
    }

    public enum BoxCollider2DType
    {
        Box = 0,
        Circle = 1
    }

    public sealed class BoxCollider2DComponent : Component
    {
        private unsafe BoxCollider2DType* m_Type;
        private unsafe Vector2* m_Offset;
        private unsafe Vector2* m_Size;

        private unsafe float* m_Density;
        private unsafe float* m_Friction;
        private unsafe float* m_Restitution;
        private unsafe float* m_RestitutionThreshold;

        public unsafe ref BoxCollider2DType Type
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<BoxCollider2DType>(m_Type);
            }
        }

        public unsafe ref Vector2 Offset
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Vector2>(m_Offset);
            }
        }

        public unsafe ref Vector2 Size
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Vector2>(m_Size);
            }
        }

        public unsafe ref float Density
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_Density);
            }
        }

        public unsafe ref float Friction
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_Friction);
            }
        }

        public unsafe ref float Restitution
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_Restitution);
            }
        }

        public unsafe ref float RestitutionThreshold
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_RestitutionThreshold);
            }
        }

        internal override unsafe void Construct()
        {
            m_Type = (BoxCollider2DType*)ComponentHandle;
            m_Offset = (Vector2*)(m_Type + 1);
            m_Size = m_Offset + 1;
            m_Density = (float*)(m_Size + 1);
            m_Friction = m_Density + 1;
            m_Restitution = m_Friction + 1;
            m_RestitutionThreshold = m_Restitution + 1;
        }
    }

    public sealed class TextRendererComponent : Component
    {
        private unsafe Color* m_ForegroundColor;
        private unsafe Color* m_BackgroundColor;
        private unsafe float* m_Kerning;
        private unsafe float* m_LineSpacing;
        private unsafe AssetHandle* m_AssetHandle;

        private Font m_Font = new Font();

        public string Text
        {
            get
            {
                CheckIfDestroyed();
                return InternalCalls.TextRendererComponent_GetText(EntityID);
            }
            set
            {
                CheckIfDestroyed();
                InternalCalls.TextRendererComponent_SetText(EntityID, value);
            }
        }

        public unsafe Font Font
        {
            get
            {
                CheckIfDestroyed();
                m_Font.m_Handle = *m_AssetHandle;
                return m_Font;
            }
            set
            {
                CheckIfDestroyed();
                Log.AssertAndThrow(value != null, "Font in TextRendererComponent can't be null");
                Log.AssertAndThrow(value.IsValid(), "Font asset is invalid");
                *m_AssetHandle = value.m_Handle;
            }
        }

        public unsafe ref Color Foreground
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Color>(m_ForegroundColor);
            }
        }

        public unsafe ref Color Background
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Color>(m_BackgroundColor);
            }
        }

        public unsafe ref float Kerning
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_Kerning);
            }
        }

        public unsafe ref float LineSpacing
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_LineSpacing);
            }
        }

        internal override unsafe void Construct()
        {
            m_ForegroundColor = (Color*)ComponentHandle;
            m_BackgroundColor = m_ForegroundColor + 1;
            m_Kerning = (float*)(m_BackgroundColor + 1);
            m_LineSpacing = m_Kerning + 1;
            m_AssetHandle = (AssetHandle*)(m_LineSpacing + 1);
        }
    }
}