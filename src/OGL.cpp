#include <windows.h>

#include "OGL.h"

// opengl related libraries
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

// macros
#define WIN_WIDTH   800
#define WIN_HEIGHT  600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variable declarations
// fullscreen
BOOL gbFullScreen = FALSE;
HWND ghwnd = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev;

// active window related variable
BOOL gbActiveWindow = FALSE;

// exit key press related
BOOL gbEscapKeyIsPressed = FALSE;

// opengl related global variables
HDC ghdc = NULL;
HGLRC ghrc = NULL;

// shader related variables
Shader* triangleShader = nullptr;

enum {
    LRC_ATTRIBUTE_POSITION = 0,
};

enum {
    ortographic = 0,
    perspective
};

GLuint vao = 0;
GLuint vbo_position = 0;

GLuint mvpMatrixUniform = 0;

glm::mat4 perspectiveProjectionMatrix;

// Camera related variables
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
    // function declarations
    int  initialize(void);
    void display(void);
    void update(float);
    void uninitialize(void);

    // variable declarations
    WNDCLASSEX wndclass;
    HWND hwnd;
    MSG msg;
    TCHAR szAppName[] = TEXT("RTR6");
    BOOL bDone = FALSE;

    // create log file
    Logger::Init();
    LOG_INFO("Program started successfully\n");

    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName = NULL;
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassEx(&wndclass);

    hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("Lalit Choudhary"),
                            WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 
                            ((GetSystemMetrics(SM_CXSCREEN) - WIN_WIDTH) / 2), ((GetSystemMetrics(SM_CYSCREEN) - WIN_HEIGHT) / 2), 
                            WIN_WIDTH, WIN_HEIGHT,
                            NULL, NULL, hInstance, NULL);
    ghwnd = hwnd;

    ShowWindow(hwnd, iCmdShow);

    UpdateWindow(hwnd);

    // initialize
    int result = initialize();
    if(result != 0) {
        LOG_ERROR("Initialize() failed\n");
        DestroyWindow(hwnd);
        hwnd = NULL;
    }
    else {
        LOG_INFO("Initialize() successful\n");
    }

    // set this window as foreground and active window
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    // game loop
    while(bDone == FALSE) {

        Timer::Tick();

        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if(msg.message == WM_QUIT) {
                bDone = TRUE;
            }
            else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {
            if(gbActiveWindow == TRUE) {
                if(gbEscapKeyIsPressed == TRUE) {
                    bDone = TRUE;
                }
                // render
                display();
                // update
                update(Timer::GetDeltaTime());
            }
        }
    }

    uninitialize();

    return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    void toggleFullScreen(void);
    void resize(int, int);
    void uninitialize(void);
    
    switch(iMsg)
    {
        case WM_CREATE:
            ZeroMemory((void*)&wpPrev, sizeof(WINDOWPLACEMENT));
            wpPrev.length = sizeof(WINDOWPLACEMENT);
            break;
        
        case WM_SETFOCUS:
            gbActiveWindow = TRUE;
            break;
            
        case WM_KILLFOCUS:
            gbActiveWindow = FALSE;
            break;

        case WM_ERASEBKGND:
            return 0;

        case WM_SIZE:
            resize(LOWORD(lParam), HIWORD(lParam));
            break;

        case WM_KEYDOWN:
            switch(wParam) {
                case VK_ESCAPE:
                    gbEscapKeyIsPressed = TRUE;
                    break;

                default:
                    break;
            }
            Input::KeyDown(wParam);
            break;

        case WM_KEYUP:
            Input::KeyUp(wParam);
            break;

        case WM_CHAR:
            switch(wParam) {
                case 'F':
                case 'f':
                    if (gbFullScreen == FALSE) {
                        toggleFullScreen();
                        gbFullScreen = TRUE;
                    }
                    else {
                        toggleFullScreen();
                        gbFullScreen = FALSE;
                    }
                    break;

                default:
                    break;
            }
            break;

        case WM_CLOSE:
            uninitialize();
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            break;
    }
    
    return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

int initialize(void)
{
    // function prototype
    void printGLInfo(void);
    void resize(int, int);
    void uninitialize(void);
    int setupPixelFormat(void);

    // code
    int result = setupPixelFormat();
    if (result != 0) {
        LOG_ERROR("setupPixelFormat() failed");
        return result;
    }

    // initialize glew
    GLenum glewResult = glewInit();
    if (glewResult != GLEW_OK) {
        LOG_ERROR("glewInit() failed");
        return (-6);
    }

    // initialize timer
    Timer::Init();

    // print opengl information
    printGLInfo();

    triangleShader = new Shader("../../shaders/triangle.vs", "../../shaders/triangle.fs");
    
    // get the required uniform location from the shader
    mvpMatrixUniform = glGetUniformLocation(triangleShader->getID(), "uMVPMatrix");
    if (mvpMatrixUniform == -1) {
        LOG_ERROR("glGetUniformLocation() failed for uMVPMatrix");
        uninitialize();
    }

    // provide vertex position, color, normal, textcord, etc
    const GLfloat triangle_position[] = {   
                                            0.0f, 1.0f, 0.0f, 
                                            -1.0f, -1.0f, 0.0f,
                                            1.0f, -1.0f, 0.0f 
                                        };

    // vertex array object for arrays of vertex attributes
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_position), triangle_position, GL_STATIC_DRAW);
    glVertexAttribPointer(LRC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(LRC_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // depth related code
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // tell opengl to choose the color to clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    perspectiveProjectionMatrix = glm::mat4(1.0f);  // identity matrix

    // warmup resize
    resize(WIN_WIDTH, WIN_HEIGHT);

    return 0;
}

void display(void)
{
    // code
    // clear opengl buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use shader program object
    triangleShader->use();

    // transformations
    glm::mat4 modelMatrix = glm::mat4(1.0f);                // identity matrix
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 modelViewProjectionMatrix = glm::mat4(1.0f);
    glm::mat4 translationMatrix = glm::mat4(1.0f);

    viewMatrix = camera.GetViewMatrix();

    modelViewProjectionMatrix = perspectiveProjectionMatrix * viewMatrix * modelMatrix;

    // send this matrix to vertex shader in uniform variable
    triangleShader->setMat4("uMVPMatrix", modelViewProjectionMatrix);
    
    // bind with vao
    glBindVertexArray(vao);

    // draw the vertex arrays
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // unbind with vao
    glBindVertexArray(0);

    // unuse shader program object
    glUseProgram(0);

    // swap the buffers
    SwapBuffers(ghdc);
}

void update(float deltaTime) {
    if(Input::IsKeyPressed('W') || Input::IsKeyPressed('w')) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if(Input::IsKeyPressed('S') || Input::IsKeyPressed('s')) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if(Input::IsKeyPressed('A') || Input::IsKeyPressed('a')) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if(Input::IsKeyPressed('D') || Input::IsKeyPressed('d')) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void resize(int width, int height)
{
    if (height <= 0) {
        height = 1;
    }

    // set the viewport
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    
    perspectiveProjectionMatrix = glm::perspective(glm::radians(45.0f), (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void uninitialize(void)
{
    // functions declaratoins
    void toggleFullScreen(void);

    // code
    if (gbFullScreen == TRUE) {
        toggleFullScreen();
        gbFullScreen = FALSE;
    }

    // free vbo of position
    if (vbo_position) {
        glDeleteBuffers(1, &vbo_position);
        vbo_position = NULL;
    }

    // free vao
    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = NULL;
    }

    if (triangleShader) {
        delete triangleShader;
        triangleShader = nullptr;
    }

    // make hdc as current context by releasing rendering context as current context
    if (wglGetCurrentContext() == ghrc) {
        wglMakeCurrent(NULL, NULL);
    }

    // delete the rendering context
    if (ghrc) {
        wglDeleteContext(ghrc);
        ghrc = NULL;
    }

    // release the dc
    if (ghdc) {
        ReleaseDC(ghwnd, ghdc);
        ghdc = NULL;
    }
    
    // destory window
    if (ghwnd) {
        DestroyWindow(ghwnd);
        ghwnd = NULL;
    }

    LOG_INFO("Program terminated successfully\n");
    // close the file
    Logger::Shutdown();
}

void printGLInfo(void)
{
    int numExtensions, i;

    // code
    //glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

    // print opengl information
    LOG_INFO("OPENGL INFORMATION\n");
    LOG_INFO("------------------\n");
    LOG_INFO("OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
    LOG_INFO("OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
    LOG_INFO("OpenGL Version : %s\n", glGetString(GL_VERSION));
    LOG_INFO("GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    LOG_INFO("------------------\n");

    // print opengl information
    /*for (i = 0; i < numExtensions; ++i) {
        LOG_INFO("%s\n", glGetStringi(GL_EXTENSIONS, i));
    }*/
}

void toggleFullScreen(void)
{
    MONITORINFO mi;

    if (gbFullScreen == FALSE) {
        dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
        if(dwStyle & WS_OVERLAPPEDWINDOW) {
            ZeroMemory((void*)&mi, sizeof(MONITORINFO));
            mi.cbSize = sizeof(MONITORINFO);

            if(GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) {
                SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                                mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,
                                SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        ShowCursor(FALSE);
    }
    else {
        SetWindowPlacement(ghwnd, &wpPrev);
        SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowCursor(TRUE);
    }
}

int setupPixelFormat(void)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormatIndex = 0;
    
    // code
    ZeroMemory((void*)&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32;

    // get DC
    ghdc = GetDC(ghwnd);
    if (ghdc == NULL) {
        LOG_ERROR("GetDC() failed\n");
        return (-1);
    }

    // get matching pixel format index using hdc and pfd
    iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
    if (iPixelFormatIndex == 0) {
        LOG_ERROR("ChoosePixelFormat() failed");
        return (-2);
    }

    // select the pixel formats of found index
    if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) {
        LOG_ERROR("SetPixelFormat() failed");
        return (-3);
    }

    // create rendering context using hdc, pfd and chosen PixelFormatDesriptor
    ghrc = wglCreateContext(ghdc);
    if (ghrc == NULL) {
        LOG_ERROR("wglCreateContext() failed");
        return (-4);
    }
    
    // make this rendering context as current context
    if (wglMakeCurrent(ghdc, ghrc) == FALSE) {
        LOG_ERROR("wglMakeCurrent() failed");
        return (-5);
    }

    return 0;
}
