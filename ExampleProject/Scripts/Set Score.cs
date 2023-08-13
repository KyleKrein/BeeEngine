using BeeEngine;
using System;
using BeeEngine.Math;

namespace Example
{
    public class SetScore : Behaviour
    {
        private TextRendererComponent m_Text;
        private int m_Score = 0;
        private TransformComponent m_Transform;
        private Camera m_Camera;
        public Vector3 Offset = Vector3.Zero;

        private void OnCreate()
        {
            m_Text = GetComponent<TextRendererComponent>();
            m_Transform = GetComponent<TransformComponent>();
            Entity cameraEntity = FindEntityByName("Camera");
            m_Camera = cameraEntity.GetBehaviour<Camera>();
            m_Camera.PositionChanged += Camera_PositionChanged;
            Camera_PositionChanged(m_Camera, cameraEntity.GetComponent<TransformComponent>());
        }

        private void OnDestroy()
        {
            m_Camera.PositionChanged -= Camera_PositionChanged;
        }

        private void Camera_PositionChanged(object sender, TransformComponent e)
        {
            m_Transform.Translation = e.Translation + Offset;
        }

        public void IncreaseScore()
        {
            m_Text.Text = $"Score: {++m_Score}";
        }
    }
}