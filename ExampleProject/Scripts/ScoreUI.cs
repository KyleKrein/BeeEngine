using BeeEngine;
using BeeEngine.UI;
using System;

namespace Example
{
		public class ScoreUI : Behaviour
		{
				private Document m_ScoreUI;
				void OnCreate()
				{
						m_ScoreUI = new Document("hello_world");
						m_ScoreUI.Show();
				}
				void OnDestroy()
				{
						m_ScoreUI.Close();
				}
		}
}
