using BeeEngine.Internal;
namespace BeeEngine
{
    public sealed class Scene: Asset
    {
        public static Scene ActiveScene 
        { 
            get
            {
                return new Scene{m_Handle = InternalCalls.Scene_GetActive()};
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
}