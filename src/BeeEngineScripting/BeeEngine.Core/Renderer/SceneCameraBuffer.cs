using BeeEngine.Math;

namespace BeeEngine.Renderer;

/// <summary>
/// Represents a camera buffer for 3D rendering.
/// This is a small abstraction around a uniform buffer and binding set.
/// </summary>
public sealed class SceneCameraBuffer : IDisposable
{
    internal readonly UniformBuffer m_UniformBuffer;
    internal readonly BindingSet m_BindingSet;
    /// <summary>
    /// The view projection matrix.
    /// The matrix is stored in the uniform buffer,
    /// therefore the set operation is expensive.
    /// </summary>
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

    /// <summary>
    /// Creates a camera buffer with a default orthographic projection,
    /// which is a projection with a width and height of 1.
    /// </summary>
    /// <returns>A new camera buffer.</returns>
    public static SceneCameraBuffer CreateDefault()
    {
        return CreateOrthographic(1, 1);
    }
    /// <summary>
    /// Creates a camera buffer with an orthographic projection.
    /// </summary>
    /// <param name="width">The width of the projection volume.</param>
    /// <param name="height">The height of the projection volume.</param>
    /// <returns>A new camera buffer.</returns>
    public static SceneCameraBuffer CreateOrthographic(uint width, uint height)
    {
        // Inverted y axis
        const float near = 1000f;
        const float far = -1.0f;
        return new SceneCameraBuffer(Matrix4.CreateOrthographic(width, height, near, far));
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