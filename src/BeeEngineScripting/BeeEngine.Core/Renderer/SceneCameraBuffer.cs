using BeeEngine.Math;

namespace BeeEngine.Renderer;

public sealed class SceneCameraBuffer : IDisposable
{
    internal readonly UniformBuffer m_UniformBuffer;
    internal readonly BindingSet m_BindingSet;
    public Matrix4 ViewProjection
    {
        get
        {
            return m_ViewProjection;
        }
        set
        {
            m_ViewProjection = value;
            m_UniformBuffer.SetData(ref m_ViewProjection);
        }
    }
    private Matrix4 m_ViewProjection;
    private bool disposedValue;

    public static SceneCameraBuffer CreateDefault()
    {
        return new SceneCameraBuffer(Matrix4.Identity);
    }
    public static SceneCameraBuffer CreateOrthographic(uint width, uint height)
    {
        return new SceneCameraBuffer(Matrix4.CreateOrthographic(width, height, -1.0f, 1000.0f));
    }
    private SceneCameraBuffer(Matrix4 matrix)
    {
        m_ViewProjection = matrix;
        m_UniformBuffer = UniformBuffer.Create<Matrix4>();
        m_UniformBuffer.SetData(ref m_ViewProjection);
        BindingSetBuilder bindingSetBuilder = new();
        bindingSetBuilder.AddBinding(m_UniformBuffer);
        m_BindingSet = bindingSetBuilder.Build();
    }

    private void Dispose(bool disposing)
    {
        if (!disposedValue)
        {
            if (disposing)
            {
                m_UniformBuffer.Dispose();
                m_BindingSet.Dispose();
            }

            // TODO: освободить неуправляемые ресурсы (неуправляемые объекты) и переопределить метод завершения
            // TODO: установить значение NULL для больших полей
            disposedValue = true;
        }
    }

    // // TODO: переопределить метод завершения, только если "Dispose(bool disposing)" содержит код для освобождения неуправляемых ресурсов
    // ~SceneCameraBuffer()
    // {
    //     // Не изменяйте этот код. Разместите код очистки в методе "Dispose(bool disposing)".
    //     Dispose(disposing: false);
    // }

    public void Dispose()
    {
        // Не изменяйте этот код. Разместите код очистки в методе "Dispose(bool disposing)".
        Dispose(disposing: true);
        GC.SuppressFinalize(this);
    }
}