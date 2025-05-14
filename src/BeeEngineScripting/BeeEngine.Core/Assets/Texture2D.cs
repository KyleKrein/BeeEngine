using BeeEngine.Internal;
namespace BeeEngine;
public sealed class Texture2D : Asset
{
		public static Texture2D? Get(string name)
		{
				Log.AssertAndThrow(name is not null, "{} could not be null", nameof(name));
				bool result = InternalCalls.Asset_GetByName(name, InternalCalls.AssetType.Texture2D, out AssetHandle handle);
				if (!result)
				{
						return null;
				}
				return new Texture2D() { m_Handle = handle };
		}
}
