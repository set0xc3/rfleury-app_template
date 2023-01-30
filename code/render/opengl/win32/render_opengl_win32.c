global HINSTANCE gl_w32_hinstance = 0;
global HMODULE gl_w32_hmodule = 0;
global HGLRC gl_w32_ctx = 0;
global PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
global PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
global PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB;
global PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
global int gl_w32_pf_attribs_i[] =
{
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
    WGL_COLOR_BITS_ARB, 24,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_STENCIL_BITS_ARB, 8,
    WGL_SAMPLE_BUFFERS_ARB, 1,
    WGL_SAMPLES_ARB, 4,
    0
};
global int gl_w32_pixel_format = 0;
global PIXELFORMATDESCRIPTOR gl_w32_pfd = {sizeof(gl_w32_pfd)};

function void
GL_W32_LoadWGLFunctions(void)
{
    wglChoosePixelFormatARB    = (PFNWGLCHOOSEPIXELFORMATARBPROC)    GL_OS_GetProcAddress("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) GL_OS_GetProcAddress("wglCreateContextAttribsARB");
    wglMakeContextCurrentARB   = (PFNWGLMAKECONTEXTCURRENTARBPROC)   GL_OS_GetProcAddress("wglMakeContextCurrentARB");
    wglSwapIntervalEXT         = (PFNWGLSWAPINTERVALEXTPROC)         GL_OS_GetProcAddress("wglSwapIntervalEXT");
}

function VoidFunction *
GL_OS_GetProcAddress(char *name)
{
    VoidFunction *p = (VoidFunction *)wglGetProcAddress(name);
    if(!p || p == (VoidFunction*)0x1 || p == (VoidFunction*)0x2 || p == (VoidFunction*)0x3 || p == (VoidFunction*)-1)
    {
        p = 0;
    }
    if(p == 0)
    {
        p = (VoidFunction *)GetProcAddress(gl_w32_hmodule, name);
    }
    return p;
}

function void
GL_OS_EquipOS(void)
{
    gl_w32_hinstance = GetModuleHandle(0);
    gl_w32_hmodule = GetModuleHandle("opengl32.dll");
    
    //- rjf: make global invisible window
    HWND dummy_hwnd = CreateWindowW(L"STATIC",
                                    L"",
                                    WS_OVERLAPPEDWINDOW,
                                    CW_USEDEFAULT, CW_USEDEFAULT,
                                    100, 100,
                                    0, 0,
                                    gl_w32_hinstance,
                                    0);
    HDC dummy_hdc = GetDC(dummy_hwnd);
    
    //- rjf: make dummy context
    HGLRC gl_dummy_render_context = 0;
    int dummy_pixel_format = 0;
    {
        PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32,
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,
            8,
            0,
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };
        dummy_pixel_format = ChoosePixelFormat(dummy_hdc, &pfd);
        if(dummy_pixel_format == 0)
        {
            goto end;
        }
        SetPixelFormat(dummy_hdc, dummy_pixel_format, &pfd);
        gl_dummy_render_context = wglCreateContext(dummy_hdc);
        wglMakeCurrent(dummy_hdc, gl_dummy_render_context);
        GL_W32_LoadWGLFunctions();
        wglMakeCurrent(0, 0);
        wglDeleteContext(gl_dummy_render_context);
    }
    
    //- rjf: make global invisible window
    HWND dummy2_hwnd = CreateWindowW(L"STATIC",
                                     L"",
                                     WS_OVERLAPPEDWINDOW,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     100, 100,
                                     0, 0,
                                     gl_w32_hinstance,
                                     0);
    HDC dummy2_hdc = GetDC(dummy2_hwnd);
    
    //- rjf: setup real pixel format
    {
        UINT num_formats = 0;
        wglChoosePixelFormatARB(dummy2_hdc, gl_w32_pf_attribs_i, 0, 1, &gl_w32_pixel_format, &num_formats);
        DescribePixelFormat(dummy2_hdc, gl_w32_pixel_format, sizeof(gl_w32_pfd), &gl_w32_pfd);
        SetPixelFormat(dummy2_hdc, gl_w32_pixel_format, &gl_w32_pfd);
    }
    
    //- rjf: initialize real context
    {
        const int context_attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 5,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_DEBUG_BIT_ARB,
            0
        };
        gl_w32_ctx = wglCreateContextAttribsARB(dummy2_hdc, 0, context_attribs);
        if(gl_w32_ctx)
        {
            BOOL make_current_good = wglMakeCurrent(dummy2_hdc, gl_w32_ctx);
            wglSwapIntervalEXT(1);
        }
    }
    
    end:;
    ReleaseDC(dummy_hwnd, dummy_hdc);
    DestroyWindow(dummy_hwnd);
}

function R_Handle
GL_OS_EquipWindow(R_Handle os_equip, OS_Handle window)
{
    HWND hwnd = (HWND)window.u64[0];
    HDC hdc = GetDC(hwnd);
    BOOL set_pixel_format_good = SetPixelFormat(hdc, gl_w32_pixel_format, &gl_w32_pfd);
    ReleaseDC(hwnd, hdc);
    R_Handle result = {0};
    return result;
}

function void
GL_OS_UnequipWindow(R_Handle os_equip, R_Handle window_equip, OS_Handle window)
{
}

function void
GL_OS_SelectWindow(R_Handle os_equip, R_Handle window_equip, OS_Handle window)
{
    HWND hwnd = (HWND)window.u64[0];
    HDC hdc = GetDC(hwnd);
    BOOL make_current_good = wglMakeCurrent(hdc, gl_w32_ctx);
    ReleaseDC(hwnd, hdc);
}

function void
GL_OS_Finish(R_Handle os_equip, R_Handle window_equip, OS_Handle window)
{
    HWND hwnd = (HWND)window.u64[0];
    HDC hdc = GetDC(hwnd);
    SwapBuffers(hdc);
    ReleaseDC(hwnd, hdc);
}
