using BeeEngine;
using BeeEngine.Platform.Metal;
using BeeEngine.Platform.OpenGL;
using BeeEngine.SmartPointers;
using NotSupportedException = System.NotSupportedException;

namespace BeeEngine;

public abstract class VertexBuffer: IDisposable
{
    private SharedPointer<RendererID> _rendererId = new SharedPointer<RendererID>();

    public unsafe SharedPointer<RendererID> RendererID
    {
        get => _rendererId.Share();
        protected set
        {
            *_rendererId.GetPtr() = *value.GetPtr();
            value.Release();
        }
    }
    public int Count { get; protected init; }
    private BufferLayout _layout;
    public BufferLayout Layout
    {
        get => _layout;
        set
        {
            if (!value.IsReadOnly)
            {
                value.Build();
            }

            _layout = value;
        }
    }
    public static VertexBuffer Create(int size)
    {
        switch (Renderer.API)
        {
            case API.OpenGL:
                return new OpenGLVertexBuffer(size);
            case API.Metal:
                #if IOS
                return new MetalVertexBuffer(size);
                #endif
            case API.None:
                Log.Error("{0} is not supported", Renderer.API);
                throw new NotSupportedException();
            default:
                Log.Error("Unknown Renderer API is not supported");
                throw new NotSupportedException();
        }
    }
    public static VertexBuffer Create(float[] vertices)
    {
        switch (Renderer.API)
        {
            case API.OpenGL:
                return new OpenGLVertexBuffer(vertices);
            case API.Metal:
                #if IOS
                return new MetalVertexBuffer(vertices);
                #endif
            case API.None:
                Log.Error("{0} is not supported", Renderer.API);
                throw new NotSupportedException();
            default:
                Log.Error("Unknown Renderer API is not supported");
                throw new NotSupportedException();
        }
    }

    public abstract void Bind();
    public abstract void Unbind();

    protected abstract void Dispose(bool disposing);

    public void Dispose()
    {
        Dispose(true);
        _rendererId.Dispose();
        GC.SuppressFinalize(this);
    }

    ~VertexBuffer()
    {
        Dispose(false);
        _rendererId.Dispose();
    }

    public abstract void SetData(IntPtr data, int size);
}

public enum DrawingFrequency
{
    
}