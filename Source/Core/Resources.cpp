#include "mobitech.h"
#include "resources.h"
#include "renderer.h"
#include "Utils.hpp"
#include "2dgameanimation/Animation.h"

#ifdef MOBITECH_ANDROID
#include <android/asset_manager_jni.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif //MOBITECH_ANDROID

void ReplaceAllSubstrings(std::string& str, const std::string& from, const std::string& to)
{
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

AssetFile::AssetFile(ResourceFactory* rf, const char *file_name)
{
    this->file_name = file_name;
#ifdef MOBITECH_ANDROID
    ReplaceAllSubstrings(this->file_name, "\\", "/");
    this->mgr = rf->asset_manager;
    if(mgr == NULL)
        Logger::Message(LT_ERROR, "AssetManager = NULL");
    file = AAssetManager_open(mgr, this->file_name.c_str(), AASSET_MODE_UNKNOWN);
#else
    file = fopen(this->file_name.c_str(), "rb");
#endif // MOBITECH_ANDROID
       	
	if (file == NULL)
        Logger::Message(LT_ERROR, (string("_ASSET_NOT_FOUND_  ") + string(file_name)).c_str());
}

unsigned int AssetFile::Read(void* buf, int size, int count) const
{
#ifdef MOBITECH_ANDROID
	return AAsset_read(file, buf, size*count);
#else
    return fread(buf, size, count, file);
#endif // MOBITECH_ANDROID
}

void AssetFile::Close()
{
#ifdef MOBITECH_ANDROID
    if(file != NULL)
	    AAsset_close(file);
#else
    if(file != NULL)
        fclose(file);
#endif // MOBITECH_ANDROID
    file = NULL;
}

unsigned int AssetFile::GetFileSize() const
{
#ifdef MOBITECH_ANDROID
	return AAsset_getLength (file);
#else
    struct stat filestatus;
    stat(file_name.c_str(), &filestatus);
    return filestatus.st_size;
#endif // MOBITECH_ANDROID
}

void* AssetFile::GetFile() const
{
	return (void*)(file);
}

Resource* ResourceFactory::Create(const RESOURCE_TYPE type)
{
    char buffer[BUFFER_LENGTH];
    memset(buffer, '\0', BUFFER_LENGTH);
    sprintf_s(buffer, "\\memory\\%d%d\\", (int)type, unique_id++);
    std::string path = buffer;
    return Create(type, path);
}


Resource* ResourceFactory::Create(const RESOURCE_TYPE type, const string path)
{
    Logger::Message("Creating resource: \"" + path + "\"");

    Resource* temp;
    if(type == RT_SHADER)
        temp = new Shader();
    else if(type == RT_TEXTURE)
        temp = new Texture();
    else if(type == RT_SHADER_PROGRAM)
        temp = new ShaderProgram();
    else if(type == RT_ANIMATION)
        temp = new Animation();
    else
        return NULL;

    temp->resource_factory = this;
    temp->resource_id = path;
    resources[path] = temp;
    return temp;  
}

/*
FileData ResourceFactory::GetFileData(const char* relative_path) const
{
    string path(relative_path);
#ifdef MOBITECH_ANDROID    
        
#endif //MOBITECH_ANDROID

    assert(relative_path != NULL);

    if(asset_manager == NULL)
        Logger::Message("Assert manager is not initialized");
    
    AAsset* asset = AAssetManager_open(asset_manager, path.c_str(), AASSET_MODE_UNKNOWN);
    assert(asset != NULL);
 
    return (FileData) { AAsset_getLength(asset), AAsset_getBuffer(asset), asset };
}
 
void ResourceFactory::ReleaseFileData(const FileData* file_data) const
{
    assert(file_data != NULL);
    assert(file_data->file_handle != NULL);
    AAsset_close((AAsset*)file_data->file_handle);
}
*/
Resource* ResourceFactory::Get(const std::string path) const
{
    if(resources.find(path) == resources.end())
        return NULL;
    return resources.at(path);
}

bool ResourceFactory::Add(const std::string path, Resource* resource)
{
    Resource* res = Get(path);
    if(res != NULL)
        return false;
    
    resources[path] = resource;
    return true;
}

ShaderProgram* ResourceFactory::Load(const std::string vp, const std::string pp)
{
    string path = "\\shader_program\\" + vp + "\\" + pp;

    ShaderProgram* res = dynamic_cast<ShaderProgram*>(Get(path));
    if(res != NULL)
        return res;

    Logger::Message("Loading resource: \"" + path + "\"");
    
    ShaderProgram* temp = new ShaderProgram();
    temp->resource_factory = this;
    temp->Load(vp, pp);
    temp->resource_id = path;
    temp->Instantiate();
    resources[path] = temp;
    return temp;
}

Resource* ResourceFactory::Load(const std::string path, const RESOURCE_TYPE type)
{
    Resource* res = Get(path);
    if(res != NULL)
        return res;

    Logger::Message("Loading resource: \"" + path + "\"");
       Resource* temp;
    if(type == RT_SHADER)
        temp = new Shader();
    else if(type == RT_TEXTURE)
        temp = new Texture();
    else if(type == RT_SHADER_PROGRAM)
        temp = new ShaderProgram();
    else if(type == RT_ANIMATION)
        temp = new Animation();    
    else
        return NULL;
    
    temp->resource_factory = this;
    temp->Load(path);
    temp->resource_id = path;
    resources[path] = temp;
    return temp;  
}

void ResourceFactory::Release(const std::string path)
{
    delete resources[path];
    resources.erase(path);
}    

void ResourceFactory::Release(Resource *resource)
{
    for (std::map<string, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
        if (it->second == resource)
        {
            delete resource;
            resources.erase(it->first);
        }
}    

void ResourceFactory::ReleaseAll()
{
    for (std::map<string, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
        delete it->second;
    resources.clear();
}

void* VertexBuffer::GetPointer() const 
{ 
    return (void*)vertices; 
}

unsigned int VertexBuffer::GetNum() const 
{
    return num_vertices; 
}

void VertexBuffer::Create(int num_vertices) 
{ 
    this->num_vertices = num_vertices; 
    vertices = new Vertex[num_vertices](); 
}

VertexArrayObject* VertexBuffer::GetVAO() const 
{ 
    return vao; 
}

bool VertexBuffer::Instantiate()
{
    vao = new VertexArrayObject();
    vao->Instantiate();
    
    Renderer::GetInstance()->BindVAO(this);

    _id = -1;
    _id = Renderer::GetInstance()->CreateVBO(this, STATIC);

    return (_id != -1) && (vao->GetId() != -1);
}

void VertexBuffer::Free()
{
    if(_id != -1)
    Renderer::GetInstance()->DeleteVBO(this);
    _id = -1;

    delete[] (Vertex*)vertices;
    delete vao;
}

void* VertexBuffer::Lock() const
{
#ifdef MOBITECH_WIN32
    glBindBuffer(GL_ARRAY_BUFFER, GLObject::_id);
    glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Vertex), 0, GL_STREAM_DRAW_ARB);
    Vertex* pBuffer = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY_ARB);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return pBuffer;
#endif //MOBITECH_WIN32
    return NULL;
}

void VertexBuffer::Unlock() const
{
#ifdef MOBITECH_WIN32
    glBindBuffer(GL_ARRAY_BUFFER, GLObject::_id);
    GLboolean result = glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif //MOBITECH_WIN32
}

bool VertexArrayObject::Instantiate()
{
    _id = -1;    
    _id = Renderer::GetInstance()->CreateVAO();
    return (_id != -1);
}

void VertexArrayObject::Free()
{
    if(_id != -1)
        Renderer::GetInstance()->DeleteVAO(this);
    _id = -1;
}

bool Shader::Instantiate()
{
    _id = Renderer::GetInstance()->CompileShader(source, type);
    return _id > -1;
}

bool Shader::Load(const std::string path) 
{
    AssetFile file(resource_factory, path.c_str());
    char temp = '\0';
    while(file.Read(&temp, 1, 1) != 0)
        source.push_back(temp);
    return true;
}

void Shader::Free()
{
    source.clear();
    if(_id != -1)
        Renderer::GetInstance()->DeleteShader(this);
    _id = -1;
}

void ShaderProgram::Free()
{    
    if(_id != -1)
        Renderer::GetInstance()->DeleteShaderProgram(this);
    _id = -1;
}

void ShaderProgram::InitLocations()
{
    uniform_locations.transform_model = glGetUniformLocation(_id, "transform.model");
    uniform_locations.transform_viewProjection = glGetUniformLocation(_id, "transform.viewProjection");
    uniform_locations.transform_normal = glGetUniformLocation(_id, "transform.normal");
    uniform_locations.transform_modelViewProjection = glGetUniformLocation(_id, "transform.modelViewProjection");
    uniform_locations.transform_viewPosition = glGetUniformLocation(_id, "transform.viewPosition");
    
    attribute_locations.color = glGetAttribLocation(_id, "color");
    attribute_locations.position = glGetAttribLocation(_id, "position");
    attribute_locations.texcoords = glGetAttribLocation(_id, "texcoords");
}

bool ShaderProgram::Load(const string path)
{
    //not implemented yet
    return false;
}

bool ShaderProgram::Instantiate()
{
    _id = Renderer::GetInstance()->CreateShaderProgram(vertex_sh, pixel_sh);
    InitLocations();
    return _id > -1;
}

bool ShaderProgram::Load(const std::string vertexshd_path, const std::string pixelshd_path)
{
    Shader *vs = dynamic_cast<Shader*>(resource_factory->Get(vertexshd_path));
    Shader *ps = dynamic_cast<Shader*>(resource_factory->Get(pixelshd_path));

    vertex_sh = dynamic_cast<Shader*> (resource_factory->Load(vertexshd_path, RT_SHADER));
    pixel_sh = dynamic_cast<Shader*> (resource_factory->Load(pixelshd_path, RT_SHADER));
    
    if(vs == NULL)
    {
        vertex_sh->type = ST_VERTEX;
        vertex_sh->Instantiate();
    }
    
    if(ps == NULL)
    {
        pixel_sh->type = ST_FRAGMENT;
        pixel_sh->Instantiate();
    } 
    
    return Instantiate();
}

ShaderProgram::ShaderProgram(void)
{
}

ShaderProgram::~ShaderProgram(void)
{
    Free();
}

Texture::Texture(void)
{
    data = NULL;
}

Texture::~Texture(void)
{
    Free();
}

bool Texture::Instantiate()
{
    if(_id != -1)
        Renderer::GetInstance()->DeleteTexture(this);
    glGenTextures(1, (GLuint*)&_id);
    return Renderer::GetInstance()->CreateTexture(this);
}

bool Texture::Load(const std::string path)
{   
    Free();
    AssetFile png_file(resource_factory, path.c_str());
    
    unsigned int size = png_file.GetFileSize();
    unsigned char *png_buffer = new unsigned char[size];
    
    data = new unsigned char[size];
    png_file.Read(png_buffer, 1, size);

    unsigned error = lodepng_decode_memory(&data, &width, &height, png_buffer, size, LCT_RGBA, 8);

    if(png_buffer != NULL)
        delete[] png_buffer;

    if(error)
    {
        Logger::Message(LT_ERROR, "LodePng decode error");
        return false;
    }
    
    bool res = Instantiate();
    
    if(data != NULL)
    {        
        delete[] data;
        data = NULL;
    }
    return res;
}

void Texture::Free()
{
    if(data != NULL)
    {
        delete[] data;
        data = NULL;
    }

    if(_id != -1)
        Renderer::GetInstance()->DeleteTexture(this);
}
