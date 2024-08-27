using BeeEngine.Internal;

namespace BeeEngine.Renderer;

public sealed class Framebuffer : IDisposable
{
    uint Width => m_Width;
    uint Height => m_Height;
    uint m_Width;
    uint m_Height;
    private IntPtr m_Handle;
    private bool disposedValue;
    private bool m_Binded;
    private Graphics m_CurrentGraphics;
    /// <summary>
    /// Initializes a new instance of the <see cref="Framebuffer"/> class with the specified width and height.
    /// </summary>
    /// <param name="width">The width of the framebuffer.</param>
    /// <param name="height">The height of the framebuffer.</param>
    Framebuffer(uint width, uint height)
    {
        m_Width = width;
        m_Height = height;
        m_Handle = InternalCalls.Framebuffer_CreateDefault(width, height);
    }
    public Graphics Bind()
    {
        ThrowIfDisposed();
        Log.AssertAndThrow(!m_Binded, "Trying to bind an already bound framebuffer. Forgot to call Unbind()?");
        m_CurrentGraphics = InternalCalls.Framebuffer_Bind(m_Handle);
        m_Binded = true;
        return m_CurrentGraphics;
    }
    public void Unbind()
    {
        ThrowIfDisposed();
        Log.AssertAndThrow(m_Binded, "Trying to unbind an already unbound framebuffer. Forgot to call Bind()?");
        InternalCalls.Framebuffer_Unbind(m_Handle, m_CurrentGraphics);
        m_Binded = false;
    }
    /// <summary>
    /// Resizes the Framebuffer to the specified width and height. Framebuffer must
    /// not be bound.
    /// </summary>
    /// <param name="width">The new width of the Framebuffer.</param>
    /// <param name="height">The new height of the Framebuffer.</param>
    /// <remarks>
    /// This method throws an exception if the Framebuffer is currently bound.
    /// </remarks>
    public void Resize(uint width, uint height)
    {
        ThrowIfDisposed();
        Log.AssertAndThrow(!m_Binded, "Trying to resize a bound Framebuffer.");
        InternalCalls.Framebuffer_Resize(m_Handle, width, height);
        m_Width = width;
        m_Height = height;
    }
    private void ThrowIfDisposed()
    {
        Log.AssertAndThrow(!disposedValue, "Trying to access a disposed framebuffer.");
    }

    private void Dispose(bool disposing)
    {
        if (disposedValue)
        {
            return;
        }
        if (disposing)
        {
            // TODO: освободить управляемое состояние (управляемые объекты)
        }

        // TODO: освободить неуправляемые ресурсы (неуправляемые объекты) и переопределить метод завершения
        // TODO: установить значение NULL для больших полей
        InternalCalls.Framebuffer_Destroy(m_Handle);
        m_Handle = IntPtr.Zero;
        disposedValue = true;
    }

    ~Framebuffer()
    {
        Dispose(disposing: false);
    }

    void IDisposable.Dispose()
    {
        Dispose(disposing: true);
        GC.SuppressFinalize(this);
    }
}