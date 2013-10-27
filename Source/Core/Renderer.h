#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Utils.hpp"
#include "Resources.h"
#include "math\mathgl.h"
#include <hash_map>
#include <map>

using namespace std;

class Camera
{
    vec3 position;
    vec3 rotation;
    mat4 projection;

    frustum frustumPlanes;

public:

    void UpdateFrustum();

    vec3 GetPosition() const;
    vec3 GetRotation()  const;
    const frustum GetFrustum() const { return frustumPlanes; }

    void SetPosition(vec3 p) { position = p; }
    void SetRotation(vec3 r) { rotation = r; }
    
    mat4 GetProjection()  const;
    mat4 GetView()  const;

    void Create(float x, float y, float z);
    void Perspective(float fov, float aspect, float zNear, float zFar);
    void Ortho(float left, float right, float bottom, float top, float zNear, float zFar);
    void LookAt(const vec3 &position, const vec3 &center, const vec3 &up);        
    void Rotate(float x, float y, float z);
    void Move( float x, float y, float z);
        
    Camera(void);
    ~Camera(void);
};

class Window
{

#ifdef MOBITECH_WIN32
    HINSTANCE g_hInstance;
    HGLRC     g_hRC;
    
    static HWND      g_hWnd;
    static HDC       g_hDC;
#endif //MOBITECH_WIN32    

    static const string UWND_CLASS_NAME;
    
    int width;
    int height;

    // States
    static bool fullscreen;    
    static bool active;
    static bool running;

public:
    
    Window();
    ~Window();
    
    static bool IsActive(){ return active; }
    static bool IsRunning(){ return running; }
    static bool IsFullscreen(){ return fullscreen; }
    
    static void SetActive( bool value){ active = value; }
    static void SetRunning( bool value ){ running = value; }
    static void SetFullscreen( bool value ){ fullscreen = value; }
    
    static void SetWindowTitle(string title);

#ifdef MOBITECH_WIN32
    static HWND GetHWND(){ return g_hWnd; }
    static HDC GetHDC(){ return g_hDC; }
#endif //MOBITECH_WIN32

    int GetWidth(){ return width; }
    int GetHeight(){ return height; }

    bool Create(string title = "Mobitech", int width = 800, int height = 600, bool isFullScreen = false);
    
    void Destroy();
    void SetSize(int width, int height, bool isFullScreen = false);
};

class URenderer: public Singleton<URenderer>
{
    Window uWnd;

    bool InitExtensions();
    bool SetVerticalSynchronization(bool enabled);

    void PrintDebugInfo();
    
    std::vector<mat4> modelViewMatrixStack;

    // Optimization
    unsigned int previousVAO;
    unsigned int previousIB;
    
    map<unsigned int, hash_map<string, unsigned int>> uniformsCache;
    map<unsigned int, unsigned int> texChannelsCache;

    int CacheUniformLocation(string name);
    int CacheUniformLocation(string name, ShaderProgram *sh);
           
public:
    
	unsigned int drawCalls;

    static URenderer* GetInstance();

    URenderer();
    ~URenderer();

    Camera currentCamera;
    Camera mainCamera;
    
    bool Initialize();

#ifdef MOBITECH_WIN32
    HWND GetHWND() { return uWnd.GetHWND(); }
#endif //MOBITECH_WIN32

    int GetWidth(){ return uWnd.GetWidth(); }
    int GetHeight(){ return uWnd.GetHeight(); }
        
    void Release();

    void PushModelMatrix();
    void PopModelMatrix();

    Camera GetCurrentCamera();
    void SetCurrentCamera(Camera cam);
    void SetupCameraForShaderProgram(ShaderProgram *shd, mat4 &model);
    void SetupCameraForShaderProgram(Camera *cam, ShaderProgram *shd, const mat4 &model);
    void SetupCameraLightForShaderProgram(Camera &camera);

    int CreateTexture(Texture *tex);
    void BindTexture(Texture *tex);
    void DeleteTexture(Texture *tex);
    void BindTexture(Texture *tex, unsigned int channel);

    void BindBuffer(VertexBuffer *vb);
    void BindBuffer(IndexBuffer *ib);
    void UnbindBuffer(bool is_vertex_buffer);
    
    int CreateVBO(VertexBuffer *vb, BUFFER_TYPE state);
    int CreateVBO(IndexBuffer *ib, BUFFER_TYPE state);    
    void DeleteVBO(Buffer *vb);

    void BindVAO(VertexBuffer *vb);
    void UnbindVAO();
    
    int CreateVAO();
    void DeleteVAO(VertexArrayObject *vao);

    int CompileShader(string source, SHADER_TYPE st);
    void DeleteShader(Shader* shd);

    int CreateShaderProgram(Shader *vertex_sh, Shader *pixel_sh);
    void SetShaderProgram(ShaderProgram *sh);
    void DeleteShaderProgram(ShaderProgram *sh);

    void CacheUniform4(ShaderProgram *sh, std::string name, unsigned int num , float *variable);
    void CacheUniform4(std::string name, unsigned int num , float *variable);
    
    void CacheUniform1(ShaderProgram *sh, std::string name, unsigned int num , float *variable);
    void CacheUniform1(std::string name, unsigned int num , float *variable);
    void CacheUniform1(std::string name, int value);

    void CacheUniform3(ShaderProgram *sh, std::string name, unsigned int num , float *variable);
    void CacheUniform3(std::string name, unsigned int num , float *variable);

    void CacheUniformMatrix4(std::string name, unsigned int num , float *variable);
    void CacheUniformMatrix3(std::string name, unsigned int num , float *variable);

    void CacheUniformMatrix4(ShaderProgram *sh, std::string name, unsigned int num , float *variable);
    void CacheUniformMatrix3(ShaderProgram *sh, std::string name, unsigned int num , float *variable);

    void Uniform4(unsigned int location, unsigned int num , float *variable);
    void Uniform1(unsigned int location, unsigned int num , float *variable);
    void Uniform1(unsigned int location, int value);
    void Uniform3(unsigned int location, unsigned int num , float *variable);
    void UniformMatrix4(unsigned int location, unsigned int num , float *variable);
    void UniformMatrix3(unsigned int location, unsigned int num , float *variable);

    void DrawSegment(const vec3& p1, const vec3& p2, const vec3& color);
    void DrawTransform(::transform xf);
    void DrawSolidPolygon(const Vertex* vertices, int vertexCount, const vec4 color);
        
    void DrawBuffer(VertexBuffer *vb);
    void DrawBuffer(IndexBuffer* ib);
};


#endif //_RENDERER_H_
