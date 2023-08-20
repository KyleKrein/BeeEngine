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

        private Texture2D[] m_Textures;
        private SpriteRendererComponent m_Sprite;
        private Random m_Random = new Random();

        private uint m_Counter = 0;

        private void OnCreate()
        {
            m_Textures = new Texture2D[]
            {
                Texture1, Texture2, Texture3, Texture4, Texture5
            };
            m_Sprite = GetComponent<SpriteRendererComponent>();
            Log.Info("RandomTexture OnCreate");
        }

        private void OnUpdate()
        {
            if (m_Counter++ < 60)
                return;
            m_Counter = 0;
            m_Sprite.Texture = m_Textures[m_Random.Next(0, 5)];
        }
    }
}