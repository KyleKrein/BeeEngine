using BeeEngine;
using System;

namespace Example
{
	public class SceneChanger: Behaviour
	{
		public Scene FirstScene;
		public Scene SecondScene;
		public Scene ThirdScene;
		public Scene FourthScene;

		public Scene FifthScene;

		private Scene[] m_Scenes;

		void OnCreate()
		{
			m_Scenes = new Scene[]
			{
				FirstScene, SecondScene, ThirdScene, FourthScene, FifthScene
			};
		}
		void OnUpdate()
		{
			//if(Input.IsKeyDown(Key.D1))
				//SceneManager.LoadScene(m_Scenes[0]);
		}
	}
}
