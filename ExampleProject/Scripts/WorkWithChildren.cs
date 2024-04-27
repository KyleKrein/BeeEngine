using BeeEngine;
using System;
using System.Text;

namespace Example
{
	public class WorkWithChildren: Behaviour
	{
		void OnUpdate()
		{
			if(Input.IsKeyDown(Key.G))
				Log.Info(PrintChildren(ThisEntity));
		}

		string PrintChildren(Entity parent)
		{
			StringBuilder result = new StringBuilder();
			result.Append('[');
			foreach (var child in parent.Children)
			{
				result.Append(PrintChildren(child));
				result.Append(' ');
				result.Append(child.Name);
				result.Append(' ');
			}
			result.Append(']');
			return result.ToString();
		}
	}
}
