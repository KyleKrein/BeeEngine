using BeeEngine.Internal;

namespace BeeEngine;
public sealed class Font : Asset
{
		public static Font? Get(string name)
		{
				Log.AssertAndThrow(name is not null, "{} could not be null", nameof(name));
				bool result = InternalCalls.Asset_GetByName(name, InternalCalls.AssetType.Font, out AssetHandle handle);
				if (!result)
				{
						return null;
				}
				return new Font() { m_Handle = handle };
		}
}
