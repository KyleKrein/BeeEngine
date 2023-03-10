using OpenTK.Graphics.OpenGL4;
using OpenTK.Mathematics;
using OpenTK.Windowing.Desktop;
using OpenTK.Windowing.GraphicsLibraryFramework;
using ImGuiNET;
using System.Runtime.CompilerServices;
using System.Diagnostics;
using BeeEngine.Events;
using BeeEngine.OpenTK.Profiling;
using ErrorCode = OpenTK.Graphics.OpenGL4.ErrorCode;

namespace BeeEngine.Gui;

internal class ImGuiController : IDisposable
{
    private readonly int _osScale = 1;

    private int _vertexArray;
    private int _vertexBuffer;
    private int _vertexBufferSize;
    private int _indexBuffer;
    private int _indexBufferSize;

    //private Texture _fontTexture;

    private int _fontTexture;

    private int _shader;
    private int _shaderFontTextureLocation;
    private int _shaderProjectionMatrixLocation;

    private int _windowWidth;
    private int _windowHeight;

    private System.Numerics.Vector2 _scaleFactor;

    private static bool _khrDebugAvailable;
    
    /// <summary>
    /// Constructs a new ImGuiController.
    /// </summary>
    [ProfileMethod]
    public ImGuiController(int width, int height)
    {
        //DebugTimer.Start();
        if (Application.PlatformOS == OS.Mac)
        {
            _osScale = 2;
        }
        _windowWidth = width;
        _windowHeight = height;
        int major = GL.GetInteger(GetPName.MajorVersion);
        int minor = GL.GetInteger(GetPName.MinorVersion);

        _khrDebugAvailable = (major == 4 && minor >= 3) || IsExtensionSupported("KHR_debug");

        IntPtr context = ImGui.CreateContext();
        ImGui.SetCurrentContext(context);
        var io = ImGui.GetIO();
        
        io.Fonts.AddFontDefault();
        ImGui.StyleColorsDark();
        io.BackendFlags |= ImGuiBackendFlags.HasMouseCursors
                           | ImGuiBackendFlags.HasSetMousePos
                           | ImGuiBackendFlags.RendererHasVtxOffset;
        io.ConfigFlags = ImGuiConfigFlags.DockingEnable |
                         ImGuiConfigFlags.ViewportsEnable|
                         ImGuiConfigFlags.DpiEnableScaleViewports |
                         ImGuiConfigFlags.DpiEnableScaleFonts;
        var style = ImGui.GetStyle();
        if (io.ConfigFlags.HasFlag(ImGuiConfigFlags.ViewportsEnable))
        {
            style.WindowRounding = 0.0f;
            style.Colors[(int) ImGuiCol.WindowBg].W = 1.0f;
        }
        System.Numerics.Vector2 dpi;
        unsafe
        {
            float xScale;
            float yScale;
            
            GLFW.GetMonitorContentScaleRaw(GLFW.GetPrimaryMonitor(), &xScale, &yScale);
            DebugLog.Info($"DPI scale is x: {xScale}    y: {yScale}");
            dpi = new System.Numerics.Vector2(xScale, yScale);
            _scaleFactor = Application.PlatformOS == OS.Mac? dpi: System.Numerics.Vector2.One;
            //GLFW.WindowHint(WindowHintbool, "why");
            //GLFW.GetWindowSize(Game.Instance.GetWindow().WindowPtr,out _windowWidth, out _windowHeight);
            //GL.Viewport(0,0,_windowWidth, _windowHeight);
        }
        ImGui.GetStyle().ScaleAllSizes(dpi.X);

        CreateDeviceResources();
        SetKeyMappings();

        //SetPerFrameImGuiData(1f / 60f);

        //ImGui.NewFrame();
        //_frameBegun = true;
        //DebugTimer.End();
    }

    /*public void OnEvent(ref EventDispatcher e)
    {
        e.Dispatch<KeyDownEvent>(KeyDownEvent);
        e.Dispatch<MouseDownEvent>(MouseDownEvent);
    }

    private bool MouseDownEvent(MouseDownEvent arg)
    {
        
    }

    private bool KeyDownEvent(KeyDownEvent arg)
    {
        
    }*/

    public void WindowResized(int width, int height)
    {
        _windowWidth = width;
        _windowHeight = height;
        //GLFW.GetWindowSize(Game.Instance.GetWindow().WindowPtr,out _windowWidth, out _windowHeight);
        //_windowWidth *= _osScale;
        //_windowHeight *= _osScale;
    }

    public void DestroyDeviceObjects()
    {
        Dispose();
    }

    public void CreateDeviceResources()
    {
        DebugTimer.Start();
        _vertexBufferSize = 10000;
        _indexBufferSize = 2000;

        int prevVao = GL.GetInteger(GetPName.VertexArrayBinding);
        int prevArrayBuffer = GL.GetInteger(GetPName.ArrayBufferBinding);

        _vertexArray = GL.GenVertexArray();
        GL.BindVertexArray(_vertexArray);
        LabelObject(ObjectLabelIdentifier.VertexArray, _vertexArray, "ImGui");

        _vertexBuffer = GL.GenBuffer();
        GL.BindBuffer(BufferTarget.ArrayBuffer, _vertexBuffer);
        LabelObject(ObjectLabelIdentifier.Buffer, _vertexBuffer, "VBO: ImGui");
        GL.BufferData(BufferTarget.ArrayBuffer, _vertexBufferSize, IntPtr.Zero, BufferUsageHint.DynamicDraw);

        _indexBuffer = GL.GenBuffer();
        GL.BindBuffer(BufferTarget.ElementArrayBuffer, _indexBuffer);
        LabelObject(ObjectLabelIdentifier.Buffer, _indexBuffer, "EBO: ImGui");
        GL.BufferData(BufferTarget.ElementArrayBuffer, _indexBufferSize, IntPtr.Zero, BufferUsageHint.DynamicDraw);

        RecreateFontDeviceTexture();

        const string vertexSource = @"#version 330 core
uniform mat4 projection_matrix;
layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_texCoord;
layout(location = 2) in vec4 in_color;
out vec4 color;
out vec2 texCoord;
void main()
{
    gl_Position = projection_matrix * vec4(in_position, 0, 1);
    color = in_color;
    texCoord = in_texCoord;
}";
        const string fragmentSource = @"#version 330 core
uniform sampler2D in_fontTexture;
in vec4 color;
in vec2 texCoord;
out vec4 outputColor;
void main()
{
    outputColor = color * texture(in_fontTexture, texCoord);
}";

        _shader = CreateProgram("ImGui", vertexSource, fragmentSource);
        _shaderProjectionMatrixLocation = GL.GetUniformLocation(_shader, "projection_matrix");
        _shaderFontTextureLocation = GL.GetUniformLocation(_shader, "in_fontTexture");

        int stride = Unsafe.SizeOf<ImDrawVert>();
        GL.VertexAttribPointer(0, 2, VertexAttribPointerType.Float, false, stride, 0);
        GL.VertexAttribPointer(1, 2, VertexAttribPointerType.Float, false, stride, 8);
        GL.VertexAttribPointer(2, 4, VertexAttribPointerType.UnsignedByte, true, stride, 16);

        GL.EnableVertexAttribArray(0);
        GL.EnableVertexAttribArray(1);
        GL.EnableVertexAttribArray(2);

        GL.BindVertexArray(prevVao);
        GL.BindBuffer(BufferTarget.ArrayBuffer, prevArrayBuffer);

        CheckGLError("End of ImGui setup");
        DebugTimer.End();
    }

    /// <summary>
    /// Recreates the device texture used to render text.
    /// </summary>
    public void RecreateFontDeviceTexture()
    {
        DebugTimer.Start();
        ImGuiIOPtr io = ImGui.GetIO();
        io.Fonts.GetTexDataAsRGBA32(out IntPtr pixels, out int width, out int height, out int bytesPerPixel);

        int mips = (int) Math.Floor(Math.Log(Math.Max(width, height), 2));

        int prevActiveTexture = GL.GetInteger(GetPName.ActiveTexture);
        GL.ActiveTexture(TextureUnit.Texture0);
        int prevTexture2D = GL.GetInteger(GetPName.TextureBinding2D);

        _fontTexture = GL.GenTexture();
        GL.BindTexture(TextureTarget.Texture2D, _fontTexture);
        GL.TexStorage2D(TextureTarget2d.Texture2D, mips, SizedInternalFormat.Rgba8, width, height);
        LabelObject(ObjectLabelIdentifier.Texture, _fontTexture, "ImGui Text Atlas");

        GL.TexSubImage2D(TextureTarget.Texture2D, 0, 0, 0, width, height, PixelFormat.Bgra, PixelType.UnsignedByte,
            pixels);

        GL.GenerateMipmap(GenerateMipmapTarget.Texture2D);

        GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int) TextureWrapMode.Repeat);
        GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int) TextureWrapMode.Repeat);

        GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMaxLevel, mips - 1);

        GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int) TextureMagFilter.Linear);
        GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int) TextureMinFilter.Linear);

        // Restore state
        GL.BindTexture(TextureTarget.Texture2D, prevTexture2D);
        GL.ActiveTexture((TextureUnit) prevActiveTexture);

        io.Fonts.SetTexID(_fontTexture);

        io.Fonts.ClearTexData();
        DebugTimer.End();
    }

    /// <summary>
    /// Renders the ImGui draw list data.
    /// </summary>
    public void Render()
    {
        DebugTimer.Start();
        ImGui.Render();
        RenderImDrawData(ImGui.GetDrawData());
        DebugTimer.End();
    }
    Vector2 _mouseScroll = Vector2.Zero;
    /// <summary>
    /// Updates ImGui input and IO configuration state.
    /// </summary>
    public void Update(WindowHandler wnd, float deltaSeconds)
    {
        SetPerFrameImGuiData(deltaSeconds);
        UpdateImGuiInput(wnd);
        ImGui.NewFrame();
    }

    /// <summary>
    /// Sets per-frame data based on the associated window.
    /// This is called by Update(float).
    /// </summary>
    private void SetPerFrameImGuiData(float deltaSeconds)
    {
        ImGuiIOPtr io = ImGui.GetIO();
        io.DisplaySize = new System.Numerics.Vector2(
            _windowWidth / _scaleFactor.X,
            _windowHeight / _scaleFactor.Y);
        io.DisplayFramebufferScale = _scaleFactor;
        io.DeltaTime = deltaSeconds; // DeltaTime is in seconds.
    }

    private readonly List<char> _pressedChars = new List<char>();
    private readonly Key[] _enumKeysValues = Enum.GetValues<Key>();
    private float scale = 1;

    private void UpdateImGuiInput(WindowHandler wnd)
    {
        ImGuiIOPtr io = ImGui.GetIO();

        io.MouseDown[0] = Input.MouseKeyPressed(MouseButton.Left);
        io.MouseDown[1] = Input.MouseKeyPressed(MouseButton.Right);
        io.MouseDown[2] = Input.MouseKeyPressed(MouseButton.Middle);

        var screenPoint = new Vector2i((int) Input.MousePosition.X, (int) Input.MousePosition.Y);
        var point = screenPoint; //wnd.PointToClient(screenPoint);
        io.MousePos = new System.Numerics.Vector2(point.X, point.Y);
        io.MouseWheel = _mouseScroll.Y;
        io.MouseWheelH = _mouseScroll.X;
        _mouseScroll = Vector2.Zero;
        for (var index = 0; index < _enumKeysValues.Length; index++)
        {
            var key = _enumKeysValues[index];
            if (key == Key.Unknown)
            {
                continue;
            }

            io.KeysDown[(int) key] = Input.KeyPressed(key);
        }

        foreach (var c in _pressedChars)
        {
            io.AddInputCharacter(c);
        }

        _pressedChars.Clear();

        io.KeyCtrl = Input.KeyPressed(Key.LeftControl) || Input.KeyPressed(Key.RightControl);
        io.KeyAlt = Input.KeyPressed(Key.LeftAlt) || Input.KeyPressed(Key.RightAlt);
        io.KeyShift = Input.KeyPressed(Key.LeftShift) || Input.KeyPressed(Key.RightShift);
        io.KeySuper = Input.KeyPressed(Key.LeftSuper) || Input.KeyPressed(Key.RightSuper);
    }

    internal void PressChar(char keyChar)
    {
        _pressedChars.Add(keyChar);
    }

    internal void MouseScroll(Vector2 offset)
    {
        _mouseScroll = offset;
        /*ImGuiIOPtr io = ImGui.GetIO();

        io.MouseWheel = offset.Y;
        io.MouseWheelH = offset.X;*/
    }

    private static void SetKeyMappings()
    {
        ImGuiIOPtr io = ImGui.GetIO();
        io.KeyMap[(int) ImGuiKey.Tab] = (int) Keys.Tab;
        io.KeyMap[(int) ImGuiKey.LeftArrow] = (int) Keys.Left;
        io.KeyMap[(int) ImGuiKey.RightArrow] = (int) Keys.Right;
        io.KeyMap[(int) ImGuiKey.UpArrow] = (int) Keys.Up;
        io.KeyMap[(int) ImGuiKey.DownArrow] = (int) Keys.Down;
        io.KeyMap[(int) ImGuiKey.PageUp] = (int) Keys.PageUp;
        io.KeyMap[(int) ImGuiKey.PageDown] = (int) Keys.PageDown;
        io.KeyMap[(int) ImGuiKey.Home] = (int) Keys.Home;
        io.KeyMap[(int) ImGuiKey.End] = (int) Keys.End;
        io.KeyMap[(int) ImGuiKey.Delete] = (int) Keys.Delete;
        io.KeyMap[(int) ImGuiKey.Backspace] = (int) Keys.Backspace;
        io.KeyMap[(int) ImGuiKey.Enter] = (int) Keys.Enter;
        io.KeyMap[(int) ImGuiKey.Escape] = (int) Keys.Escape;
        io.KeyMap[(int) ImGuiKey.A] = (int) Keys.A;
        io.KeyMap[(int) ImGuiKey.C] = (int) Keys.C;
        io.KeyMap[(int) ImGuiKey.V] = (int) Keys.V;
        io.KeyMap[(int) ImGuiKey.X] = (int) Keys.X;
        io.KeyMap[(int) ImGuiKey.Y] = (int) Keys.Y;
        io.KeyMap[(int) ImGuiKey.Z] = (int) Keys.Z;
    }

    private void RenderImDrawData(ImDrawDataPtr draw_data)
    {
        DebugTimer.Start();
        if (draw_data.CmdListsCount == 0)
        {
            return;
        }

        // Get intial state.
        int prevVao = GL.GetInteger(GetPName.VertexArrayBinding);
        int prevArrayBuffer = GL.GetInteger(GetPName.ArrayBufferBinding);
        int prevProgram = GL.GetInteger(GetPName.CurrentProgram);
        bool prevBlendEnabled = GL.GetBoolean(GetPName.Blend);
        bool prevScissorTestEnabled = GL.GetBoolean(GetPName.ScissorTest);
        int prevBlendEquationRgb = GL.GetInteger(GetPName.BlendEquationRgb);
        int prevBlendEquationAlpha = GL.GetInteger(GetPName.BlendEquationAlpha);
        int prevBlendFuncSrcRgb = GL.GetInteger(GetPName.BlendSrcRgb);
        int prevBlendFuncSrcAlpha = GL.GetInteger(GetPName.BlendSrcAlpha);
        int prevBlendFuncDstRgb = GL.GetInteger(GetPName.BlendDstRgb);
        int prevBlendFuncDstAlpha = GL.GetInteger(GetPName.BlendDstAlpha);
        bool prevCullFaceEnabled = GL.GetBoolean(GetPName.CullFace);
        bool prevDepthTestEnabled = GL.GetBoolean(GetPName.DepthTest);
        int prevActiveTexture = GL.GetInteger(GetPName.ActiveTexture);
        GL.ActiveTexture(TextureUnit.Texture0);
        int prevTexture2D = GL.GetInteger(GetPName.TextureBinding2D);
        Span<int> prevScissorBox = stackalloc int[4];
        unsafe
        {
            fixed (int* iptr = &prevScissorBox[0])
            {
                GL.GetInteger(GetPName.ScissorBox, iptr);
            }
        }

        // Bind the element buffer (thru the VAO) so that we can resize it.
        GL.BindVertexArray(_vertexArray);
        // Bind the vertex buffer so that we can resize it.
        GL.BindBuffer(BufferTarget.ArrayBuffer, _vertexBuffer);
        for (int i = 0; i < draw_data.CmdListsCount; i++)
        {
            ImDrawListPtr cmdList = draw_data.CmdListsRange[i];

            int vertexSize = cmdList.VtxBuffer.Size * Unsafe.SizeOf<ImDrawVert>();
            if (vertexSize > _vertexBufferSize)
            {
                int newSize = (int) Math.Max(_vertexBufferSize * 1.5f, vertexSize);

                GL.BufferData(BufferTarget.ArrayBuffer, newSize, IntPtr.Zero, BufferUsageHint.DynamicDraw);
                _vertexBufferSize = newSize;

                DebugLog.Info($"Resized dear imgui vertex buffer to new size {_vertexBufferSize}");
            }

            int indexSize = cmdList.IdxBuffer.Size * sizeof(ushort);
            if (indexSize > _indexBufferSize)
            {
                int newSize = (int) Math.Max(_indexBufferSize * 1.5f, indexSize);
                GL.BufferData(BufferTarget.ElementArrayBuffer, newSize, IntPtr.Zero, BufferUsageHint.DynamicDraw);
                _indexBufferSize = newSize;

                DebugLog.Info($"Resized dear imgui index buffer to new size {_indexBufferSize}");
            }
        }
        
        // Setup orthographic projection matrix into our constant buffer
        ImGuiIOPtr io = ImGui.GetIO();
        
        Matrix4 mvp = Matrix4.CreateOrthographicOffCenter(
            0.0f,
            io.DisplaySize.X,
            io.DisplaySize.Y,
            0.0f,
            -1.0f,
            1.0f);

        GL.UseProgram(_shader);
        GL.UniformMatrix4(_shaderProjectionMatrixLocation, false, ref mvp);
        GL.Uniform1(_shaderFontTextureLocation, 0);
        CheckGLError("Projection");

        GL.BindVertexArray(_vertexArray);
        CheckGLError("VAO");

        draw_data.ScaleClipRects(io.DisplayFramebufferScale);

        GL.Enable(EnableCap.Blend);
        GL.Enable(EnableCap.ScissorTest);
        GL.BlendEquation(BlendEquationMode.FuncAdd);
        GL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);
        GL.Disable(EnableCap.CullFace);
        GL.Disable(EnableCap.DepthTest);

        // Render command lists
        for (int n = 0; n < draw_data.CmdListsCount; n++)
        {
            ImDrawListPtr cmd_list = draw_data.CmdListsRange[n];

            GL.BufferSubData(BufferTarget.ArrayBuffer, IntPtr.Zero,
                cmd_list.VtxBuffer.Size * Unsafe.SizeOf<ImDrawVert>(), cmd_list.VtxBuffer.Data);
            CheckGLError($"Data Vert {n}");

            GL.BufferSubData(BufferTarget.ElementArrayBuffer, IntPtr.Zero, cmd_list.IdxBuffer.Size * sizeof(ushort),
                cmd_list.IdxBuffer.Data);
            CheckGLError($"Data Idx {n}");
            
            for (int cmd_i = 0; cmd_i < cmd_list.CmdBuffer.Size; cmd_i++)
            {
                ImDrawCmdPtr pcmd = cmd_list.CmdBuffer[cmd_i];
                if (pcmd.UserCallback != IntPtr.Zero)
                {
                    Log.Error("ImGui UserCallback in {0} can't be IntPtr.Zero", nameof(ImDrawCmd));
                    return;
                }

                GL.ActiveTexture(TextureUnit.Texture0);
                GL.BindTexture(TextureTarget.Texture2D, (int) pcmd.TextureId);
                CheckGLError("Texture");

                // We do _windowHeight - (int)clip.W instead of (int)clip.Y because gl has flipped Y when it comes to these coordinates
                var clip = pcmd.ClipRect;
                GL.Scissor((int) clip.X, _windowHeight - (int) clip.W, (int) ((int) (clip.Z - clip.X)),
                    (int) ((int) (clip.W - clip.Y)));
                CheckGLError("Scissor");
                
                if ((io.BackendFlags & ImGuiBackendFlags.RendererHasVtxOffset) != 0)
                {
                    GL.DrawElementsBaseVertex(PrimitiveType.Triangles, (int) pcmd.ElemCount,
                        DrawElementsType.UnsignedShort, (IntPtr) (pcmd.IdxOffset * sizeof(ushort)),
                        unchecked((int) pcmd.VtxOffset));
                }
                else
                {
                    GL.DrawElements(BeginMode.Triangles, (int) pcmd.ElemCount, DrawElementsType.UnsignedShort,
                        (int) pcmd.IdxOffset * sizeof(ushort));
                }

                CheckGLError("Draw");
            }
        }
        

        GL.Disable(EnableCap.Blend);
        GL.Disable(EnableCap.ScissorTest);
        DebugTimer.Start("ResetState");
        // Reset state
        GL.BindTexture(TextureTarget.Texture2D, prevTexture2D);
        GL.ActiveTexture((TextureUnit) prevActiveTexture);
        GL.UseProgram(prevProgram);
        GL.BindVertexArray(prevVao);
        GL.Scissor(prevScissorBox[0], prevScissorBox[1], prevScissorBox[2], prevScissorBox[3]);
        GL.BindBuffer(BufferTarget.ArrayBuffer, prevArrayBuffer);
        GL.BlendEquationSeparate((BlendEquationMode) prevBlendEquationRgb, (BlendEquationMode) prevBlendEquationAlpha);
        GL.BlendFuncSeparate(
            (BlendingFactorSrc) prevBlendFuncSrcRgb,
            (BlendingFactorDest) prevBlendFuncDstRgb,
            (BlendingFactorSrc) prevBlendFuncSrcAlpha,
            (BlendingFactorDest) prevBlendFuncDstAlpha);
        if (prevBlendEnabled) GL.Enable(EnableCap.Blend);
        else GL.Disable(EnableCap.Blend);
        if (prevDepthTestEnabled) GL.Enable(EnableCap.DepthTest);
        else GL.Disable(EnableCap.DepthTest);
        if (prevCullFaceEnabled) GL.Enable(EnableCap.CullFace);
        else GL.Disable(EnableCap.CullFace);
        if (prevScissorTestEnabled) GL.Enable(EnableCap.ScissorTest);
        else GL.Disable(EnableCap.ScissorTest);
        DebugTimer.End("ResetState");
        DebugTimer.End();
    }

    /// <summary>
    /// Frees all graphics resources used by the renderer.
    /// </summary>
    public void Dispose()
    {
        GL.DeleteVertexArray(_vertexArray);
        GL.DeleteBuffer(_vertexBuffer);
        GL.DeleteBuffer(_indexBuffer);

        GL.DeleteTexture(_fontTexture);
        GL.DeleteProgram(_shader);
    }

    public static void LabelObject(ObjectLabelIdentifier objLabelIdent, int glObject, string name)
    {
        if (_khrDebugAvailable)
            GL.ObjectLabel(objLabelIdent, glObject, name.Length, name);
    }

    static bool IsExtensionSupported(string name)
    {
        int n = GL.GetInteger(GetPName.NumExtensions);
        for (int i = 0; i < n; i++)
        {
            string extension = GL.GetString(StringNameIndexed.Extensions, i);
            if (extension == name) return true;
        }

        return false;
    }

    public static int CreateProgram(string name, string vertexSource, string fragmentSoruce)
    {
        int program = GL.CreateProgram();
        LabelObject(ObjectLabelIdentifier.Program, program, $"Program: {name}");

        int vertex = CompileShader(name, global::OpenTK.Graphics.OpenGL4.ShaderType.VertexShader, vertexSource);
        int fragment = CompileShader(name, global::OpenTK.Graphics.OpenGL4.ShaderType.FragmentShader, fragmentSoruce);

        GL.AttachShader(program, vertex);
        GL.AttachShader(program, fragment);

        GL.LinkProgram(program);

        GL.GetProgram(program, GetProgramParameterName.LinkStatus, out int success);
        if (success == 0)
        {
            string info = GL.GetProgramInfoLog(program);
            Debug.WriteLine($"GL.LinkProgram had info log [{name}]:\n{info}");
        }

        GL.DetachShader(program, vertex);
        GL.DetachShader(program, fragment);

        GL.DeleteShader(vertex);
        GL.DeleteShader(fragment);

        return program;
    }

    private static int CompileShader(string name, global::OpenTK.Graphics.OpenGL4.ShaderType type, string source)
    {
        int shader = GL.CreateShader(type);
        LabelObject(ObjectLabelIdentifier.Shader, shader, $"Shader: {name}");

        GL.ShaderSource(shader, source);
        GL.CompileShader(shader);

        GL.GetShader(shader, ShaderParameter.CompileStatus, out int success);
        if (success == 0)
        {
            string info = GL.GetShaderInfoLog(shader);
            Debug.WriteLine($"GL.CompileShader for shader '{name}' [{type}] had info log:\n{info}");
        }

        return shader;
    }

    public static void CheckGLError(string title)
    {
        ErrorCode error;
        int i = 1;
        while ((error = GL.GetError()) != ErrorCode.NoError)
        {
            Log.Error($"{title} ({i++}): {error}");
        }
    }
}