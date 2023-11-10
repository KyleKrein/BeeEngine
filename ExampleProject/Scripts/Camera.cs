using BeeEngine;
using System;
using BeeEngine.Math;

namespace Example
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
                m_Transform.Translation.Y += Speed * Time.DeltaTime;
                changed = true;
            }
            if (Input.IsKeyDown(Key.S))
            {
                m_Transform.Translation.Y -= Speed * Time.DeltaTime;
                changed = true;
            }
            if (Input.IsKeyDown(Key.D))
            {
                m_Transform.Translation.X += Speed * Time.DeltaTime;
                changed = true;
            }
            if (Input.IsKeyDown(Key.A))
            {
                m_Transform.Translation.X -= Speed * Time.DeltaTime;
                changed = true;
            }

            if(Input.IsMouseButtonDown(MouseButton.Left))
            {
                var mouseCoords = Input.GetMouseCoordsInWorldSpace(ThisEntity);
                var start = new Vector2(m_Transform.Translation.X, m_Transform.Translation.Y);
                var direction = Vector2.Normalize(mouseCoords);
                const float distance = 100.0f;
                var end = mouseCoords;//start + direction * distance;
                Entity e = Physics2D.CastRay(start, end);
                //Entity e = Physics2D.CastRay(new Vector2(mouseCoords.X + 0.0001f, mouseCoords.Y), mouseCoords);
                if (e != null && e.IsAlive())
                {
                    e.Destroy();
                }
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