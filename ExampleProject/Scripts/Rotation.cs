using BeeEngine;
using System;

namespace Example
{
    public sealed class Rotation : Behaviour
    {
        private TransformComponent m_Transform;
        public float Speed = 0;

        private void OnCreate()
        {
            Log.Info("OnCreate: {0}", nameof(Rotation));
            m_Transform = GetComponent<TransformComponent>();
        }

        private void OnUpdate()
        {
            m_Transform.Rotation.Y += Speed * Time.DeltaTime;
            m_Transform.Rotation.X += Speed * Time.DeltaTime;
            m_Transform.Rotation.Z += Speed * Time.DeltaTime;
        }

        private void OnDestroy()
        {
            Log.Info("OnDestroy: {0}", nameof(Rotation));
        }
    }
}