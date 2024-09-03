using BeeEngine.Internal;
using BeeEngine.Math;
using BeeEngine.Renderer;
using System.Runtime.CompilerServices;

namespace BeeEngine
{
    /// <summary>
    /// Abstract class for all components. Only for internal use,
    /// it doesn't meant to be used by Game Developers, because 
    /// each component class must have its twin class on the C++
    /// side of the engine.
    /// </summary>
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

    /// <summary>
    /// The TransformComponent class represents a Position, Rotation, and Scale
    /// of an Entity. Each Entity has its own TransformComponent by default.
    /// </summary>
    public sealed class TransformComponent : Component
    {
        private unsafe Vector3* m_Translation;
        private unsafe Vector3* m_Rotation;
        private unsafe Vector3* m_Scale;

        /// <summary>
        /// Returns a reference to the Transform object,
        /// which contains the Position, Rotation, and Scale of the Entity.
        /// </summary>
        public unsafe ref Transform Transform
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Transform>((Transform*)m_Translation);
            }
        }

        /// <summary>
        /// Returns a reference to the Position/Translation of the Entity.
        /// </summary>
        public unsafe ref Vector3 Translation
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Vector3>(m_Translation);
            }
        }
        /// <summary>
        /// Returns a reference to the Rotation of the Entity.
        /// </summary>
        public unsafe ref Vector3 Rotation
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Vector3>(m_Rotation);
            }
        }
        /// <summary>
        /// Returns a reference to the Scale of the Entity.
        /// </summary>
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

    /// <summary>
    /// The SpriteRendererComponent class allows to render 2D sprites at
    /// the position of an Entity.
    /// </summary>
    public sealed class SpriteRendererComponent : Component
    {
        private unsafe Color* m_Color;
        private unsafe AssetHandle* m_AssetHandle;
        private unsafe float* m_TilingFactor;
        private unsafe bool* m_HasTexture;

        private Texture2D m_Texture2D = new Texture2D();

        /// <summary>
        /// Returns a reference to the current Color, that is
        /// used to tint the sprite. Color.White
        /// does not tint the sprite.
        /// </summary>
        public unsafe ref Color Color
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Color>(m_Color);
            }
        }

        /// <summary>
        /// Returns a Texture2D of the sprite, if any.
        /// Otherwise, returns null.
        /// </summary>
        public unsafe Texture2D? Texture
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
                if (value is null)
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
        /// <summary>
        /// Returns a reference to the tiling factor of the sprite.
        /// TilingFactor allows to expand or shrink the sprite.
        /// If the sprite is extended, it will be repeated.
        /// Default value is 1.
        /// </summary>
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

    /// <summary>
    /// The CircleRendererComponent class allows to render circles at
    /// the position of an Entity.
    /// </summary>
    public sealed class CircleRendererComponent : Component
    {
        private unsafe Color* m_Color;
        private unsafe float* m_Thickness;
        private unsafe float* m_Fade;

        /// <summary>
        /// Returns a reference to the current Color, that is
        /// used to tint the circle.
        /// </summary>
        public unsafe ref Color Color
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Color>(m_Color);
            }
        }

        /// <summary>
        /// Returns the thickness of the circle.
        /// </summary>
        public unsafe ref float Thickness
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_Thickness);
            }
        }

        /// <summary>
        /// Returns the fade of the circle.
        /// The fade is the amount of fade to apply to the circle's edges.
        /// </summary>
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

    /// <summary>
    /// The RigidBodyType enum defines the type of the RigidBody
    /// of an Entity.
    /// </summary>
    public enum RigidBodyType
    {
        Static = 0, // Body is not affected by forces
        Dynamic = 1, // Body is affected by forces
        Kinematic = 2, // Body is affected by forces, but can not move. Not in use
    }

    /// <summary>
    /// The RigidBody2DComponent class allows to add a RigidBody to an Entity.
    /// RigidBodies are affected by forces and can move.
    /// </summary>
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

    /// <summary>
    /// The BoxCollider2DComponent class represents a 2D box or circle collider
    /// </summary>
    public sealed class BoxCollider2DComponent : Component
    {
        private unsafe BoxCollider2DType* m_Type;
        private unsafe Vector2* m_Offset;
        private unsafe Vector2* m_Size;

        private unsafe float* m_Density;
        private unsafe float* m_Friction;
        private unsafe float* m_Restitution;
        private unsafe float* m_RestitutionThreshold;

        /// <summary>
        /// Returns a reference to the type of the 2D collider (Box or Circle).
        /// </summary>
        public unsafe ref BoxCollider2DType Type
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<BoxCollider2DType>(m_Type);
            }
        }

        /// <summary>
        /// Returns a reference to the offset of the collider relative to the entity's center.
        /// </summary>
        public unsafe ref Vector2 Offset
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Vector2>(m_Offset);
            }
        }

        /// <summary>
        /// Returns a reference to the size of the 2D collider.
        /// </summary>
        public unsafe ref Vector2 Size
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Vector2>(m_Size);
            }
        }

        /// <summary>
        /// Returns a reference to the density of the collider. 
        /// Density affects the mass of the entity in physics calculations.
        /// </summary>
        public unsafe ref float Density
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_Density);
            }
        }

        /// <summary>
        /// Returns a reference to the friction of the collider. 
        /// Friction affects how much the entity resists sliding.
        /// </summary>
        public unsafe ref float Friction
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_Friction);
            }
        }

        /// <summary>
        /// Returns a reference to the restitution of the collider.
        /// Restitution determines the bounciness of the entity.
        /// </summary>
        public unsafe ref float Restitution
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_Restitution);
            }
        }

        /// <summary>
        /// Returns a reference to the restitution threshold of the collider.
        /// The restitution threshold determines the minimum velocity 
        /// needed for the entity to bounce.
        /// </summary>
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

    /// <summary>
    /// This class allows to render text using the Transform of an Entity.
    /// </summary>
    public sealed class TextRendererComponent : Component
    {
        private unsafe Color* m_ForegroundColor;
        private unsafe Color* m_BackgroundColor;
        private unsafe float* m_Kerning;
        private unsafe float* m_LineSpacing;
        private unsafe AssetHandle* m_AssetHandle;

        private Font m_Font = new Font();

        /// <summary>
        /// Gets or sets the text associated with this TextRendererComponent.
        /// </summary>
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
                Log.AssertAndThrow(value is not null, "Text in TextRendererComponent can't be null");
                InternalCalls.TextRendererComponent_SetText(EntityID, value);
            }
        }

        /// <summary>
        /// Gets or sets the font used by the TextRendererComponent.
        /// The font cannot be null, and must be a valid asset.
        /// </summary>
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
                Log.AssertAndThrow(value is not null, "Font in TextRendererComponent can't be null");
                Log.AssertAndThrow(value!.IsValid(), "Font asset is invalid");
                *m_AssetHandle = value.m_Handle;
            }
        }

        /// <summary>
        /// Returns a reference to the foreground color used for rendering the text.
        /// </summary>
        public unsafe ref Color Foreground
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Color>(m_ForegroundColor);
            }
        }

        /// <summary>
        /// Returns a reference to the background color used for rendering the text.
        /// </summary>
        public unsafe ref Color Background
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<Color>(m_BackgroundColor);
            }
        }

        /// <summary>
        /// Returns a reference to the kerning value of the text.
        /// Kerning adjusts the spacing between characters.
        /// </summary>
        public unsafe ref float Kerning
        {
            get
            {
                CheckIfDestroyed();
                return ref Unsafe.AsRef<float>(m_Kerning);
            }
        }

        /// <summary>
        /// Returns a reference to the line spacing value of the text.
        /// Line spacing adjusts the space between lines of text.
        /// </summary>
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