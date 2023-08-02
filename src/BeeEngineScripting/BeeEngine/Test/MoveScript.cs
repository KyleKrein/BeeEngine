using BeeEngine.Math;

namespace BeeEngine.Test
{
    public class MoveScript : Entity
    {
        public float Speed = 0.01f;
        public bool ControlFromKeyboard = false;
        public bool UseWASD = true;
        public Vector3 zhu = new Vector3(4, 2, 0.5f);
        private TransformComponent m_Transform;

        private void OnCreate()
        {
            Log.Debug("Has Transform Component = {0}", HasComponent<TransformComponent>());
            m_Transform = GetComponent<TransformComponent>();
        }

        private void OnUpdate()
        {
            if (!ControlFromKeyboard)
                return;
            if (UseWASD ? Input.IsKeyDown(Key.W) : Input.IsKeyDown(Key.Up))
            {
                m_Transform.Translation.Y += 1 * Speed;
            }
            if (UseWASD ? Input.IsKeyDown(Key.S) : Input.IsKeyDown(Key.Down))
            {
                m_Transform.Translation.Y -= 1 * Speed;
            }
            if (UseWASD ? Input.IsKeyDown(Key.A) : Input.IsKeyDown(Key.Left))
            {
                m_Transform.Translation.X -= 1 * Speed;
            }
            if (UseWASD ? Input.IsKeyDown(Key.D) : Input.IsKeyDown(Key.Right))
            {
                m_Transform.Translation.X += 1 * Speed;
            }
        }

        private void OnDestroy()
        {
        }
    }
}