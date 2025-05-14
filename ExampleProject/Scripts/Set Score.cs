using BeeEngine;
using BeeEngine.UI;
using System;
using BeeEngine.Math;

namespace Example
{
		public class SetScore : Behaviour
		{
				private Document m_UI;
				public int Score = 0;

				private void OnCreate()
				{
						m_UI = new Document("game_score_overlay");
						m_UI.Show(); 
						SetTextScore();
				}

				void OnUpdate()
				{
						if (Input.IsKeyDown(Key.KeyPadSubtract))
						{
								--Score;
								SetTextScore();
						}
						if (Input.IsKeyDown(Key.KeyPadAdd))
						{
								++Score;
								SetTextScore();
						}
				}

				private void OnDestroy()
				{
						m_UI.Close();
				}
				public void IncreaseScore()
				{
						++Score;
						SetTextScore();
				}
				private void SetTextScore()
				{
						m_UI.SetText("score-text", Localization.Translate("test_scene.score", "score", Score));
				}
		}
}
