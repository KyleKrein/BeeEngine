using System.Runtime.InteropServices;

namespace BeeEngine.Internal
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct AssetHandle
    {
        private ulong RegisterID;
        private ulong AssetID;

        public static bool operator ==(AssetHandle left, AssetHandle right)
        {
            return left.RegisterID == right.RegisterID && left.AssetID == right.AssetID;
        }

        public static bool operator !=(AssetHandle left, AssetHandle right)
        {
            return !(left == right);
        }
    }
}