#include "mobitech.h"
#include "resources.h"
#include "renderer.h"
#include "libzip\zip.h"

#ifdef MOBITECH_ANDROID
#include <android/asset_manager_jni.h>
#endif //MOBITECH_ANDROID

Resource* ResourceFactory:: Create(RESOURCE_TYPE type)
{
    char buffer[BUFFER_LENGTH];
    memset(buffer, '\0', BUFFER_LENGTH);
    sprintf_s(buffer, "\\memory\\%d%d\\", (int)type, unique_id++);
    std::string path = buffer;

    Logger::Message("Creating resource: \"" + path + "\"");

    Resource* temp;
    if(type == RT_SHADER)
        temp = new Shader();
    else if(type == RT_TEXTURE)
        temp = new Texture();
    else if(type == RT_SHADER_PROGRAM)
        temp = new ShaderProgram();
    else
        return NULL;

    temp->resource_factory = this;
    temp->resourceId = path;
    resources[path] = temp;
    return temp;  
}

FileData ResourceFactory:: GetFileData(const char* relative_path) const
{
    assert(relative_path != NULL);

    if(asset_manager == NULL)
        Logger::Message("Assert manager is not initialized");
    
    AAsset* asset = AAssetManager_open(asset_manager, relative_path, AASSET_MODE_UNKNOWN);
    Logger::Message("Loading ok");
    assert(asset != NULL);
 
    return (FileData) { AAsset_getLength(asset), AAsset_getBuffer(asset), asset };
}
 
void ResourceFactory:: ReleaseFileData(const FileData* file_data) const
{
    assert(file_data != NULL);
    assert(file_data->file_handle != NULL);
    AAsset_close((AAsset*)file_data->file_handle);
}

Resource* ResourceFactory:: Get(std::string path) const
{
    if(resources.find(path) == resources.end())
        return NULL;
    return resources.at(path);
}

bool ResourceFactory:: Add(std::string path, Resource* resource)
{
    Resource* res = Get(path);
    if(res != NULL)
        return false;
    
    resources[path] = resource;
    return true;
}

ShaderProgram* ResourceFactory:: Load(std::string vp, std::string pp)
{
    string path = "\\shader_program\\" + vp + "\\" + pp;

    ShaderProgram* res = dynamic_cast<ShaderProgram*>(Get(path));
    if(res != NULL)
        return res;

    Logger::Message("Loading resource: \"" + path + "\"");
    
    ShaderProgram* temp = new ShaderProgram();
    temp->resource_factory = this;
    temp->Load(vp, pp);
    return NULL;
    temp->resourceId = path;
    temp->Instantiate();
    resources[path] = temp;
    return temp;
}

Resource* ResourceFactory:: Load(std::string path, RESOURCE_TYPE type)
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
    else
        return NULL;
    
    temp->Load(path);
    temp->resource_factory = this;
    temp->resourceId = path;
    resources[path] = temp;
    return temp;  
}

void ResourceFactory::Release(std::string path)
{
    delete resources[path];
    resources.erase(path);
}    

void ResourceFactory::Release(Resource *resource)
{
    //not implemented
}    

void ResourceFactory::ReleaseAll()
{
    //not implemented
}

void* VertexBuffer:: GetPointer() const 
{ 
    return (void*)vertices; 
}

unsigned int VertexBuffer:: GetNum() const 
{
    return num_vertices; 
}

void VertexBuffer:: Create(int num_vertices) 
{ 
    this->num_vertices = num_vertices; 
    vertices = new Vertex[num_vertices](); 
}

VertexArrayObject* VertexBuffer:: GetVAO() const 
{ 
    return vao; 
}

bool VertexBuffer:: Instantiate()
{
    vao = new VertexArrayObject();
    vao->Instantiate();
    
    Renderer::GetInstance()->BindVAO(this);

    _id = -1;
    _id = Renderer::GetInstance()->CreateVBO(this, STATIC);

    return (_id != -1) && (vao->GetId() != -1);
}

void VertexBuffer:: Free()
{
    if(_id != -1)
    Renderer::GetInstance()->DeleteVBO(this);
    _id = -1;

    delete[] (Vertex*)vertices;
    delete vao;
}

void* VertexBuffer:: Lock() const
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

void VertexBuffer:: Unlock() const
{
#ifdef MOBITECH_WIN32
    glBindBuffer(GL_ARRAY_BUFFER, GLObject::_id);
    GLboolean result = glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif //MOBITECH_WIN32
}

bool VertexArrayObject:: Instantiate()
{
    _id = -1;    
    _id = Renderer::GetInstance()->CreateVAO();
    return (_id != -1);
}

void VertexArrayObject::  Free()
{
    if(_id != -1)
        Renderer::GetInstance()->DeleteVAO(this);
    _id = -1;
}

bool Shader::Instantiate()
{
    _id = Renderer::GetInstance()->CompileShader(source, type);
    return true;
}

bool Shader::Load(std::string path) 
{
#ifdef MOBITECH_WIN32
    FILE* file;
    file = fopen(path.c_str() , "rb");
    if(file == NULL)
    {
        Logger::Message("Shader: Can`t open file " + path, LT_ERROR);    
        return false;
    }

    while(!feof(file))
    {
        char temp = '\0';
        fread(&temp, 1, 1, file);
        source.push_back(temp);
    }    
        
    if(file != NULL)
    {
        fclose(file);
    }
#else if MOBITECH_ANDROID
    Logger::Message("LOAD SHADER 1");
    FileData file = resource_factory->GetFileData(path.c_str());
    Logger::Message(LT_INFO, "LOAD SHADER 2", path.c_str());
    if (file.data_length <= 0) 
    {
        Logger::Message(LT_ERROR, "Error opening %s from APK", path.c_str());
        return false;
    }
    Logger::Message(LT_INFO, "LOAD SHADER 3", path.c_str());
    for(int i = 0; i < file.data_length; i++)
        source.push_back(((char*)file.data)[i]);
    Logger::Message(LT_INFO, "LOAD SHADER 4", path.c_str());
    resource_factory->ReleaseFileData(&file);
    Logger::Message(LT_INFO, "LOAD SHADER 5", path.c_str());
#endif //MOBITECH_ANDROID   
    return true;
}

void Shader::Free()
{
    if(_id != -1)
        Renderer::GetInstance()->DeleteShader(this);
    _id = -1;
    source.clear();
}


void ShaderProgram::Free()
{    
    if(_id != -1)
        Renderer::GetInstance()->DeleteShaderProgram(this);
    _id = -1;
}

void ShaderProgram:: InitLocations()
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

bool ShaderProgram:: Load(string path)
{
    //not implemented yet
    return false;
}

bool ShaderProgram:: Instantiate()
{
    _id = Renderer::GetInstance()->CreateShaderProgram(vertex_sh, pixel_sh);
    InitLocations();
    return true;
}

bool ShaderProgram:: Load(std::string vertexshd_path, std::string pixelshd_path)
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
    
    return true;
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
}

Texture::~Texture(void)
{
    Free();
}

bool Texture:: Instantiate()
{
    Renderer::GetInstance()->CreateTexture(this);
    return true;
}

bool Texture:: Load(std:: string path)
{   
    std::vector<unsigned char> png;
    lodepng::load_file(png, path);
    unsigned error = lodepng::decode(data, width, height, path);
    if(error)
        return false;
    return true;
}

void Texture::   Free()
{
    if(_id != -1)
        Renderer::GetInstance()->DeleteTexture(this);
}
