#ifndef _RESOURCES_H
#define _RESOURCES_H

#include "math\mathgl.h"
#include <map>
#include <string>

using namespace std;

enum RESOURCE_TYPE
{
    RT_TEXTURE,
    RT_SHADER,
    RT_SHADER_PROGRAM,
    RT_AUDIO
};

enum SHADER_TYPE
{
    ST_VERTEX,
    ST_FRAGMENT
};

enum BUFFER_TYPE
{
    STATIC,
    DYNAMIC
};

struct Vertex
{
    static const int offsetPos = sizeof(vec3) + sizeof(vec3) + sizeof(vec3) + sizeof(vec2);

    vec3 pos;
    vec3 normal;
    vec3 color;
    vec2 texcoord;

    Vertex(vec3 p): pos(p), normal(vec3_zero), texcoord(vec2_zero) {}
    Vertex(vec3 p, vec3 n): pos(p), normal(n), texcoord(vec2_zero) {}
    Vertex(vec3 p, vec3 n, vec2 tc): pos(p), normal(n), texcoord(tc) {}
    Vertex(vec3 p, vec3 n, vec3 c, vec2 tc): pos(p), normal(n), texcoord(tc), color(c) {}
};

class GLObject
{
protected:
    int _id;

public:

    int GetId() { return _id; }
    bool IsInitialized(){ return _id > 0;}
    virtual bool Instantiate();

    GLObject(void) {_id = -1; }
    virtual ~GLObject(void) { _id = -1; } 
};

class Resource
{    
public:

    string resourceId;

    virtual bool Load(string path){ return true; }
    virtual void Free() {}

    Resource() : resourceId("") {}
    virtual ~Resource();
};

class Texture :  public Resource, public GLObject
{
protected:
    int width, height;

public:

    string name;

    int Width() {return width;}
    int Height() {return height;}

    virtual bool Instantiate();
    virtual void Free();
    virtual bool Load(string path);

    Texture(void);
    virtual ~Texture(void);
};

class Shader : public Resource, public GLObject
{
public:

    string source;
    SHADER_TYPE type;

    virtual bool Instantiate();
    virtual bool Load(string path);
    void Free();
    Shader(void){}
    virtual ~Shader(void) { Free(); }
};

class ShaderProgram : public GLObject, public Resource
{    
    Shader *pixel_sh;
    Shader *vertex_sh;

public:

    struct UniformLocations
    {
        unsigned int transform_model;
        unsigned int transform_viewProjection;
        unsigned int transform_normal;
        unsigned int transform_modelViewProjection;
        unsigned int transform_viewPosition;
    };

    UniformLocations locations;

    void InitLocations();

    virtual bool Instantiate();
    virtual bool Load(string path);
    bool Load(Shader* vertex, Shader* fragment);

    virtual void Free();

    ShaderProgram(const ShaderProgram& s) {this->pixel_sh = s.pixel_sh; this->vertex_sh = s.vertex_sh; this->_id = s._id; };
    ShaderProgram(void);
    virtual ~ShaderProgram(void);
};

class ResourceFactory 
{
protected:

    map<string, Resource*> resources;
    int unique_id;

public:

    ResourceFactory(){ unique_id = 0;}
    ~ResourceFactory(){ ReleaseAll(); }

    Resource* Get(string path);
    bool Add(string path, Resource* res);
    Resource *Load(string path, RESOURCE_TYPE type);
    ShaderProgram *Load(string vp, string pp);
    Resource *Create(RESOURCE_TYPE type);

    void Release(string path);    
    void Release(Resource *resource);
    void ReleaseAll();
};

class VertexArrayObject: public GLObject
{
public:
    
    bool Initialize();
    void Free();
    VertexArrayObject(void){ _id = -1; };
    virtual ~VertexArrayObject(void){Free();};
};

class Buffer: public GLObject
{
    BUFFER_TYPE type;

public:

    virtual void* Lock() = 0;
    virtual void Unlock() = 0;

    virtual void* GetPointer() = 0;
    virtual int GetNum() = 0;
    virtual void Create(int num) = 0;
    virtual bool Initialize() = 0;
    virtual bool Instantiate() = 0;
    virtual void Free() = 0;

    Buffer(void);
    virtual ~Buffer(void);
};


class IndexBuffer : public Buffer
{
    unsigned int num_indices;  
    unsigned int *indices;

public:

    void* GetPointer() {return (void*)indices;}
    int GetNum() { return num_indices; }

    void Create(int num_faces);
    bool Initialize();
    void Free();
    void* Lock();
    void Unlock();

    IndexBuffer(void);
    virtual ~IndexBuffer(void);
};


class VertexBuffer : public Buffer
{    
    int num_vertices;
    void *vertices;    
    VertexArrayObject* vao;
    
public:

	void* GetPointer();
    int GetNum();
    void Create(int num_vertices);
    VertexArrayObject* GetVAO();
    
    bool Initialize(IndexBuffer* ib);
    bool Instantiate();
    void Free();
    void* Lock();
    void Unlock();

    VertexBuffer(void);
    virtual ~VertexBuffer(void);
};

#endif //_RESOURCES_H