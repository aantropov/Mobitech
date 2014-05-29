#include "../Core/Mobitech.h"
float vertices[] = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 
                     1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f };
float texcoords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 
                      1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f };

class Asteroid : public RigidBody
{
    IndexBuffer index_buffer;
    Texture* texture;
    ShaderProgram* shader;

public:
    
    Asteroid() : RigidBody(200.0f, PO_DYNAMIC)
    {
        shape = new VertexBuffer();
        unsigned int count = unirand(7, 11);
        shape->Create(count);
        float size = unirand(10.0f, 50.0f);
        float dispertion = 0.2f;

        for(int i = 0; i < count; i++)
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

    ~Asteroid() { delete shape; }

    virtual void OnCollide(RigidBody *body) {}
};

#define ASTEROIDS_COUNT 10
class GameScene : public Scene, IInputListener
{
    ShaderProgram* shader;
    ShaderProgram* background;

    vec4 colors[6];
    Texture* test_texture;

    bool touch_pressed;
    vec2 prev_mouse_pos;
    Camera camera;
    Camera test_camera;
    float angle;
    float background_rotation;
    Animation *test_animation;
    RenderTexture rt;
    BMFont* font;
    Texture *nebula_tile_1;
    Texture *nebula_tile_2;
    Texture *background_stars;

    Asteroid test_asteroid[ASTEROIDS_COUNT];

public:

    GameScene()
    {         
        angle = 0.0f;
        background_rotation = 0.0f;
        Renderer *render = Renderer::GetInstance();

        float h = 600.0f;
        float w = (h * render->GetWidth()) /render->GetHeight();
        camera.Create(-render->GetWidth()/2.0f, -render->GetHeight()/2.0f, 0.0f);
        camera.Ortho(0.0f, w, 0.0f, h, 0.0f, 1000.0f);
                
        test_camera.Create(0.0f, 0.0f, 0.0f);
        test_camera.Ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
        
        touch_pressed = false;

        font = dynamic_cast<BMFont*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Fonts\\Font_plain.txt", RT_BM_FONT));
        test_animation = dynamic_cast<Animation*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Animations\\111.aml", RT_ANIMATION));
        shader = Engine::main_resource_factory.Load(ASSETS_ROOT + "Shaders\\diffuse.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");
        background = Engine::main_resource_factory.Load(ASSETS_ROOT + "Shaders\\background.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");

        test_texture = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\Noise.png", RT_TEXTURE));
        nebula_tile_1 = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\nebula_tile1.png", RT_TEXTURE));
        nebula_tile_2 = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\nebula_tile2.png", RT_TEXTURE));
        background_stars = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\stars.png", RT_TEXTURE));
        Input::GetInstance()->Register(this);

        /*vertices[0] = vec2(0.0f, 0.5f);
        vertices[1] = vec2(-0.5f, -0.5f);
        vertices[3] = vec2(0.5f, -0.5f);*/

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

        for(int i = 0; i < ASTEROIDS_COUNT; i++)
        {
            float angle = float(i) * (360.0f/float(ASTEROIDS_COUNT)) * math_radians;

            test_asteroid[i].velocity = -vec2(cosf(angle), sinf(angle)) * unirand(50.0f, 350.0f);
            test_asteroid[i].rotation = 15.0f;
            //test_asteroid[i].model.rotation = GLRotationZ(79.0f);
            test_asteroid[i].model.position = vec3(cosf(angle), sinf(angle), 0.0f) * 300.0f;
            test_asteroid[i].elasticity = 0.005f;
        }
    }

    ~GameScene() { Input::GetInstance()->Unregister(this); }

    virtual void Update(double dt)
    {
        angle += dt;
        if(angle >= 1.0f)
            angle = 0.0f;

        background_rotation += dt * 2.0f;
        if(background_rotation >= 360.0f)
            background_rotation = 0.0f;
    }

    virtual void DrawFrame()
    {        
        Renderer *render = Renderer::GetInstance();

        mat4 model = mat4_identity;
        //model = GLRotationZ(angle);
        angle = clamp(angle, 0.0f, 1.0f);
        background_rotation = clamp(background_rotation, 0.0f, 360.0f);

        rt.Begin();
        render->Clear();
        
        render->SetCurrentCamera(&camera);
        render->BindShaderProgram(shader);
        render->SetupCameraForShaderProgram(shader, mat4_identity);
        
        test_animation->GetAnimationClip("banana_level2")->SetModel(GLScale(1.0f, -1.0f, 1.0f), false);
        test_animation->GetAnimationClip("banana_level2")->Draw(angle);

        for(int i = 0; i < ASTEROIDS_COUNT; i++)
            test_asteroid[i].Draw();        
        
        render->SetupCameraForShaderProgram(shader, mat4_identity);
        render->EnableBlend(BT_ALPHA_BLEND);
        //font->Print(0, 0, "ololo");
        render->DisableBlend();

        rt.End();
        
        render->SetCurrentCamera(&test_camera);
        render->BindShaderProgram(background);        
        render->SetupCameraForShaderProgram(font->shader, GLScale(1.5f, 1.5f, 1.5f) * GLRotationZ(background_rotation));

        render->EnableBlend(BT_ALPHA_BLEND);
        render->BindTexture(nebula_tile_1, 0);
        render->DrawTriangles(vertices, colors, texcoords, 6);
        
        render->SetupCameraForShaderProgram(font->shader, GLScale(1.5f, 1.5f, 1.5f) * GLRotationZ(background_rotation * -2.0f));
        render->EnableBlend(BT_MULTIPLY);
        render->BindTexture(nebula_tile_2, 0);
        render->DrawTriangles(vertices, colors, texcoords, 6);

        render->BindShaderProgram(font->shader);
        render->SetupCameraForShaderProgram(font->shader, mat4_identity);
        render->EnableBlend(BT_ADDITIVE);
        render->BindTexture(background_stars, 0);
        render->DrawTriangles(vertices, colors, texcoords, 6);

        render->EnableBlend(BT_ALPHA_BLEND);
        render->BindTexture(rt.GetTexture(), 0);
        render->DrawTriangles(vertices, colors, texcoords, 6);
        
        //font->Print(-300.0f, 0.0f, GLScale(1.0f/render->GetWidth(), 1.0f/render->GetHeight(), 1.0f), "This is a different font, centered.");
        render->DisableBlend();
    }

    virtual void OnTouchDown(int x, int y, unsigned int touch_id = 0) { touch_pressed = true; prev_mouse_pos = vec2((float)x, (float)y); }
    virtual void OnTouchUp(int x, int y, unsigned int touch_id = 0) 
    { 
        touch_pressed = false; 
    }

    virtual void OnMove(int x, int y, unsigned int touch_id = 0)
    {
        if(!touch_pressed)
            return;

        vec3 pos = camera.GetPosition();
        pos.x += prev_mouse_pos.x - x;
        pos.y -= prev_mouse_pos.y - y;
        camera.SetPosition(vec3(pos.x, pos.y, 0.0f));

        prev_mouse_pos = vec2((float)x, (float)y);
    }
};

void GameMain()
{
    Engine *engine = Engine::GetInstance();
    engine->Initialize();

    std::tr1::shared_ptr<GameScene> game_scene(new GameScene());
    engine->SetScene(game_scene);

    engine->Run();
}

