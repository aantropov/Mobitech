#include "Renderer.h"
#include "Mobitech.h"
#include "Utils.hpp"

const string Window:: WND_CLASS_NAME = "MobitechWindow";

GLenum g_OpenGLError = GL_NO_ERROR;

bool Window::active;
bool Window::fullscreen;
bool Window::running;

#ifdef MOBITECH_WIN32
HWND Window::hwnd = NULL;
HDC Window::hdc = NULL;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // MOBITECH_WIN32

Window::Window()
{
    width = 320;
    height = 240;
    fullscreen = false;
}

Window::~Window()
{
}

bool Window:: Create(string title, int width, int height, bool fullScreen)
{
    Window::width = width;
    Window::height = height;
    Window::fullscreen = fullScreen;

#ifdef MOBITECH_WIN32

    WNDCLASSEX            wcx;
    PIXELFORMATDESCRIPTOR pfd;
    RECT                  rect;
    HGLRC                 hRCTemp;
    DWORD                 style, exStyle;
    int                   x, y, format;

#ifdef MOBITECH_DEBUG
    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB ,
        0
    };
#endif //MOBITECH_DEBUG

#ifdef MOBITECH_RELEASE
    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
#endif //MOBITECH_RELEASE

    hinstance = static_cast<HINSTANCE>(GetModuleHandle(NULL));

    // Window class registration
    memset(&wcx, 0, sizeof(wcx));
    wcx.cbSize        = sizeof(wcx);
    wcx.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcx.lpfnWndProc   = reinterpret_cast<WNDPROC>(WindowProc);
    wcx.hInstance     = hinstance;
    wcx.lpszClassName = (LPCSTR)WND_CLASS_NAME.c_str();
    wcx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassEx(&wcx))
    {
        char message[BUFFER_LENGTH];
        sprintf_s(message,"RegisterClassEx fail (%d)", GetLastError());
        Logger::Message(message, LT_ERROR);
        return false;
    }

    // Window styles
    style   = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    exStyle = WS_EX_APPWINDOW;

    // Centrate window
    x = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
    y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    rect.left   = x; 
    rect.right  = x + width;
    rect.top    = y;
    rect.bottom = y + height;

    // Setup window size with styles
    AdjustWindowRectEx (&rect, style, FALSE, exStyle);

    // Create window
    hwnd = CreateWindowEx(exStyle, (LPCSTR)WND_CLASS_NAME.c_str(), (LPCSTR)title.c_str(), style, rect.left, rect.top,
        rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hinstance, NULL);

    if (!hwnd)
    {
        char message[BUFFER_LENGTH];
        sprintf_s(message,"CreateWindowEx fail (%d)", GetLastError());
        Logger::Message(message, LT_ERROR);
        return false;
    }

    // ÔGet window descriptor
    hdc = GetDC(hwnd);

    if (!hdc)
    {
        char message[BUFFER_LENGTH];
        sprintf_s(message,"GetDC fail (%d)", GetLastError());
        Logger::Message(message, LT_ERROR);
        return false;
    }

    // Pixel format description
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;

    // Get pixel format for format which is described above
    format = ChoosePixelFormat(hdc, &pfd);
    if (!format || !SetPixelFormat(hdc, format, &pfd))
    {
        char message[BUFFER_LENGTH];
        sprintf_s(message,"Setting pixel format fail (%d)", GetLastError());
        Logger::Message(message, LT_ERROR);
        return false;
    }

    // Create temp rendering context
    // It is neccessary for getting wglCreateContextAttribsARB function
    hRCTemp = wglCreateContext(hdc);
    if (!hRCTemp || !wglMakeCurrent(hdc, hRCTemp))
    {
        char message[BUFFER_LENGTH];
        sprintf_s(message,"—reating temp render context fail (%d)", GetLastError());
        Logger::Message(message, LT_ERROR);
        return false;
    }

    GLenum err = glewInit();
    OPENGL_CHECK_FOR_ERRORS();
    // Get function address
    //wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
    //wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    //OPENGL_GET_PROC(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);

    // Delete temp context
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRCTemp);

    if (!wglCreateContextAttribsARB)
    {
        char message[BUFFER_LENGTH];
        sprintf_s(message,"wglCreateContextAttribsARB fail (%d), try to create context with wglCreateContext", GetLastError());
        Logger::GetInstance()->Message(message, LOG_TYPE::LT_ERROR);
        //return false;

        // Create context with OpenGL 2 support
        hrc = wglCreateContext(hdc);
    }
    else
    {
        // Create extended context with OpenGL 3 support
        hrc = wglCreateContextAttribsARB(hdc, 0, attribs);
    }

    if (!hrc || !wglMakeCurrent(hdc, hrc))
    {
        char message[BUFFER_LENGTH];
        sprintf_s(message,"Creating render context fail (%d)", GetLastError());
        Logger::GetInstance()->Message(message, LOG_TYPE::LT_ERROR);
        return false;
    }

    Logger::Message("Window: Window is created succesfully");
#endif // MOBITECH_WIN32

    SetSize(width, height, fullScreen);
    return true;
}

void Window::SetSize(int width, int height, bool is_fullScreen)
{

#ifdef MOBITECH_WIN32
    RECT    rect;
    DWORD   style, exStyle;
    DEVMODE dev_mode;
    LONG    result;
    int     x, y;

    // If we change from fullscreen mode
    if (fullscreen && !is_fullScreen)
    {
        ChangeDisplaySettings(NULL, CDS_RESET);
        ShowCursor(TRUE);
    }

    fullscreen = is_fullScreen;

    // Fullscreen mode
    if (fullscreen)
    {
        memset(&dev_mode, 0, sizeof(dev_mode));
        dev_mode.dmSize       = sizeof(dev_mode);
        dev_mode.dmPelsWidth  = width;
        dev_mode.dmPelsHeight = height;
        dev_mode.dmBitsPerPel = GetDeviceCaps(hdc, BITSPIXEL);
        dev_mode.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Try to set fullscreen mode
        result = ChangeDisplaySettings(&dev_mode, CDS_FULLSCREEN);
        if (result != DISP_CHANGE_SUCCESSFUL)
        {
            char message[BUFFER_LENGTH];
            sprintf_s(message,"ChangeDisplaySettings fail %dx%d (%d)", width, height, result);
            Logger::Message(message, LOG_TYPE::LT_ERROR);
            fullscreen = false;
        }
    }

    // If fullscreen mode setuped succesfully
    if (fullscreen)
    {
        ShowCursor(FALSE);
        style   = WS_POPUP;
        exStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;

        x = y = 0;
    } 
    else 
    { //Window
        style   = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        exStyle = WS_EX_APPWINDOW;

        // Centralize window
        x = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
        y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
    }

    rect.left   = x;
    rect.right  = x + width;
    rect.top    = y;
    rect.bottom = y + height;

    //Setup styles
    AdjustWindowRectEx (&rect, style, FALSE, exStyle);

    SetWindowLong(hwnd, GWL_STYLE,   style);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

    // Refresh window position
    SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top,
        rect.right - rect.left, rect.bottom - rect.top,
        SWP_FRAMECHANGED);

    // Show
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
    UpdateWindow(hwnd);

    // Get sizes of the window
    GetClientRect(hwnd, &rect);
    width  = rect.right - rect.left;
    height = rect.bottom - rect.top;
        
    // Centralize cursor in the center of the screen
    SetCursorPos(x + width / 2, y + height / 2);

    OPENGL_CHECK_FOR_ERRORS();
#endif // MOBITECH_WIN32

    OPENGL_CALL(glViewport(0, 0, width, height));
}

void Window:: Destroy()
{
 #ifdef MOBITECH_WIN32   
    
    // Restore window size
    if (fullscreen)
    {
        ChangeDisplaySettings(NULL, CDS_RESET);
        ShowCursor(TRUE);
    }

    // Release renderer`s context 
    if (hrc)
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hrc);
    }

    // Release window context
    if (hdc)
        ReleaseDC(hwnd, hdc);

    // Destroy window
    if (hwnd)
        DestroyWindow(hwnd);

    // Release window class
    if (hinstance)
        UnregisterClassA((LPCSTR)WND_CLASS_NAME.c_str(), hinstance);
#endif // MOBITECH_WIN32
}

#ifdef MOBITECH_WIN32
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        {
            int mouse_x = (int)LOWORD(lParam);
            int mouse_y = (int)HIWORD(lParam);

            if (msg == WM_LBUTTONDOWN)
                Input::GetInstance()->OnTouchDown(mouse_x, mouse_y);
            if (msg == WM_LBUTTONUP)
                Input::GetInstance()->OnTouchUp(mouse_x, mouse_y);

            return FALSE;
        }

    case WM_MOUSEMOVE:
        {
            int mouse_x = (int)LOWORD(lParam);
            int mouse_y = (int)HIWORD(lParam);

            Input::GetInstance()->OnTouchMove(mouse_x, mouse_y);
            return FALSE;
        }

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        {
            //if (wParam < 256 && (lParam & 0x40000000) == 0)
                //g_input.keyState[wParam] = UINPUT_PRESSED;

            return FALSE;
        }
    case WM_KEYUP:
    case WM_SYSKEYUP:
        {
            //if (wParam < 256)
                //g_input.keyState[wParam] = UINPUT_UP;

            return FALSE;
        }

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
        Window::SetActive(msg == WM_SETFOCUS);
        return FALSE;

    case WM_ACTIVATE:
        Window::SetActive(LOWORD(wParam) == WA_INACTIVE);
        return FALSE;

    case WM_CLOSE:
        {
            Window::SetActive(false);
            Window::SetRunning(false);

            PostQuitMessage(0);
            return FALSE;
        }
    case WM_SYSCOMMAND:
        {    switch (wParam & 0xFFF0)
        {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
            if (Window::IsFullscreen())
                return FALSE;
            break;

        case SC_KEYMENU:
            return FALSE;
        }
        break;
        }
    case WM_ERASEBKGND:
        return FALSE;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
#endif // MOBITECH_WIN32

vec3 Camera::GetPosition() const { return position; }
vec3 Camera::GetRotation() const { return rotation; }

mat4 Camera::GetProjection() const { return projection; }
mat4 Camera::GetView() const { return GLRotation(rotation.x, rotation.y, rotation.z) * GLTranslation(-position); }

void Camera::UpdateFrustum()
{
    frustum_planes.extract(GetView(), GetProjection());
}

void Camera::Create(float x, float y, float z)
{
    position   = vec3(x, y, z);
    projection = mat4_identity;
}

void Camera::Perspective(float fov, float aspect, float zNear, float zFar)
{
    projection = GLPerspective(fov, aspect, zNear, zFar);
}

void Camera::Ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    projection = GLOrthographic(left, right, bottom, top, zNear, zFar);
}

void Camera::Rotate(float x, float y, float z)
{
    rotation += vec3(x, y, z);
}

void Camera::Move( float x, float y, float z)
{
    vec3 move = transpose(mat3(GLRotation(rotation.x,rotation.y,rotation.z))) * vec3(x, y, z);
    //vec3 move = vec3(x,y,z);
    position += move;
}

void Camera::LookAt(const vec3 &position, const vec3 &center, const vec3 &up)
{
    rotation = GLToEuler(GLLookAt(position, center, up));
    this->position = position;
}

Camera::Camera(void){ rotation = vec3_zero; }
Camera:: ~Camera(void){}

Renderer::Renderer()
{
    previous_vao = 0;
    previous_ib = 0;
}

Renderer::~Renderer()
{
}

Renderer* Renderer::GetInstance()
{
    if(instance == NULL)
        instance = new Renderer();        
    return instance;
}

void Renderer:: SetupCameraForShaderProgram(ShaderProgram *shd, mat4 &model)
{
    mat4 view           = current_camera->GetView();
    mat4 viewProjection = current_camera->GetProjection() * view;
    mat4 modelViewProjection = transpose(model * viewProjection);
    
    mat4 modelTr = transpose(model);
    view = transpose(view);
    viewProjection = transpose(viewProjection);

    //mat3 normal         = transpose(mat3(inverse(model)));
    
    UniformMatrix4(shd->uniform_locations.transform_model,  1, modelTr.m);
    UniformMatrix4(shd->uniform_locations.transform_viewProjection, 1, viewProjection.m);
    UniformMatrix4(shd->uniform_locations.transform_modelViewProjection, 1, modelViewProjection.m);
    Uniform3(shd->uniform_locations.transform_viewPosition, 1, current_camera->GetPosition().v);
    //UniformMatrix3(shd->uniform_locations.transform_normal, 1, normal.m);
}

void Renderer:: SetCurrentCamera(Camera *cam)
{
    current_camera = cam;
}

Camera* Renderer:: GetCurrentCamera() const
{
    return current_camera;
}

void Renderer:: BindTexture(Texture *tex)
{
    BindTexture(tex, 0);
}

void Renderer:: BindTexture(Texture *tex, unsigned int channel)
{
    if(tex_channels_cache[channel] == tex->GetId())
        return;
    else
    {
        tex_channels_cache[channel] = tex->GetId();
        glActiveTexture(GL_TEXTURE0 + channel);
        glBindTexture(GL_TEXTURE_2D, tex->GetId());
        CacheUniform1(tex->name, channel);
    }
}

int Renderer:: CreateTexture(Texture *tex) const
{
    GLuint texture = tex->GetId();

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger ( IL_IMAGE_FORMAT ) , tex->GetWidth(), tex->GetHeight(), 0, ilGetInteger ( IL_IMAGE_FORMAT ) , ilGetInteger ( IL_IMAGE_TYPE    ), ilGetData()));
#ifdef MOBITECH_WIN32
    OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, 4, tex->GetWidth(), tex->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, &tex->GetData()[0]));
#else
    OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->GetWidth(), tex->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, &tex->GetData()[0]));
#endif //MOBITECH_WIN32
    OPENGL_CHECK_FOR_ERRORS();
    return texture;
}

void Renderer:: DeleteTexture(Texture *tex) const
{
    GLuint t = tex->GetId();
    OPENGL_CALL(glDeleteTextures(1, &t));
}

int Renderer::CreateVBO(VertexBuffer *vb, BUFFER_TYPE state) const
{
    int size = vb->GetNum() * sizeof(Vertex);

    GLuint vbo;
    OPENGL_CALL(glGenBuffers ( 1, &vbo ));
    glBindBuffer(GL_ARRAY_BUFFER , vbo );
    OPENGL_CALL(glBufferData(GL_ARRAY_BUFFER , size, vb->GetPointer(), state));

    return vbo;
}

int Renderer:: CreateVBO(IndexBuffer *ib, BUFFER_TYPE state) const
{
    int size = ib->GetNum()*sizeof(unsigned int);

    GLuint vbo;
    OPENGL_CALL(glGenBuffers ( 1, &vbo ));
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , vbo );
    OPENGL_CALL(glBufferData ( GL_ELEMENT_ARRAY_BUFFER , size, ib->GetPointer(), state));         

    return vbo;
}

void Renderer:: DeleteVBO(Buffer *vb) const
{
    GLuint vbo =  vb->GetId();
    OPENGL_CALL(glDeleteBuffers(1, &vbo));
}

void Renderer:: DrawSegment(const vec3& p1, const vec3& p2, const vec3& color) const
{    /*
    glColor4f(color.x, color.y, color.z, 1);    
    GLfloat glVertices[] = 
    {
        p1.x, p1.y, p1.z, p2.x, p2.y, p2.z
    };

    glVertexPointer(3, GL_FLOAT, 0, glVertices);
    glDrawArrays(GL_LINES, 0, 2);*/
}

void Renderer:: DrawTransform(::transform xf) const
{
    /*
    vec3 p1 = xf.position;
    vec3 p2;

    const float k_axisScale = 0.4f;

    p2 = p1 + (xf.matrix() * vec4_x) * k_axisScale;
    DrawSegment(p1, p2, vec3(1,0,0));

    p2 = p1 + (xf.matrix() * vec4_y) * k_axisScale;
    DrawSegment(p1, p2, vec3(0,1,0));

    p2 = p1 + (xf.matrix() * vec4_z) * k_axisScale;
    DrawSegment(p1, p2, vec3(0,0,1));*/
}

void Renderer:: DrawSolidPolygon(const Vertex* vertices, int vertex_count, const vec4 color)  const
{

    /*GLfloat glverts[24];
    glVertexPointer(3, GL_FLOAT, 0, glverts);
    glEnableClientState(GL_VERTEX_ARRAY);

    for (int i = 0; i < vertex_count; i++) 
    {
        glverts[i*3]   = vertices[i].pos.x;
        glverts[i*3+1] = vertices[i].pos.y;
        glverts[i*3+2] = vertices[i].pos.z;
    }

    glColor4f(color.x, color.y, color.z, 1);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_count);
    
    glLineWidth(3);
    glColor4f(1, 0, 1, 1 );
    glDrawArrays(GL_LINE_LOOP, 0, vertex_count);*/
}

void Renderer:: DrawBuffer(VertexBuffer* vb)
{    
    draw_calls++;
    OPENGL_CALL(glDrawArrays(GL_TRIANGLES, 0, vb->GetNum()));    
}

void Renderer:: DrawBuffer(IndexBuffer* ib)
{    
    draw_calls++;
    OPENGL_CALL(glDrawElements(GL_TRIANGLES, ib->GetNum(), GL_UNSIGNED_INT, NULL));    
}

void Renderer:: BindBuffer(VertexBuffer *vb) const
{    
    glBindBuffer(GL_ARRAY_BUFFER , vb->GetId());    
}

void Renderer:: BindBuffer(IndexBuffer *vb)
{
    if(previous_ib != vb->GetId())
    {
        previous_ib = vb->GetId();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , vb->GetId());
    }
}

void Renderer:: UnbindBuffer(bool vertex_buffer) const
{
    if(vertex_buffer)
        glBindBuffer ( GL_ARRAY_BUFFER , 0 ); 
    else
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , 0 ); 
}    

int Renderer:: CreateVAO() const
{
    GLuint vao;
    //OPENGL_CALL(glGenVertexArrays ( 1, &vao ));
    //glBindVertexArray( vao );        
    return vao;
}

void Renderer:: DeleteVAO(VertexArrayObject *vao) const
{
    //GLuint id =  vao->GetId();
    //OPENGL_CALL(glDeleteVertexArrays(1, &id));
}

void Renderer:: BindVAO(VertexBuffer *vb)
{
    /*if(previous_vao != vb->GetVAO()->GetId())
    {
        previous_vao = vb->GetVAO()->GetId();
        glBindVertexArray(vb->GetVAO()->GetId());
    }*/
}

void Renderer:: UnbindVAO() const
{
    //glBindVertexArray(0);    
}

int Renderer:: CompileShader(std::string source, SHADER_TYPE st) const
{
    GLuint shd;
    GLchar *strings = (GLchar*)source.c_str();

    shd = glCreateShader(st); 
    glShaderSource(shd, 1, (const GLchar**)&strings, NULL);
    glCompileShader(shd);

    GLint status = 0;
    glGetShaderiv(shd, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE)
    {
        GLint infoLogLength = 0;
        glGetShaderiv(shd, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shd, infoLogLength, NULL, strInfoLog);

        Logger::Message(string((char*)strInfoLog), LT_ERROR);    
    }

    return shd;
}

void Renderer:: DeleteShader(Shader *shd) const
{
    OPENGL_CALL(glDeleteShader(shd->GetId()));
}

int Renderer:: CreateShaderProgram(Shader *vertex_sh, Shader *pixel_sh) const
{
    GLuint sh_pr_id = glCreateProgram();
    OPENGL_CALL(glAttachShader(sh_pr_id, vertex_sh->GetId()));
    OPENGL_CALL(glAttachShader(sh_pr_id, pixel_sh->GetId()));
    OPENGL_CALL(glLinkProgram(sh_pr_id));
    return sh_pr_id;
}

void Renderer:: DeleteShaderProgram(ShaderProgram *shd) const
{
    OPENGL_CALL(glDeleteProgram(shd->GetId()));
}

void Renderer:: BindShaderProgram(ShaderProgram *sh)
{
    if(sh == NULL || (shader_program != NULL && sh == shader_program))
        return;

    shader_program = sh;
    OPENGL_CALL(glUseProgram(sh->GetId()));
    OPENGL_CHECK_FOR_ERRORS();
}

int Renderer:: CacheUniformLocation(string name)
{
    return CacheUniformLocation(name, shader_program);
}

int Renderer:: CacheUniformLocation(string name, ShaderProgram *sh)
{
    unsigned int *res = &uniforms_cache[sh->GetId()][name];
    if(*res == 0)
    {
        unsigned int  loc = glGetUniformLocation(sh->GetId(),  name.c_str());

        if(loc > MAX_UNIFORM_LOCATIONS)
            loc = MAX_UNIFORM_LOCATIONS+1;
        *res = loc;
    }
    return *res;
}

void Renderer:: CacheUniform4(ShaderProgram *sh, std::string name, unsigned int num , float *variable)
{
    Uniform4(CacheUniformLocation(name, sh),  num, variable);
}

void Renderer:: CacheUniform4(std::string name, unsigned int num , float *variable)
{
    Uniform4(CacheUniformLocation(name),  num, variable);
}

void Renderer:: CacheUniformMatrix4(std::string name, unsigned int num , float *variable)
{    
    UniformMatrix4(CacheUniformLocation(name), num, variable);
}

void Renderer:: CacheUniformMatrix3(ShaderProgram *sh, std::string name, unsigned int num , float *variable)
{    
    UniformMatrix3(CacheUniformLocation(name, sh), num, variable);
}

void Renderer:: CacheUniformMatrix4(ShaderProgram *sh, std::string name, unsigned int num , float *variable)
{    
    UniformMatrix4(CacheUniformLocation(name, sh), num, variable);
}

void Renderer:: CacheUniformMatrix3(std::string name, unsigned int num , float *variable)
{    
    UniformMatrix3(CacheUniformLocation(name), num, variable);
}

void Renderer:: CacheUniform1(ShaderProgram *sh, std::string name, unsigned int num , float *variable)
{
    Uniform1(CacheUniformLocation(name, sh),  num, variable);
}

void Renderer:: CacheUniform1(std::string name, unsigned int num , float *variable)
{
    Uniform1(CacheUniformLocation(name),  num, variable);
}

void Renderer:: CacheUniform1(std::string name, int value)
{
    Uniform1(CacheUniformLocation(name), value);
}

void Renderer:: CacheUniform3(ShaderProgram *sh, std::string name, unsigned int num , float *variable)
{
    Uniform3(CacheUniformLocation(name, sh),  num, variable);
}

void Renderer:: CacheUniform3(std::string name, unsigned int num , float *variable)
{
    Uniform3(CacheUniformLocation(name),  num, variable);
}

void Renderer:: Uniform4(unsigned int location, unsigned int num , float *variable) const
{
    if(location < MAX_UNIFORM_LOCATIONS)
        glUniform4fv(location,  num, variable);
}

void Renderer:: UniformMatrix4(unsigned int location, unsigned int num , float *variable) const
{    
    if(location < MAX_UNIFORM_LOCATIONS)
        glUniformMatrix4fv(location, num, GL_FALSE, variable);
}

void Renderer:: UniformMatrix3(unsigned int location, unsigned int num , float *variable) const
{    
    if(location < MAX_UNIFORM_LOCATIONS)
        glUniformMatrix3fv(location, num, GL_FALSE, variable);
}

void Renderer:: Uniform1(unsigned int location, unsigned int num , float *variable) const
{
    if(location < MAX_UNIFORM_LOCATIONS)
        glUniform1fv(location,  num, variable);
}

void Renderer:: Uniform1(unsigned int location, int value) const
{
    if(location < MAX_UNIFORM_LOCATIONS)
        glUniform1i(location, value);
}

void Renderer:: Uniform3(unsigned int location, unsigned int num , float *variable) const
{
    if(location < MAX_UNIFORM_LOCATIONS)
        glUniform3fv(location,  num, variable);
}

bool Renderer:: SetVerticalSynchronization(bool bEnabled)
{
#ifdef MOBITECH_WIN32
    if(!wglSwapIntervalEXT)
        return false;

    if(bEnabled)
        wglSwapIntervalEXT(1);
    else
        wglSwapIntervalEXT(0);
#endif //MOBITECH_WIN32
    return true;
}

bool Renderer::Initialize()
{
    int width = 320;
    int height = 240;
    bool fullscreen = false;
    bool vsync = false;

#ifdef MOBITECH_WIN32
    TiXmlDocument document((ASSETS_ROOT + "/config.xml").c_str());
    document.LoadFile(TIXML_ENCODING_UTF8);
    TiXmlElement *config = document.FirstChildElement("config");

    for (TiXmlElement *platform = config->FirstChildElement("platform"); platform; platform = platform->NextSiblingElement("platform"))
    {
        const char *name = platform->Attribute("name");
        if (strcmp(name, MOBITECH_PLATFORM.c_str()) == 0)
        {
            TiXmlElement *display = platform->FirstChildElement("display");
            if (display)
            {
                width = atoi(display->Attribute("width"));
                height = atoi(display->Attribute("height"));
                fullscreen = atoi(display->Attribute("fullscreen")) == 1;
                vsync = atoi(display->Attribute("vsync")) == 1;
            }
            break;
        }
    }

    if( !window.Create("Mobitech", width, height, fullscreen))
        return false;
#endif //MOBITECH_WIN32

    OPENGL_CALL(glActiveTexture(GL_TEXTURE0));
    OPENGL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    //OPENGL_CALL(glClearDepth(1.0f));
    OPENGL_CALL(glEnable(GL_DEPTH_TEST));
    OPENGL_CALL(glEnable(GL_CULL_FACE));

#ifdef MOBITECH_WIN32
    SetVerticalSynchronization(vsync);
#endif //MOBITECH_WIN32

    OPENGL_CHECK_FOR_ERRORS();

    float aspectRatio = (float)width / (float)height;
    main_camera.Create(0.0f, 1.0f, 0.0f);
    main_camera.Ortho(0, width, 0, height, 0.0f, 10.0f);// Perspective(45.0f, aspectRatio, 0.001f, 1000.0f);

    SetCurrentCamera(&main_camera);

    PrintDebugInfo();
    return true;
}

void Renderer::Release()
{
    window.Destroy();    
}

void Renderer::PrintDebugInfo()
{
#ifdef MOBITECH_WIN32
    const char *major = (const char *)glGetString(GL_MAJOR_VERSION);
    const char *minor = (const char *)glGetString(GL_MINOR_VERSION);
    const char *mrt = (const char *)glGetString(GL_MINOR_VERSION);
#endif //MOBITECH_WIN32

    char message[1024];

    sprintf_s(message, "\nOpenGL render context information:\n"
        "  Renderer       : %s\n"
        "  Vendor         : %s\n"
        "  Version        : %s\n"
        "  GLSL version   : %s\n",
        //"  OpenGL version : %s.%s\n"
        //"  Max Render Targets: %s\n",
        (const char*)glGetString(GL_RENDERER),
        (const char*)glGetString(GL_VENDOR),
        (const char*)glGetString(GL_VERSION),
        (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)
        //major, minor, mrt
        );

    Logger::Message(message);

    OPENGL_CHECK_FOR_ERRORS();
}