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
    }

    public class TransformComponent : Component
    {
        private unsafe Vector3* m_Translation;
        private unsafe Vector3* m_Rotation;
        private unsafe Vector3* m_Scale;

        internal unsafe TransformComponent()
        {
            m_Translation = (Vector3*)ComponentHandle;
            m_Rotation = m_Translation + 1;
            m_Scale = m_Rotation + 1;
        }

        public unsafe ref Vector3 Translation => ref Unsafe.AsRef<Vector3>(m_Translation);
        public unsafe ref Vector3 Rotation => ref Unsafe.AsRef<Vector3>(m_Rotation);
        public unsafe ref Vector3 Scale => ref Unsafe.AsRef<Vector3>(m_Scale);
    }
}