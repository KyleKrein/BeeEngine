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
			{
				if(!FirstScene.IsActive())
					FirstScene.SetActive();
				else
					FirstScene.Reset();
			}
			if(Input.IsKeyDown(Key.D2))
			{
				if(!SecondScene.IsActive())
					SecondScene.SetActive();
				else
					SecondScene.Reset();
			}
			if(Input.IsKeyDown(Key.D3))
			{
				if(!ThirdScene.IsActive())
					ThirdScene.SetActive();
				else
					ThirdScene.Reset();
			}
			if(Input.IsKeyDown(Key.D4))
			{
				if(!FourthScene.IsActive())
					FourthScene.SetActive();
				else
					FourthScene.Reset();
			}
			if(Input.IsKeyDown(Key.D5))
			{
				if(!FifthScene.IsActive())
					FifthScene.SetActive();
				else
					FifthScene.Reset();
			}
		}
	}
}
