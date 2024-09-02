using System.Runtime.CompilerServices;
using BeeEngine.Internal;

namespace BeeEngine.Renderer;

/// <summary>
/// The UniformBuffer struct represents a uniform buffer
/// on the GPU.
/// </summary>
public sealed class UniformBuffer : IDisposable
{
    public uint Size => m_SizeInBytes; // Size in bytes
    public Type StoredType => m_Type; // The type stored in the uniform buffer
    internal IntPtr m_Handle;
    private uint m_SizeInBytes;
    private Type m_Type;
    /// <summary>
    /// Creates a uniform buffer with size of type T.
    /// </summary>
    /// <typeparam name="T">The type of the uniform buffer.</typeparam>
    /// <returns>A new uniform buffer.</returns>
    public static UniformBuffer Create<T>() where T : unmanaged
    {
        return new UniformBuffer((uint)Unsafe.SizeOf<T>(), typeof(T));
    }
    /// <summary>
    /// Sets the data of the uniform buffer.
    /// </summary>
    /// <typeparam name="T">The type of the data to set. It must be
    /// the same type as the one used to create a UniformBuffer.</typeparam>
    /// <param name="data">The data to set.</param>
    /// <remarks>
    /// This method is a convenience wrapper around <see cref="SetData{T}(ref T)"/>.
    /// It creates a temporary variable and calls the other overload.
    /// </remarks>
    public void SetData<T>(T data) where T : unmanaged
    {
        SetData(ref data);
    }
    /// <summary>
    /// Sets the data of the uniform buffer.
    /// </summary>
    /// <typeparam name="T">The type of the data to set. It must be
    /// the same type as the one used to create a UniformBuffer.</typeparam>
    /// <param name="data">The data to set.</param>
    public void SetData<T>(ref T data) where T : unmanaged
    {
        Log.AssertAndThrow(!disposedValue, "Trying to access a disposed uniform buffer");
        Log.AssertAndThrow(m_Type == typeof(T), "Type mismatch. Must be {0} but got {1}", m_Type.FullName, typeof(T).FullName);
        InternalCalls.UniformBuffer_SetData(m_Handle, ref Unsafe.As<T, byte>(ref data), m_SizeInBytes);
    }
    private UniformBuffer(uint sizeBytes, Type type)
    {
        m_SizeInBytes = sizeBytes;
        m_Type = type;
        m_Handle = InternalCalls.UniformBuffer_CreateDefault(m_SizeInBytes);
    }
    private bool disposedValue;

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
            InternalCalls.UniformBuffer_Destroy(m_Handle);
            disposedValue = true;
        }
    }

    // // TODO: переопределить метод завершения, только если "Dispose(bool disposing)" содержит код для освобождения неуправляемых ресурсов
    ~UniformBuffer()
    {
        // Не изменяйте этот код. Разместите код очистки в методе "Dispose(bool disposing)".
        Dispose(disposing: false);
    }

    public void Dispose()
    {
        // Не изменяйте этот код. Разместите код очистки в методе "Dispose(bool disposing)".
        Dispose(disposing: true);
        GC.SuppressFinalize(this);
    }
}