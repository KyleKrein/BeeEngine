using BeeEngine;
using System;

namespace Example
{
    public sealed class RandomColor : Behaviour
    {
        private SpriteRendererComponent m_Sprite;
        private Random m_Random = new Random();
        private byte[] m_RandomColor = new byte[3];
        
        private float m_PrevTime;

        private void OnCreate()
        {
            Log.Info("OnCreate: {0}", nameof(RandomColor));
            m_Sprite = GetComponent<SpriteRendererComponent>();
            
            m_PrevTime = Time.TotalTime;
        }

        private void OnUpdate()
        {
            float currentTime = Time.TotalTime;
            if (currentTime - m_PrevTime < 1)
                return;
            m_PrevTime = currentTime;
            m_Random.NextBytes(m_RandomColor);
            m_Sprite.Color = new Color(m_RandomColor[0], m_RandomColor[1], m_RandomColor[2], 255);
        }

        private void OnDestroy()
        {
            Log.Info("OnDestroy: {0}", nameof(RandomColor));
        }
    }
}