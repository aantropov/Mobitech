#include "mobitech.h"
#include "resources.h"
#include "renderer.h"

Resource* ResourceFactory:: Create(RESOURCE_TYPE type)
{
    char buffer[MAXCHAR];
    memset(buffer, '\0', MAXCHAR);
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

    temp->resourceFactory = this;
    temp->resourceId = path;
    resources[path] = temp;
    return temp;  
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
    temp->resourceFactory = this;
    temp->Load(vp, pp);
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
    temp->resourceFactory = this;
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
    FILE* fShd;
    fShd = fopen(path.c_str() , "rb");

    if(fShd == NULL)
    {
        Logger::Message("Shader: Can`t open file " + path, LT_ERROR);    
        return false;
    }

    while(!feof(fShd))
    {
        char temp = '\0';
        fread(&temp, 1, 1, fShd);
        source.push_back(temp);
    }    

    if(fShd != NULL)
    {
        fclose(fShd);
    }
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
    uniformLocations.transform_model = glGetUniformLocation(_id, "transform.model");
    uniformLocations.transform_viewProjection = glGetUniformLocation(_id, "transform.viewProjection");
    uniformLocations.transform_normal = glGetUniformLocation(_id, "transform.normal");
    uniformLocations.transform_modelViewProjection = glGetUniformLocation(_id, "transform.modelViewProjection");
    uniformLocations.transform_viewPosition = glGetUniformLocation(_id, "transform.viewPosition");
    
    attributeLocations.color = glGetAttribLocation(_id, "color");
    attributeLocations.position = glGetAttribLocation(_id, "position");
    attributeLocations.texcoords = glGetAttribLocation(_id, "texcoords");    
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
    Shader *vs = dynamic_cast<Shader*>(resourceFactory->Get(vertexshd_path));
    Shader *ps = dynamic_cast<Shader*>(resourceFactory->Get(pixelshd_path));

    vertex_sh = dynamic_cast<Shader*> (resourceFactory->Load(vertexshd_path, RT_SHADER));
    pixel_sh = dynamic_cast<Shader*> (resourceFactory->Load(pixelshd_path, RT_SHADER));

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
