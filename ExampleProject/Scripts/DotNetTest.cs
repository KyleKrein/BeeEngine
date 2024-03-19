using BeeEngine;
using System;

namespace Example
{
	public class DotNetTest: Behaviour
	{
		void OnCreate()
		{
			Log.Info(ThisEntity.Name + " was created");
		}
		void OnUpdate()
		{
			
		}
	}
}
