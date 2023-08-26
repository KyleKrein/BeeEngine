using BeeEngine.Internal;

namespace BeeEngine
{
    public abstract class Asset
    {
        internal AssetHandle m_Handle = new AssetHandle();

        public void Load()
        {
            InternalCalls.Asset_Load(ref m_Handle);
        }

        public void Unload()
        {
            InternalCalls.Asset_Unload(ref m_Handle);
        }

        public bool IsValid()
        {
            return InternalCalls.Asset_IsValid(ref m_Handle);
        }

        public bool IsLoaded()
        {
            return InternalCalls.Asset_IsLoaded(ref m_Handle);
        }
    }
}