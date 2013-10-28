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
    
    static bool IsActive() const { return active; }
    static bool IsRunning() const { return running; }
    static bool IsFullscreen() const { return fullscreen; }
    
    static void SetActive(bool value){ active = value; }
    static void SetRunning(bool value ){ running = value; }
    static void SetFullscreen(bool value ){ fullscreen = value; }
    
    static void SetWindowTitle(string title);

#ifdef MOBITECH_WIN32
    static HWND GetHWND() const { return g_hWnd; }
    static HDC GetHDC() const { return g_hDC; }
#endif //MOBITECH_WIN32

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

    bool Create(string title = "Mobitech", int width = 800, int height = 600, bool isFullScreen = false);
    
    void Destroy();
    void SetSize(int width, int height, bool isFullScreen = false);
};

class Renderer: public Singleton<Renderer>
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

    static Renderer* GetInstance() const;

    Renderer();
    ~Renderer();

    Camera currentCamera;
    Camera mainCamera;
    
    bool Initialize();

#ifdef MOBITECH_WIN32
    HWND GetHWND() const { return uWnd.GetHWND(); }
#endif //MOBITECH_WIN32

    int GetWidth() const { return uWnd.GetWidth(); }
    int GetHeight() const { return uWnd.GetHeight(); }
        
    void Release();

    void PushModelMatrix();
    void PopModelMatrix();

    Camera GetCurrentCamera();
    void SetCurrentCamera(Camera cam);
    void SetupCameraForShaderProgram(ShaderProgram *shd, mat4 &model);
    void SetupCameraForShaderProgram(Camera *cam, ShaderProgram *shd, const mat4 &model);
    void SetupCameraLightForShaderProgram(Camera &camera);

    int CreateTexture(Texture *tex) const;
    void BindTexture(Texture *tex) const;
    void DeleteTexture(Texture *tex) const;
    void BindTexture(Texture *tex, unsigned int channel) const;

    void BindBuffer(VertexBuffer *vb) const;
    void BindBuffer(IndexBuffer *ib) const;
    void UnbindBuffer(bool is_vertex_buffer) const;
    
    int CreateVBO(VertexBuffer *vb, BUFFER_TYPE state) const;
    int CreateVBO(IndexBuffer *ib, BUFFER_TYPE state) const;    
    void DeleteVBO(Buffer *vb) const;

    void BindVAO(VertexBuffer *vb) const;
    void UnbindVAO() const;
    
    int CreateVAO() const;
    void DeleteVAO(VertexArrayObject *vao) const;

    int CompileShader(string source, SHADER_TYPE st) const;
    void DeleteShader(Shader* shd) const;

    int CreateShaderProgram(Shader *vertex_sh, Shader *pixel_sh) const;
    void SetShaderProgram(ShaderProgram *sh) const;
    void DeleteShaderProgram(ShaderProgram *sh) const;

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

    void Uniform4(unsigned int location, unsigned int num , float *variable) const;
    void Uniform1(unsigned int location, unsigned int num , float *variable) const;
    void Uniform1(unsigned int location, int value) const;
    void Uniform3(unsigned int location, unsigned int num , float *variable) const;
    void UniformMatrix4(unsigned int location, unsigned int num , float *variable) const;
    void UniformMatrix3(unsigned int location, unsigned int num , float *variable) const;

    void DrawSegment(const vec3& p1, const vec3& p2, const vec3& color) const;
    void DrawTransform(::transform xf) const;
    void DrawSolidPolygon(const Vertex* vertices, int vertexCount, const vec4 color) const;
        
    void DrawBuffer(VertexBuffer *vb) const;
    void DrawBuffer(IndexBuffer* ib) const;
};


#endif //_RENDERER_H_
