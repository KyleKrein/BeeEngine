using System.Runtime.InteropServices;
using System.Text;

namespace BeeEngine.Internal
{
    [StructLayout(LayoutKind.Explicit)]
    internal struct AssetHandle
    {
        [FieldOffset(0)]
        private ulong RegisterID;

        [FieldOffset(8)]
        private ulong AssetID;

        public static bool operator ==(AssetHandle left, AssetHandle right)
        {
            return left.RegisterID == right.RegisterID && left.AssetID == right.AssetID;
        }

        public static bool operator !=(AssetHandle left, AssetHandle right)
        {
            return !(left == right);
        }
        public override string ToString()
        {
            StringBuilder stringBuilder = new StringBuilder();
            stringBuilder.Append(RegisterID);
            stringBuilder.Append(':');
            stringBuilder.Append(AssetID);
            return stringBuilder.ToString();
        }
    }
}