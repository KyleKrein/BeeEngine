using BeeEngine.Events;
using BeeEngine.Gui;
using OpenTK.Mathematics;
using OpenTK.Windowing.Desktop;

namespace BeeEngine.Platform.OpenGL;

internal sealed class ImGuiLayerOpenGL: ImGuiLayer
{
    private WindowHandler _window;
    private ImGuiController _controller;

    public ImGuiLayerOpenGL()
    {
        _window = CrossPlatformWindow.Instance.GetWindow();
    }

    public override void OnAttach()
    {
        _controller = new ImGuiController(_window.Width, _window.Height);
    }

    public override void OnDetach()
    {
        _controller.Dispose();
    }

    public override void OnEvent(ref EventDispatcher dispatcher)
    {
        dispatcher.Dispatch<MouseScrolledEvent>(OnMouseScrolled);
        dispatcher.Dispatch<KeyTypedEvent>(OnKeyTyped);
        dispatcher.Dispatch<WindowResizedEvent>(OnWindowResized);
    }
    private bool OnMouseScrolled(MouseScrolledEvent e)
    {
        _controller.MouseScroll(new Vector2(e.OffsetHorizontal, e.Offset));
        return false;
    }

    private Vector2 _mouseWheelOffset = Vector2.Zero;
    private bool OnWindowResized(WindowResizedEvent e)
    {
        _controller.WindowResized(e.Width, e.Height);
        return false;
    }
    private bool OnKeyTyped(KeyTypedEvent e)
    {
        _controller.PressChar(e.KeyChar);
        return false;
    }

    public override void OnBegin()
    {
        _controller.Update(_window, Time.DeltaTime);
    }

    public override void OnGUIRendering()
    {
        //ImGui.ShowDemoWindow();
    }

    public override void OnEnd()
    {
        _controller.Render();
    }
    
    public override void OnUpdate()
    {
        
    }
}