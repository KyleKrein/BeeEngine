using BeeEngine.Internal;

namespace BeeEngine;
public sealed class Prefab : Asset
{
		public static Prefab? Get(string name)
		{
				Log.AssertAndThrow(name is not null, "{} could not be null", nameof(name));
				bool result = InternalCalls.Asset_GetByName(name, InternalCalls.AssetType.Prefab, out AssetHandle handle);
				if (!result)
				{
						return null;
				}
				return new Prefab() { m_Handle = handle };
		}
}
