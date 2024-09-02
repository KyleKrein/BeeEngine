using System.Buffers;

namespace BeeEngine.Renderer;
/// <summary>
/// The BindingSetBuilder struct is used to build a binding set.
/// If you instantiated an object of this struct, you MUST
/// call the Build method to create the binding set.
/// If you don't do this, you leak the resources.
/// </summary>
public ref struct BindingSetBuilder
{
    public const uint MaxBindings = 10;
    private IntPtr[]? m_Bindings;
    private int m_Next;

    public BindingSetBuilder()
    {
        m_Bindings = ArrayPool<IntPtr>.Shared.Rent((int)MaxBindings);
        m_Next = 0;
    }

    /// <summary>
    /// Adds a uniform buffer to the binding set.
    /// </summary>
    /// <param name="uniformBuffer">The uniform buffer to add.</param>
    /// <remarks>
    /// If the builder is in an invalid state, this method will throw an exception.
    /// If the maximum number of bindings has been reached, this method will throw an exception.
    /// </remarks>
    public void AddBinding(UniformBuffer uniformBuffer)
    {
        Log.AssertAndThrow(m_Bindings != null, "BindingSetBuilder is in invalid state.");
        Log.AssertAndThrow(m_Next < MaxBindings, "Can't add more than {0} bindings.", MaxBindings);
        m_Bindings[m_Next++] = uniformBuffer.m_Handle;
    }

    /// <summary>
    /// Builds the binding set. After calling this method, the builder is in an
    /// invalid state and should not be used anymore.
    /// </summary>
    /// <remarks>
    /// If the builder is in an invalid state, this method will throw an exception.
    /// After calling this method, the builder is in an invalid state.
    /// </remarks>
    /// <returns>The binding set.</returns>
    public BindingSet Build()
    {
        Log.AssertAndThrow(m_Bindings != null, "BindingSetBuilder is in invalid state.");
        var result = new BindingSet(m_Bindings[0..m_Next]);
        ArrayPool<IntPtr>.Shared.Return(m_Bindings);
        m_Bindings = null;
        m_Next = 0;
        return result;
    }
}