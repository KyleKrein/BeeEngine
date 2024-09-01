using System.Buffers;

namespace BeeEngine.Renderer;

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

    public void AddBinding(UniformBuffer uniformBuffer)
    {
        Log.AssertAndThrow(m_Bindings != null, "BindingSetBuilder is in invalid state.");
        Log.AssertAndThrow(m_Next < MaxBindings, "Can't add more than {0} bindings.", MaxBindings);
        m_Bindings[m_Next++] = uniformBuffer.m_Handle;
    }

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