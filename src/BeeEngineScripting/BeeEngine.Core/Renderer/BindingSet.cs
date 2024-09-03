using BeeEngine.Internal;

namespace BeeEngine.Renderer;
/// <summary>
/// The BindingSet struct is used to bind uniform buffers and textures
/// to shader programs.
/// </summary>
public sealed class BindingSet : IDisposable
{
    internal IntPtr m_Handle;
    private bool disposedValue;

    internal BindingSet(params IntPtr[] handles)
    {
        m_Handle = InternalCalls.BindingSet_Create(handles);
    }

    private void Dispose(bool disposing)
    {
        if (!disposedValue)
        {
            if (disposing)
            {
                // TODO: освободить управляемое состояние (управляемые объекты)
            }

            // TODO: освободить неуправляемые ресурсы (неуправляемые объекты) и переопределить метод завершения
            // TODO: установить значение NULL для больших полей
            InternalCalls.BindingSet_Destroy(m_Handle);
            disposedValue = true;
        }
    }

    ~BindingSet()
    {
        // Не изменяйте этот код. Разместите код очистки в методе "Dispose(bool disposing)".
        Dispose(disposing: false);
    }

    private void ThrowIfDisposed()
    {
        Log.AssertAndThrow(!disposedValue, "Trying to access a disposed binding set.");
    }

    public void Dispose()
    {
        Dispose(disposing: true);
        GC.SuppressFinalize(this);
    }
}
