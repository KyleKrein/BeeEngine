using BeeEngine.Internal;

namespace BeeEngine
{
    /// <summary>
    /// Base class for all assets, that are provided by the engine.
    /// All assets are managed by the engine and can be loaded automatically
    /// the first time they are accessed or manually. The unload process is
    /// manual only. So the user has to call the Unload method to free the
    /// resources, that are used by the asset.
    /// </summary>
    public abstract class Asset
    {
        internal AssetHandle m_Handle = new AssetHandle();

        /// <summary>
        /// Load the asset manually. 
        /// If the asset is already loaded, this method does nothing.
        /// Can be used to load the asset before it is accessed the first time.
        /// </summary>
        public void Load()
        {
            InternalCalls.Asset_Load(ref m_Handle);
        }

        /// <summary>
        /// Unload the asset manually. 
        /// If the asset is already unloaded, this method does nothing.
        /// Can be used to free the resources, that are used by the asset.
        /// If the asset is accessed after it is unloaded, it will be loaded
        /// automatically.
        /// </summary>
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