#include <iostream>

#include "core/Application.h"
inline namespace
{
    Application* gApplication = 0;
    GLuint gVertexArrayObject = 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT cmd, WPARAM wp, LPARAM lp)
{
    return DefWindowProc(hwnd, cmd, wp, lp);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE
    hPrevInstance, PSTR szCmdLine,
    int iCmdShow)
{
    //Create The Application Instance
    auto* gApplication = new Application;
    //Win32 Window Creation
    WNDCLASSEX wndclass;
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wndclass.lpszMenuName = 0;
    wndclass.lpszClassName = L"Anima TestBed";
    RegisterClassEx(&wndclass);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int clientWidth = 800;
    int clientHeight = 600;
    RECT windowRect;
    SetRect(&windowRect,
        (screenWidth / 2) - (clientWidth / 2),
        (screenHeight / 2) - (clientHeight / 2),
        (screenWidth / 2) + (clientWidth / 2),
        (screenHeight / 2) + (clientHeight / 2));
    DWORD style = (WS_OVERLAPPED | WS_CAPTION |
        WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
    // | WS_THICKFRAME to resize
    AdjustWindowRectEx(&windowRect, style, FALSE, 0);
    HWND hwnd = CreateWindowEx(0, wndclass.lpszClassName,
        L"Anima", style, windowRect.left,
        windowRect.top, windowRect.right -
        windowRect.left, windowRect.bottom -
        windowRect.top, NULL, NULL,
        hInstance, szCmdLine);
    HDC hdc = GetDC(hwnd);
    //Start of the OpenGL Creation
    //Get Pixel Format
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW
        | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);
    HGLRC tempRC = wglCreateContext(hdc);
    wglMakeCurrent(hdc, tempRC);
    PFNWGLCREATECONTEXTATTRIBSARBPROC
        wglCreateContextAttribsARB = NULL;
    wglCreateContextAttribsARB =
        (PFNWGLCREATECONTEXTATTRIBSARBPROC)
        wglGetProcAddress("wglCreateContextAttribsARB");
    const int attribList[9] = {
    0x2091, 4,
    0x2092, 6,
    0x2094, 0,
    0x9126,
    0x00000001,
    0, };
    HGLRC hglrc = wglCreateContextAttribsARB(
        hdc, 0, attribList);
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(tempRC);
    wglMakeCurrent(hdc, hglrc);
    //Now load glad.
    if(!gladLoadGL())
    {
        std::cout << "Failed to load glad gl\n";
        __debugbreak();
    }
    else {
        std::cout << "OpenGL Version " <<
            GLVersion.major << "." << GLVersion.minor <<
            "\n";
    }
    PFNWGLGETEXTENSIONSSTRINGEXTPROC
        _wglGetExtensionsStringEXT =
        (PFNWGLGETEXTENSIONSSTRINGEXTPROC)
        wglGetProcAddress("wglGetExtensionsStringEXT");
    bool swapControlSupported = strstr(
        _wglGetExtensionsStringEXT(),
        "WGL_EXT_swap_control") != 0;
    int vsynch = 0;
    if (swapControlSupported) {
        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
            (PFNWGLSWAPINTERVALEXTPROC)
            wglGetProcAddress("wglSwapIntervalEXT");
        PFNWGLGETSWAPINTERVALEXTPROC
            wglGetSwapIntervalEXT =
            (PFNWGLGETSWAPINTERVALEXTPROC)
            wglGetProcAddress("wglGetSwapIntervalEXT");
        if (wglSwapIntervalEXT(1)) {
            std::cout << "Enabled vsynch\n";
            vsynch = wglGetSwapIntervalEXT();
        }
        else {
            std::cout << "Could not enable vsynch\n";
        }
    }
    else { // !swapControlSupported
	    std::cout << "WGL_EXT_swap_control not supported\n";
    }
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    gApplication->Initialize();
    //Start Loop
    DWORD lastTick = GetTickCount();
    MSG msg;
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                PostQuitMessage(0);
                return  (int)msg.wParam;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        DWORD thisTick = GetTickCount();
        float dt = float(thisTick - lastTick) * 0.001f;
        lastTick = thisTick;
        if (gApplication != 0) {
            gApplication->Update(dt);
        }
        if (gApplication != 0) {
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            clientWidth = clientRect.right -
                clientRect.left;
            clientHeight = clientRect.bottom -
                clientRect.top;
            glViewport(0, 0, clientWidth, clientHeight);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glPointSize(5.0f);
            glBindVertexArray(gVertexArrayObject);
            glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            float aspect = (float)clientWidth /
                (float)clientHeight;
            gApplication->Render(aspect);
        }
        if (gApplication != 0) {
            SwapBuffers(hdc);
            if (vsynch != 0) {
                glFinish();
            }
        }
    } // End of game loop
    if (gApplication != 0) {
        std::cout << "Expected application to be null on exit\n";
        delete gApplication;
    }
    return (int)msg.wParam;
}
