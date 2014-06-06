#ifndef _RESOURCES_H
#define _RESOURCES_H

#include "math\mathgl.h"
#include <map>
#include <vector>
#include <string>
#include "Utils.hpp"
#include "Renderer.h"

#ifdef MOBITECH_ANDROID
#include <android/asset_manager_jni.h>
#endif //MOBITECH_ANDROID

using namespace std;

class ResourceFactory;
class AssetFile
{
private:
	string file_name;

#ifdef MOBITECH_ANDROID
	AAsset* file;
	AAssetManager *mgr;
#else
	FILE * file;
#endif
	
public:
    AssetFile(ResourceFactory* rf, const char *file_name);
    ~AssetFile() { Close(); }
    
    unsigned int Read(void* buf, int size, int count) const;
    void Close();
    unsigned int GetFileSize() const;
    void* GetFile() const;
};

class ResourceFactory;
class Resource
{    
public:

    string resource_id;
    ResourceFactory* resource_factory;

    virtual bool Load(const string path) { return true; }
    virtual void Free() {}

    Resource() : resource_id("") {}
    virtual ~Resource() {}
};

class Texture :  public Resource, public GLObject
{
protected:
    unsigned int width, height;
    unsigned char *data;

public:

    string name;
    
    unsigned int GetWidth() const { return width; }
    unsigned int GetHeight() const { return height; }
    const unsigned char* GetData() const { return data; }
    
    void Initialize(const unsigned int w, const unsigned int h) { width = w; height = h; }
    virtual bool Instantiate();
    virtual void Free();
    virtual bool Load(const string path);

    Texture(void);
    virtual ~Texture(void);
};

class Shader : public Resource, public GLObject
{
    string source;
public:
        
    SHADER_TYPE type;

    string GetSource() const { return source; }
    virtual bool Instantiate();
    virtual bool Load(const string path);
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
        unsigned int time;
    };

    struct AttributeLocations
    {
        unsigned int position;
        unsigned int texcoords;
        unsigned int color;
    };

    UniformLocations uniform_locations;
    AttributeLocations attribute_locations;

    void InitLocations();

    virtual bool Instantiate();
    virtual bool Load(const string path);
    bool Load(const std::string vertexshd_path, const std::string pixelshd_path);

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
    
#ifdef MOBITECH_ANDROID
    AAssetManager* asset_manager;
    AAssetManager* GetAssetManagerArchive() const { return asset_manager; }
    //zip* apk_archive;
    //zip* GetApkArchive() const { return apk_archive; }
#endif// MOBITECH_ANDROID      

    ResourceFactory() 
    { 
        unique_id = 0; 
#ifdef MOBITECH_ANDROID 
        asset_manager = NULL; 
#endif// MOBITECH_ANDROID
    }

    ~ResourceFactory() { ReleaseAll(); }

    Resource* Get (const string path) const;
    bool Add(const string path, Resource* res);
    Resource* Load(const string path, const RESOURCE_TYPE type);
    ShaderProgram* Load(const string vp, const string pp);
    Resource* Create(const RESOURCE_TYPE type);
    Resource* Create(const RESOURCE_TYPE type, const string path);

    void Release(const string path);
    void Release(Resource *resource);
    void ReleaseAll();
};

#endif //_RESOURCES_H