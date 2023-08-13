using BeeEngine;
using System;
using BeeEngine.Math;

namespace Zhu
{
    public sealed class Camera : Behaviour
    {
        private TransformComponent m_Transform;
        public float Speed = 0;

        public event EventHandler<TransformComponent> PositionChanged;

        private void OnCreate()
        {
            Log.Info("OnCreate: {0}", nameof(Camera));
            m_Transform = GetComponent<TransformComponent>();
        }

        private void OnUpdate()
        {
            bool changed = false;
            if (Input.IsKeyDown(Key.W))
            {
                m_Transform.Translation.Y += Speed;
                changed = true;
            }
            if (Input.IsKeyDown(Key.S))
            {
                m_Transform.Translation.Y -= Speed;
                changed = true;
            }
            if (Input.IsKeyDown(Key.D))
            {
                m_Transform.Translation.X += Speed;
                changed = true;
            }
            if (Input.IsKeyDown(Key.A))
            {
                m_Transform.Translation.X -= Speed;
                changed = true;
            }

            if (changed)
            {
                PositionChanged?.Invoke(this, m_Transform);
            }
        }

        private void OnDestroy()
        {
            Log.Info("OnDestroy: {0}", nameof(Camera));
        }
    }
}