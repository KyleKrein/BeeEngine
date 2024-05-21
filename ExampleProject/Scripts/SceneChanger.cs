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
		void OnUpdate()
		{
			if(Input.IsKeyDown(Key.D1))
				FirstScene.SetActive();
			if(Input.IsKeyDown(Key.D2))
				SecondScene.SetActive();
		}
	}
}
