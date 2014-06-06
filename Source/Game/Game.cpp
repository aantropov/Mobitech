#include "../Core/Mobitech.h"

float vertices_ortho_01[] = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 
                     1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f };

float texcoords_ortho_01[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 
                      1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f };

class GameScene;
class GameObject
{
public:
    bool is_destroying;
    GameScene *current_scene;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual ~GameObject() {}
    GameObject(): is_destroying(false) {}
};

class Bullet : public RigidBody, public GameObject
{
    IndexBuffer index_buffer;
    Texture* texture;
    ShaderProgram* shader;

public:
    
    Bullet() : RigidBody(300.0f, PO_STATIC)
    {
        shape = new VertexBuffer();
        
        shape->Create(6);
        
        float width = 32;
        float height = 32;
        
        Vertex *ptr = ((Vertex*)shape->GetPointer());

        ptr[0].pos = vec3(-width*0.5f,-height*0.5f, 0.0f);
        ptr[0].color = vec3_one;
        ptr[0].texcoord = vec2(0.0f, 0.0f);

        ptr[1].pos = vec3(width*0.5f,-height*0.5f, 0.0f);
        ptr[1].color = vec3_one;
        ptr[1].texcoord = vec2(1.0f, 0.0f);

        ptr[2].pos = vec3(width*0.5f, height*0.5f, 0.0f);
        ptr[2].color = vec3_one;
        ptr[2].texcoord = vec2(1.0f, 1.0f);

        ptr[3].pos = vec3(-width*0.5f,-height*0.5f, 0.0f);
        ptr[3].color = vec3_one;
        ptr[3].texcoord = vec2(0.0f, 0.0f);
        
        ptr[4].pos = vec3(width*0.5f, height*0.5f, 0.0f);
        ptr[4].color = vec3_one;
        ptr[4].texcoord = vec2(1.0f, 1.0f);

        ptr[5].pos = vec3(-width*0.5f,height*0.5f, 0.0f);
        ptr[5].color = vec3_one;
        ptr[5].texcoord = vec2(0.0f, 1.0f);

        shape->Instantiate();

        index_buffer.Create(2);
        index_buffer.Fill(GL_TRIANGLES);
        index_buffer.Instantiate();

        shader = Engine::main_resource_factory.Load(ASSETS_ROOT + "Shaders\\diffuse.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");
        texture = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\bullet.png", RT_TEXTURE));

        this->velocity = vec3(900.0f, 0.0f, 0.0f);        

        OPENGL_CHECK_FOR_ERRORS();
    }

    virtual ~Bullet() { delete shape; }

    virtual void Draw()
    {
        Renderer *render = Renderer::GetInstance();
        
        render->SetupCameraForShaderProgram(shader, model.matrix());
        render->BindTexture(texture, 0);
        
        render->EnableBlend(BT_ADDITIVE);

        render->BindBuffer(shape);
        render->BindBuffer(&index_buffer);
        render->DrawBuffer(&index_buffer);
        render->UnbindBuffer(true);
        render->UnbindBuffer(false);

        render->DisableBlend();
    }

    virtual void Update();   

    virtual void OnCollide(RigidBody *body);    
};

class Ship : public RigidBody
{
    IndexBuffer index_buffer;
    Texture* texture;
    ShaderProgram* shader;

public:
    
    float strafe_speed;
    float width;
    float height;

    Ship() : RigidBody(300.0f, PO_STATIC), strafe_speed(500.0f), width(75), height(150)
    {
        shape = new VertexBuffer();
        
        shape->Create(6);
        
        Vertex *ptr = ((Vertex*)shape->GetPointer());

        ptr[0].pos = vec3(-width*0.5f,-height*0.5f, 0.0f);
        ptr[0].color = vec3_one;
        ptr[0].texcoord = vec2(0.0f, 0.0f);

        ptr[1].pos = vec3(width*0.5f,-height*0.5f, 0.0f);
        ptr[1].color = vec3_one;
        ptr[1].texcoord = vec2(1.0f, 0.0f);

        ptr[2].pos = vec3(width*0.5f, height*0.5f, 0.0f);
        ptr[2].color = vec3_one;
        ptr[2].texcoord = vec2(1.0f, 1.0f);

        ptr[3].pos = vec3(-width*0.5f,-height*0.5f, 0.0f);
        ptr[3].color = vec3_one;
        ptr[3].texcoord = vec2(0.0f, 0.0f);
        
        ptr[4].pos = vec3(width*0.5f, height*0.5f, 0.0f);
        ptr[4].color = vec3_one;
        ptr[4].texcoord = vec2(1.0f, 1.0f);

        ptr[5].pos = vec3(-width*0.5f,height*0.5f, 0.0f);
        ptr[5].color = vec3_one;
        ptr[5].texcoord = vec2(0.0f, 1.0f);

        shape->Instantiate();

        index_buffer.Create(2);
        index_buffer.Fill(GL_TRIANGLES);
        index_buffer.Instantiate();

        shader = Engine::main_resource_factory.Load(ASSETS_ROOT + "Shaders\\diffuse.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");
        texture = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\alien2.png", RT_TEXTURE));

        OPENGL_CHECK_FOR_ERRORS();
    }
    
    void Draw()
    {
        Renderer *render = Renderer::GetInstance();
        
        render->SetupCameraForShaderProgram(shader, model.matrix());
        render->BindTexture(texture, 0);

        render->BindBuffer(shape);
        render->BindBuffer(&index_buffer);
        render->DrawBuffer(&index_buffer);
        render->UnbindBuffer(true);
        render->UnbindBuffer(false);
    }

    virtual ~Ship() { delete shape; }

    virtual void OnCollide(RigidBody *body) {}
};

class Asteroid : public RigidBody, public GameObject
{    
    IndexBuffer index_buffer;
    Texture* texture;
    ShaderProgram* shader;

public:
    
    float size;

    Asteroid(float _size = unirand(50.0f, 150.0f)) : RigidBody(_size * 300.0f, PO_DYNAMIC)
    {
        shape = new VertexBuffer();
        unsigned int count = (unsigned int)unirand(7, 11);
        shape->Create(count);
        this->size = _size;

        float dispertion = 0.2f;

        for(unsigned int i = 0; i < count; i++)
        {            
            float angle = float(i) * (360.0f/float(count)) * math_radians;
            ((Vertex*)shape->GetPointer())[i].pos = vec3(cosf(angle) + unirand(-dispertion, dispertion), sinf(angle) + unirand(-dispertion, dispertion), 0.0f) * size;
            ((Vertex*)shape->GetPointer())[i].color = vec3_one;//vec3(unirand(0.5f, 1.0f), unirand(0.5f, 1.0f), unirand(0.5f, 1.0f));
            ((Vertex*)shape->GetPointer())[i].texcoord = vec2(cosf(angle) * 0.5f + 0.5f, sinf(angle)* 0.5f + 0.5f);
        }
        shape->Instantiate();

        index_buffer.Create(count - 2);
        index_buffer.Fill(GL_TRIANGLE_FAN);
        index_buffer.Instantiate();

        shader = Engine::main_resource_factory.Load(ASSETS_ROOT + "Shaders\\diffuse.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");
        texture = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\asteroid.png", RT_TEXTURE));

        this->velocity = -vec2_x * unirand(50.0f, 350.0f);
        //this->rotation = unirand(50.0f, -50.0f);
        this->elasticity = 0.001f;

        OPENGL_CHECK_FOR_ERRORS();
    }
    
    void Draw()
    {
        Renderer *render = Renderer::GetInstance();
        
        render->SetupCameraForShaderProgram(shader, model.matrix());
        render->BindTexture(texture, 0);

        render->BindBuffer(shape);
        render->BindBuffer(&index_buffer);
        render->DrawBuffer(&index_buffer);
        render->UnbindBuffer(true);
        render->UnbindBuffer(false);
    }

    virtual void Update();
    virtual void OnCollide(RigidBody *body);
    virtual ~Asteroid() { delete shape; }    
};

class GameScene : public Scene, IInputListener
{
    ShaderProgram* shader;
    ShaderProgram* background;

    vec4 colors[6];

    bool touch_pressed;
    vec2 prev_mouse_pos;
    Camera camera;
    Camera camera_ortho_01;
    float background_rotation;
    RenderTexture rt;
    BMFont* font;
    Texture *nebula_tile_1;
    Texture *nebula_tile_2;
    Texture *background_stars;
    
    Ship ship;

    float render_w;
    float render_h;

    std::list<GameObject*> objects;

    float latest_asteroid_spawn_time;
    float asteroid_spawn_time;

public:

    GameScene() : asteroid_spawn_time(5.0f)
    {         
        background_rotation = 0.0f;
        Renderer *render = Renderer::GetInstance();

        render_h = 800.0f;
        render_w = (render_h * render->GetWidth()) / render->GetHeight();
        //camera.Create(-render->GetWidth()/2.0f, -render->GetHeight()/2.0f, 0.0f);
        camera.Ortho(0.0f, render_w, 0.0f, render_h, 0.0f, 1000.0f);
                
        camera_ortho_01.Create(0.0f, 0.0f, 0.0f);
        camera_ortho_01.Ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
        
        touch_pressed = false;

        font = dynamic_cast<BMFont*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Fonts\\Font_plain.txt", RT_BM_FONT));
        shader = Engine::main_resource_factory.Load(ASSETS_ROOT + "Shaders\\diffuse.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");
        background = Engine::main_resource_factory.Load(ASSETS_ROOT + "Shaders\\background.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");

        nebula_tile_1 = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\nebula_tile1.png", RT_TEXTURE));
        nebula_tile_2 = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\nebula_tile2.png", RT_TEXTURE));
        background_stars = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\stars.png", RT_TEXTURE));
        Input::GetInstance()->Register(this);

        colors[0] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[1] = vec4(0.7f, 1.0f, 1.0f, 1.0f);
        colors[2] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[3] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[4] = vec4(1.0f, 1.0f, 0.7f, 1.0f);
        colors[5] = vec4(1.0f, 1.0f, 1.0f, 1.0f);

        render->SetCurrentCamera(&camera);
          
        rt.Initialize(render->GetWidth(), render->GetHeight());
        rt.GetTexture()->name = "text";
        
        render->ClearColor(vec4(0.3f, 0.3f, 0.3f, 0.0f));       

        ship.model.position = vec3(0.0f, 0.0f, 0.0f);
        ship.model.rotation = quat(GLRotationZ(90.0f));

        camera.SetPosition(ship.model.position - vec2(render_w * 0.15f, render_h * 0.5f));

        //to seconds
        latest_asteroid_spawn_time = (float)Engine::GetTimeMS() * 0.001f;
    }

    ~GameScene() { Input::GetInstance()->Unregister(this); }

    virtual void Update(double dt)
    {
        Renderer *render = Renderer::GetInstance();
        //camera.SetPosition(ship.model.position - vec2(render_w * 0.3f, render_h * 0.5f));
        
        for(auto it = objects.begin(); it != objects.end(); )
            (*(it++))->Update();
        
        for(auto it = objects.begin(); it != objects.end(); )
        {
            GameObject *obj = *(it++);
            if(obj->is_destroying)
                DeleteObject(obj);
        }

        ship.model.position.y = clamp(ship.model.position.y, -(render_h*0.5f - 40.0f), render_h*0.5f - 40.0f);

        background_rotation += (float)dt * 2.0f;
        if(background_rotation >= 360.0f)
            background_rotation = 0.0f;

        if(abs(latest_asteroid_spawn_time - (float)Engine::GetTimeMS() * 0.001f) > asteroid_spawn_time)
        {
            CreateObject(new Asteroid(), vec3(render_w + 300.0f, unirand(-render_h,render_h) * 0.5f, 0.0f));
            latest_asteroid_spawn_time = (float)Engine::GetTimeMS() * 0.001f;
        }
    }

    virtual void DrawFrame()
    {        
        Renderer *render = Renderer::GetInstance();

        mat4 model = mat4_identity;
        background_rotation = clamp(background_rotation, 0.0f, 360.0f);

        rt.Begin();
        render->Clear();
        
        render->SetCurrentCamera(&camera);
        render->BindShaderProgram(shader);
        render->SetupCameraForShaderProgram(shader, mat4_identity);
        
        ship.Draw();

        for(auto it = objects.begin(); it !=  objects.end(); it++)
            (*it)->Draw();              

        render->SetupCameraForShaderProgram(shader, mat4_identity);
        render->EnableBlend(BT_ALPHA_BLEND);
        //font->Print(0, 0, "ololo");
        render->DisableBlend();

        rt.End();
        
        //
        render->SetCurrentCamera(&camera_ortho_01);
        render->BindShaderProgram(background);        
        render->SetupCameraForShaderProgram(font->shader, GLScale(1.5f, 1.5f, 1.5f) * GLRotationZ(background_rotation));

        render->EnableBlend(BT_ALPHA_BLEND);
        render->BindTexture(nebula_tile_1, 0);
        render->DrawTriangles(vertices_ortho_01, colors, texcoords_ortho_01, 6);
        
        render->SetupCameraForShaderProgram(font->shader, GLScale(1.5f, 1.5f, 1.5f) * GLRotationZ(background_rotation * -2.0f));
        render->EnableBlend(BT_MULTIPLY);
        render->BindTexture(nebula_tile_2, 0);
        render->DrawTriangles(vertices_ortho_01, colors, texcoords_ortho_01, 6);

        render->BindShaderProgram(font->shader);
        render->SetupCameraForShaderProgram(font->shader, mat4_identity);
        render->EnableBlend(BT_ADDITIVE);
        render->BindTexture(background_stars, 0);
        render->DrawTriangles(vertices_ortho_01, colors, texcoords_ortho_01, 6);

        render->EnableBlend(BT_ALPHA_BLEND);
        render->BindTexture(rt.GetTexture(), 0);
        render->DrawTriangles(vertices_ortho_01, colors, texcoords_ortho_01, 6);
        
        //font->Print(-300.0f, 0.0f, GLScale(1.0f/render->GetWidth(), 1.0f/render->GetHeight(), 1.0f), "This is a different font, centered.");
        render->DisableBlend();
    }

    virtual void OnTouchDown(int x, int y, unsigned int touch_id = 0) 
    { 
        touch_pressed = true; 
        //prev_mouse_pos = vec2((float)x, (float)y); 

        float sign = 1.0f;
        float ship_y = 1.0f - (ship.model.position.y + render_h*0.5f)/render_h;
        float abs_y = (float(y)/Renderer::GetInstance()->GetHeight());
        if(abs_y > ship_y)
            sign *= -1.0f;
           
        if(abs(abs_y - ship_y) > ship.width/render_h)
            ship.velocity = vec3(0.0f, ship.strafe_speed * sign, 0.0f);
        else
            CreateObject(new Bullet(), ship.model.position + vec3(ship.height*0.5f, 0.0f, 0.0f));
    }

    virtual void OnTouchUp(int x, int y, unsigned int touch_id = 0) 
    { 
        ship.velocity = vec3_zero;
        touch_pressed = false; 
    }

    virtual void OnMove(int x, int y, unsigned int touch_id = 0)
    {
        if(!touch_pressed)
            return;

        vec3 pos = camera.GetPosition();
        pos.x += prev_mouse_pos.x - x;
        pos.y -= prev_mouse_pos.y - y;
        //camera.SetPosition(vec3(pos.x, pos.y, 0.0f));

        prev_mouse_pos = vec2((float)x, (float)y);
    }
    
    void DeleteObject(GameObject* obj)
    {
        objects.remove(obj);
        delete obj;
    }

    void CreateObject(GameObject* obj, vec3 position)
    {
        RigidBody* body = dynamic_cast<RigidBody*>(obj);
        if(body != NULL)
            body->model.position = position;
            
        obj->current_scene = this;
        objects.push_back(obj);
    }
};

void Bullet:: OnCollide(RigidBody *body)
{
    if(body != NULL && dynamic_cast<Asteroid*>(body) != NULL)
        is_destroying = true;
}

void Bullet:: Update()
{
    if(length(model.position) > 2000.0f)
        current_scene->DeleteObject(this);
}

void Asteroid:: OnCollide(RigidBody *body)
{
    if(dynamic_cast<Bullet*>(body) != NULL)
    {
        is_destroying = true;        

        if(size > 7.0f)
        {
            current_scene->CreateObject(new Asteroid(size * 0.5f), model.position + vec2_y*size*0.6f);
            current_scene->CreateObject(new Asteroid(size * 0.5f), model.position - vec2_y*size*0.6f);
        }
    }
}

void Asteroid:: Update()
{
    if(length(model.position) > 2000.0f)
        current_scene->DeleteObject(this);
}

void GameMain()
{
    Engine *engine = Engine::GetInstance();
    engine->Initialize();

    std::tr1::shared_ptr<GameScene> game_scene(new GameScene());
    engine->SetScene(game_scene);

    engine->Run();
}

