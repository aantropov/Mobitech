#ifndef _RESOURCES_H
#define _RESOURCES_H

#include "math\mathgl.h"
#include <map>
#include <vector>
#include <string>
#include "Utils.hpp"

using namespace std;

struct Vertex
{
    static const int offsetPos = sizeof(vec3) + sizeof(vec3) + sizeof(vec3) + sizeof(vec2);

    vec3 pos;
    vec3 normal;
    vec3 color;
    vec2 texcoord;

    Vertex(): pos(vec3_zero), normal(vec3_zero), texcoord(vec2_zero) {}
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

    int GetId() const { return _id; }
    bool IsInitialized() const { return _id > 0;}
    virtual bool Instantiate() = 0;

    GLObject(void) { _id = -1; }
    virtual ~GLObject(void) { _id = -1; } 
};

class ResourceFactory;
class Resource
{    
public:

    string resourceId;
    ResourceFactory* resourceFactory;

    virtual bool Load(string path) { return true; }
    virtual void Free() {}

    Resource() : resourceId("") {}
    virtual ~Resource() {}
};

class Texture :  public Resource, public GLObject
{
protected:
    unsigned int width, height;
    std::vector<unsigned char> data;

public:

    string name;

    unsigned int GetWidth() const { return width; }
    unsigned int GetHeight() const { return height; }
    const std::vector<unsigned char>& GetData() const { return data; }
    
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
    bool Load(std::string vertexshd_path, std::string pixelshd_path);

    virtual void Free();

    ShaderProgram(const ShaderProgram& s) { this->pixel_sh = s.pixel_sh; this->vertex_sh = s.vertex_sh; this->_id = s._id; };
    ShaderProgram(void);
    virtual ~ShaderProgram(void);
};

class ResourceFactory 
{
protected:

    map<string, Resource*> resources;
    int unique_id;

public:

    ResourceFactory() { unique_id = 0;}
    ~ResourceFactory() { ReleaseAll(); }

    Resource* Get (string path) const;
    bool Add(string path, Resource* res);
    Resource* Load(string path, RESOURCE_TYPE type);
    ShaderProgram* Load(string vp, string pp);
    Resource* Create(RESOURCE_TYPE type);

    void Release(string path);
    void Release(Resource *resource);
    void ReleaseAll();
};

class VertexArrayObject: public GLObject
{
public:

    virtual bool Instantiate();
    void Free();
    VertexArrayObject(void){ _id = -1; }
    virtual ~VertexArrayObject(void) { Free(); }
};

class Buffer: public GLObject
{
    BUFFER_TYPE type;

public:

    virtual void* Lock() const = 0;
    virtual void Unlock() const = 0;

    virtual void* GetPointer() const = 0;
    virtual unsigned int GetNum() = 0;
    virtual void Create(int num) = 0;
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
    virtual ~VertexBuffer(void);
};

#endif //_RESOURCES_H