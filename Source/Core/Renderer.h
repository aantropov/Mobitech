#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Utils.hpp"
#include "Resources.h"
#include "math\mathgl.h"
//#include <hash_map>
#include <map>

using namespace std;

class Camera
{
    vec3 position;
    vec3 rotation;
    mat4 projection;

    frustum frustum_planes;

public:

    void UpdateFrustum();

    vec3 GetPosition() const;
    vec3 GetRotation()  const;
    const frustum GetFrustum() const { return frustum_planes; }

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
    HINSTANCE hinstance;
    HGLRC     hrc;
    
    static HWND      hwnd;
    static HDC       hdc;
#endif //MOBITECH_WIN32    

    static const string WND_CLASS_NAME;
    
    int width;
    int height;

    // States
    static bool fullscreen;    
    static bool active;
    static bool running;

public:
    
    Window();
    ~Window();
    
    static bool IsActive() { return active; }
    static bool IsRunning() { return running; }
    static bool IsFullscreen() { return fullscreen; }
    
    static void SetActive(bool value){ active = value; }
    static void SetRunning(bool value ){ running = value; }
    static void SetFullscreen(bool value ){ fullscreen = value; }
    
    static void SetWindowTitle(string title)
    { 
#ifdef MOBITECH_WIN32 
        SetWindowText(hwnd, title.c_str()); 
#endif //MOBITECH_WIN32
    }

#ifdef MOBITECH_WIN32
    static HWND GetHWND() { return hwnd; }
    static HDC GetHDC() { return hdc; }
#endif //MOBITECH_WIN32

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

    bool Create(string title = "Mobitech", int width = 800, int height = 600, bool is_fullScreen = false);
    
    void Destroy();
    void SetSize(int width, int height, bool is_fullScreen = false);

    friend class Renderer;
};

class Renderer: public Singleton<Renderer>
{
    Window window;
    
    bool SetVerticalSynchronization(bool enabled);
    void PrintDebugInfo();
    
    std::vector<mat4> modelview_matrix_stack;

    // Optimization
    ShaderProgram *shader_program;
    unsigned int previous_vao;
    unsigned int previous_ib;
    
    map<unsigned int, map<string, unsigned int> > uniforms_cache;
    map<unsigned int, unsigned int> tex_channels_cache;

    int CacheUniformLocation(string name);
    int CacheUniformLocation(string name, ShaderProgram *sh);
           
public:
    
	unsigned int draw_calls;

    static Renderer* GetInstance();

    Renderer();
    ~Renderer();

    Camera current_camera;
    Camera mainCamera;
    
    bool Initialize();

#ifdef MOBITECH_WIN32
    HWND GetHWND() const { return window.GetHWND(); }
#endif //MOBITECH_WIN32

    int GetWidth() const { return window.GetWidth(); }
    int GetHeight() const { return window.GetHeight(); }

    void SetWidth(int w) { window.width = w; }
    void SetHeight(int h) { window.height = h; }
      
    void Release();

    Camera GetCurrentCamera();
    void SetCurrentCamera(Camera cam);
    void SetupCameraForShaderProgram(ShaderProgram *shd, mat4 &model);
    void SetupCameraForShaderProgram(Camera *cam, ShaderProgram *shd, const mat4 &model);
    void SetupCameraLightForShaderProgram(Camera &camera);

    int CreateTexture(Texture *tex) const;
    void BindTexture(Texture *tex);
    void DeleteTexture(Texture *tex) const;
    void BindTexture(Texture *tex, unsigned int channel);

    void BindBuffer(VertexBuffer *vb) const;
    void BindBuffer(IndexBuffer *ib);
    void UnbindBuffer(bool is_vertex_buffer) const;
    
    int CreateVBO(VertexBuffer *vb, BUFFER_TYPE state) const;
    int CreateVBO(IndexBuffer *ib, BUFFER_TYPE state) const;    
    void DeleteVBO(Buffer *vb) const;

    void BindVAO(VertexBuffer *vb);
    void UnbindVAO() const;    
    int CreateVAO() const;
    void DeleteVAO(VertexArrayObject *vao) const;

    int CompileShader(string source, SHADER_TYPE st) const;
    void DeleteShader(Shader* shd) const;

    int CreateShaderProgram(Shader *vertex_sh, Shader *pixel_sh) const;
    void SetShaderProgram(ShaderProgram *sh);
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
    void DrawSolidPolygon(const Vertex* vertices, int vertex_count, const vec4 color) const;
        
    void DrawBuffer(VertexBuffer *vb);
    void DrawBuffer(IndexBuffer* ib);
};


#endif //_RENDERER_H_
