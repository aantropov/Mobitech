#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Resources.h"
#include "math\mathgl.h"
//#include <hash_map>
#include <map>
#include "Utils.hpp"

using namespace std;

class VertexArrayObject: public GLObject
{
public:

    virtual bool Instantiate();
    void Free();
    VertexArrayObject(void){ _id = -1; }
    virtual ~VertexArrayObject(void) { Free(); }
};

class IndexBuffer : public Buffer
{
    unsigned int num_indices;  
    unsigned int *indices;

public:

    virtual void* GetPointer() const { return (void*)indices; }
    virtual unsigned int GetNum() const { return num_indices; }

    virtual void Create(int num_faces);
    virtual bool Instantiate();
    virtual void Free();

    void* Lock() const;
    void Unlock() const;

    IndexBuffer(void);
    virtual ~IndexBuffer(void);
};

class VertexBuffer : public Buffer
{    
    int num_vertices;
    void *vertices;    
    VertexArrayObject* vao;

public:

    virtual void* GetPointer() const;
    virtual unsigned int GetNum() const;
    virtual void Create(int num_vertices);
    VertexArrayObject* GetVAO() const;
    virtual bool Instantiate();
    virtual void Free();
    void* Lock() const;
    void Unlock() const;

    VertexBuffer(void);
    virtual ~VertexBuffer(void) {}
};

class RenderBuffer : public GLObject
{
public:
    virtual bool Instantiate();
    void Free();
    RenderBuffer(void);
    virtual ~RenderBuffer(void);
};

class Texture;
class FrameBufferObject : public GLObject
{   
public:
    virtual bool Instantiate();
    void Free();

    void BindTexture(const Texture *tex, FRAMEBUFFER_ATTACHMENT type);

    FrameBufferObject(void);
    virtual ~FrameBufferObject(void);
};

class Camera
{
protected:
    vec3 position;
    vec3 rotation;
    vec3 scale;
    mat4 projection;

    frustum frustum_planes;

public:

    void UpdateFrustum();

    vec3 GetPosition() const;
    vec3 GetRotation()  const;
    vec3 GetScale()  const;
    const frustum GetFrustum() const { return frustum_planes; }

    void SetPosition(vec3 p) { position = p; }
    void SetRotation(vec3 r) { rotation = r; }
    void SetScale(vec3 s) { scale = s; }
    
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

class Texture;
class RenderTexture
{
protected:
    RenderBuffer rb;
    FrameBufferObject fbo;
    Texture *res;
    unsigned int width;
    unsigned int height;

public:
    Texture* GetTexture() const { return res; }
    bool Initialize(unsigned int width, unsigned int height);
    void Begin() const;
    void End() const;
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

class ShaderProgram;
class Texture;
class Shader;
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

    int CacheUniformLocation(const string name);
    int CacheUniformLocation(const string name, const ShaderProgram *sh);
           
public:
    
	unsigned int draw_calls;

    static Renderer* GetInstance();

    Renderer();
    ~Renderer();

    Camera *current_camera;
    Camera main_camera;
    
    bool Initialize();

#ifdef MOBITECH_WIN32
    HWND GetHWND() const { return window.GetHWND(); }
#endif //MOBITECH_WIN32

    ShaderProgram* GetCurrentShaderProgram() const { return shader_program; }

    int GetWidth() const { return window.GetWidth(); }
    int GetHeight() const { return window.GetHeight(); }

    void SetWidth(int w) { window.width = w; }
    void SetHeight(int h) { window.height = h; }
      
    void Release();

    Camera *GetCurrentCamera() const;
    void SetCurrentCamera(Camera *cam);
    
    void SetupCameraForShaderProgram(const ShaderProgram *shd, const mat4 model);
    void SetupCameraCameraForShaderProgram(const ShaderProgram *shd, const mat4 model);
    void SetupCameraForShaderProgram(Camera *cam, const ShaderProgram *shd, const mat4 model);   

    void ClearColor(vec4 color) const;

    int CreateTexture(const Texture *tex) const;
    void BindTexture(const Texture *tex);
    void DeleteTexture(const Texture *tex) const;
    void BindTexture(const Texture *tex, unsigned int channel);

    void BindBuffer(const VertexBuffer *vb) const;
    void BindBuffer(const IndexBuffer *ib);
    void UnbindBuffer(bool is_vertex_buffer) const;
    
    int CreateVBO(const VertexBuffer *vb, BUFFER_TYPE state) const;
    int CreateVBO(const IndexBuffer *ib, BUFFER_TYPE state) const;    
    void DeleteVBO(const Buffer *vb) const;

    void BindVAO(VertexBuffer *vb);
    void UnbindVAO() const;    
    int CreateVAO() const;
    void DeleteVAO(VertexArrayObject *vao) const;

    int CompileShader(const string source, SHADER_TYPE st) const;
    void DeleteShader(const Shader* shd) const;

    int CreateShaderProgram(const Shader *vertex_sh, const Shader *pixel_sh) const;
    void BindShaderProgram(ShaderProgram *sh);
    void DeleteShaderProgram(const ShaderProgram *sh) const;

    int CreateFBO() const;
    void DeleteFBO(const FrameBufferObject *fb) const;
    void BindFBO(const FrameBufferObject *fb) const;
    void UnbindFBO() const;

    int CreateRB() const;
    void DeleteRB(const RenderBuffer *rb) const;
    void BindRB(const RenderBuffer *rb) const;
    void UnbindRB() const;

    void CacheUniform4(const ShaderProgram *sh, const std::string name, unsigned int num , float *variable);
    void CacheUniform4(const std::string name, unsigned int num , float *variable);
    
    void CacheUniform1(const ShaderProgram *sh, const std::string name, unsigned int num , float *variable);
    void CacheUniform1(const std::string name, unsigned int num , float *variable);
    void CacheUniform1(const std::string name, int value);

    void CacheUniform3(const ShaderProgram *sh, const std::string name, unsigned int num , float *variable);
    void CacheUniform3(const std::string name, unsigned int num , float *variable);

    void CacheUniformMatrix4(const std::string name, unsigned int num , float *variable);
    void CacheUniformMatrix3(const std::string name, unsigned int num , float *variable);

    void CacheUniformMatrix4(const ShaderProgram *sh, const std::string name, unsigned int num , float *variable);
    void CacheUniformMatrix3(const ShaderProgram *sh, const std::string name, unsigned int num , float *variable);

    void Uniform4(unsigned int location, unsigned int num , float *variable) const;
    void Uniform1(unsigned int location, unsigned int num , float *variable) const;
    void Uniform1(unsigned int location, int value) const;
    void Uniform3(unsigned int location, unsigned int num , const float *variable) const;
    void UniformMatrix4(unsigned int location, unsigned int num , float *variable) const;
    void UniformMatrix3(unsigned int location, unsigned int num , float *variable) const;

    void DrawSegment(const vec3& p1, const vec3& p2, const vec3& color) const;
    void DrawTransform(::transform xf) const;
    void DrawSolidPolygon(const Vertex* vertices, int vertex_count, const vec4 color) const;
    void DrawArrays(int type, int a, int size);
    void DrawBuffer(const VertexBuffer *vb);
    void DrawBuffer(const IndexBuffer* ib);
    void DrawTriangles(void* vertices, void* colors, void* texcoords, unsigned int count);

    void EnableBlend(BLEND_TYPE type);
    void DisableBlend();
};

#endif //_RENDERER_H_
