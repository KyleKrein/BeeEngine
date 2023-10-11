using BeeEngine;
using System;

namespace Example
{
	public class WorkWithParents: Behaviour
	{
		void OnUpdate()
		{
			if (Input.IsKeyDown(Key.L))
			{
				Entity c = ThisEntity;
				string result = c.Name;
				while (c.Parent != null)
				{
					c = c.Parent;
					result += " -> " + c.Name;
				}
				Log.Info(result);
			}
		}
	}
}
