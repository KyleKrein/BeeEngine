using BeeEngine;
using System;

namespace Example
{
    public sealed class RandomTexture : Behaviour
    {
        public Texture2D Texture1;
        public Texture2D Texture2;
        public Texture2D Texture3;
        public Texture2D Texture4;
        public Texture2D Texture5;

        public Texture2D Sprite;

        private Texture2D[] m_Textures;
        private SpriteRendererComponent m_Sprite;
        private Random m_Random = new Random();

        private float m_PrevTime;

        private void OnCreate()
        {
            m_Textures = new Texture2D[]
            {
                Texture1, Texture2, Texture3, Texture4, Texture5
            };
            m_Sprite = GetComponent<SpriteRendererComponent>();
            Sprite = m_Sprite.Texture;
            Log.Info("RandomTexture OnCreate");
            m_PrevTime = Time.TotalTime;
        }

        private void OnUpdate()
        {
            float currentTime = Time.TotalTime;
            if (currentTime - m_PrevTime < 1)
                return;
            m_PrevTime = currentTime;
            m_Sprite.Texture = m_Textures[m_Random.Next(0, 5)];
        }
    }
}