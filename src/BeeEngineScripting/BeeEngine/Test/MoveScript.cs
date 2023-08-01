using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BeeEngine.Test
{
    public class MoveScript : Entity
    {
        public float Speed = 0.01f;
        private TransformComponent m_Transform;

        private void OnCreate()
        {
            Log.Debug("Has Transform Component = {0}", HasComponent<TransformComponent>());
            m_Transform = GetComponent<TransformComponent>();
        }

        private void OnUpdate()
        {
            if (Input.IsKeyDown(Key.W))
            {
                m_Transform.Translation.Y += 1 * Speed;
            }
            if (Input.IsKeyDown(Key.S))
            {
                m_Transform.Translation.Y -= 1 * Speed;
            }
            if (Input.IsKeyDown(Key.A))
            {
                m_Transform.Translation.X -= 1 * Speed;
            }
            if (Input.IsKeyDown(Key.D))
            {
                m_Transform.Translation.X += 1 * Speed;
            }
        }

        private void OnDestroy()
        {
        }
    }
}