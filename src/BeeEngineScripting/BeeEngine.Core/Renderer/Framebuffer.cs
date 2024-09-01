using BeeEngine.Internal;

namespace BeeEngine.Renderer;

/// <summary>
/// Represents a framebuffer, a texture/image that can be drawn to.
/// Use <see cref="Framebuffer.Create(uint, uint)"/> to create a new framebuffer.
/// Call <see cref="Bind()"/> to prepare the framebuffer for draw commands.
/// use <see cref="Graphics"/> object returned from <see cref="Bind()"/> to draw to the framebuffer.
/// Call <see cref="Unbind()"/> when done.
/// </summary>
public sealed class Framebuffer : IDisposable
{
    uint Width => m_WidthPoints; // Returns width
    uint Height => m_HeightPoints; // Returns height
    uint WidthInPixels => m_WidthPixels; // Returns actual width in pixels
    uint HeightInPixels => m_HeightPixels; // Returns actual height in pixels
    uint m_WidthPixels;
    uint m_HeightPixels;
    uint m_WidthPoints;
    uint m_HeightPoints;
    private IntPtr m_Handle;
    private bool disposedValue;
    private bool m_Binded;
    private Graphics m_CurrentGraphics;

    internal IntPtr Handle => m_Handle;

    /// <summary>
    /// Creates a new Framebuffer with the specified width and height in points.
    /// </summary>
    /// <param name="width">The width of the Framebuffer in points.</param>
    /// <param name="height">The height of the Framebuffer in points.</param>
    /// <returns>A new instance of <see cref="Framebuffer"/>.</returns>
    public static Framebuffer Create(uint width, uint height)
    {
        return new Framebuffer(width, height, Color.Transparent);
    }
    /// <summary>
    /// Creates a new Framebuffer with the specified width and height in pixels.
    /// Prefer using <see cref="Create(uint, uint)"/>, unless you really know
    /// that you need set size in pixels.
    /// </summary>
    /// <param name="width">The width of the Framebuffer in pixels.</param>
    /// <param name="height">The height of the Framebuffer in pixels.</param>
    /// <returns>A new instance of <see cref="Framebuffer"/>.</returns>
    public static Framebuffer CreatePixels(uint width, uint height)
    {
        return new Framebuffer(width, height, width, height, Color.Transparent);
    }
    public static Framebuffer Create(uint width, uint height, Color backgroundColor)
    {
        return new Framebuffer(width, height, backgroundColor);
    }
    public static Framebuffer CreatePixels(uint width, uint height, Color backgroundColor)
    {
        return new Framebuffer(width, height, width, height, backgroundColor);
    }
    /// <summary>
    /// Initializes a new instance of the <see cref="Framebuffer"/> class with the specified width and height.
    /// </summary>
    /// <param name="width">The width of the framebuffer in points.</param>
    /// <param name="height">The height of the framebuffer in points.</param>
    Framebuffer(uint width, uint height, Color color) : this(width, height, PointsToPixels(width), PointsToPixels(height), color)
    { }
    private Framebuffer(uint widthPoints, uint heightPoints, uint widthPixels, uint heightPixels, Color color)
    {
        m_WidthPoints = widthPoints;
        m_HeightPoints = heightPoints;
        m_WidthPixels = widthPixels;
        m_HeightPixels = heightPixels;
        m_Handle = InternalCalls.Framebuffer_CreateDefault(m_WidthPixels, m_HeightPixels, color);
    }
    /// <summary>
    /// Prepares framebuffer for draw commands and creates Graphics object.
    /// Don't forget to call <see cref="Unbind"/> when done.
    /// </summary>
    /// <remarks>
    /// This method throws an exception if the Framebuffer is currently bound.
    /// </remarks>
    /// <returns>The graphics associated with the Framebuffer.</returns>
    public Graphics Bind()
    {
        ThrowIfDisposed();
        Log.AssertAndThrow(!m_Binded, "Trying to bind an already bound framebuffer. Forgot to call Unbind()?");
        m_CurrentGraphics = InternalCalls.Framebuffer_Bind(m_Handle);
        m_Binded = true;
        return m_CurrentGraphics;
    }

    /// <summary>
    /// Finishes draw commands for framebuffer, that were sent to Graphics object,
    /// returned from <see cref="Bind"/>.
    /// </summary>
    /// <remarks>
    /// This method throws an exception if the Framebuffer is not currently bound.
    /// </remarks>
    public void Unbind()
    {
        ThrowIfDisposed();
        Log.AssertAndThrow(m_Binded, "Trying to unbind an already unbound framebuffer. Forgot to call Bind()?");
        InternalCalls.Framebuffer_Unbind(m_Handle, m_CurrentGraphics);
        m_Binded = false;
    }

    /// <summary>
    /// Resizes the Framebuffer to the specified width and height in points. Framebuffer must
    /// not be bound.
    /// </summary>
    /// <param name="width">The new width of the Framebuffer in points.</param>
    /// <param name="height">The new height of the Framebuffer in points.</param>
    /// <remarks>
    /// This method throws an exception if the Framebuffer is currently bound.
    /// </remarks>
    public void Resize(uint width, uint height)
    {
        ThrowIfDisposed();
        Log.AssertAndThrow(!m_Binded, "Trying to resize a bound Framebuffer.");
        ResizeImpl(width, height, PointsToPixels(width), PointsToPixels(height));
    }

    /// <summary>
    /// Resizes the Framebuffer to the specified width and height in pixels. Framebuffer must
    /// not be bound. Prefer using <see cref="Resize(uint, uint)"/>, unless you really know
    /// that you need to set size in pixels.
    /// </summary>
    /// <param name="width">The new width of the Framebuffer in pixels.</param>
    /// <param name="height">The new height of the Framebuffer in pixels.</param>
    /// <remarks>
    /// This method throws an exception if the Framebuffer is currently bound.
    /// </remarks>
    public void ResizePixels(uint width, uint height)
    {
        ThrowIfDisposed();
        Log.AssertAndThrow(!m_Binded, "Trying to resize a bound Framebuffer.");
        ResizeImpl(width, height, width, height);
    }
    private void ResizeImpl(uint widthPoints, uint heightPoints, uint widthPixels, uint heightPixels)
    {
        InternalCalls.Framebuffer_Resize(m_Handle, widthPixels, heightPixels);
        m_WidthPoints = widthPoints;
        m_HeightPoints = heightPoints;
        m_WidthPixels = widthPixels;
        m_HeightPixels = heightPixels;
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

    private static uint PointsToPixels(uint points)
    {
        return (uint)(points * 1 /*ScaleFactor of Window*/);
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