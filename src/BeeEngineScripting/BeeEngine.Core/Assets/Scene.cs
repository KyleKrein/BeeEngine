using BeeEngine.Internal;
namespace BeeEngine;
public sealed class Scene : Asset
{
		public static Scene? Get(string name)
		{
				Log.AssertAndThrow(name is not null, "{} could not be null", nameof(name));
				bool result = InternalCalls.Asset_GetByName(name, InternalCalls.AssetType.Scene, out AssetHandle handle);
				if (!result)
				{
						return null;
				}
				return new Scene() { m_Handle = handle };
		}
		public static Scene ActiveScene
		{
				get
				{
						return new Scene { m_Handle = InternalCalls.Scene_GetActive() };
				}
		}
		public void SetActive()
		{
				Log.AssertAndThrow(IsActive() == false, "Scene is already active");
				InternalCalls.Scene_SetActive(ref m_Handle);
		}
		public bool IsActive()
		{
				return InternalCalls.Scene_GetActive() == m_Handle;
		}
		public void Reset()
		{
				InternalCalls.Scene_SetActive(ref m_Handle);
		}
}
