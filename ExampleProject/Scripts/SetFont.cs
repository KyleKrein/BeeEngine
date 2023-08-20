using BeeEngine;
using System;

namespace Example
{
    public class SetFont : Behaviour
    {
        public Font FirstFont = null;
        public Font SecondFont = null;
        public Font ThirdFont = null;
        private short m_Counter = 1;
        private TextRendererComponent m_Text;

        private void OnCreate()
        {
            m_Text = GetComponent<TextRendererComponent>();
        }

        private void OnUpdate()
        {
            if (Input.IsKeyDown(Key.F))
            {
                switch (m_Counter)
                {
                    case 0:
                        m_Text.Font = FirstFont;
                        m_Counter++; break;
                    case 1:
                        m_Text.Font = SecondFont;
                        m_Counter++; break;
                    case 2:
                        m_Text.Font = ThirdFont;
                        m_Counter++; break;
                    default:
                        m_Counter = 0;
                        break;
                }
            }
        }
    }
}