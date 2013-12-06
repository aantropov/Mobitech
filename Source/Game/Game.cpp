#include "../Core/Mobitech.h"
float vertices[] = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 
                     1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f };
float texcoords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 
                      1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f };

class GameScene : public Scene, IInputListener
{
    ShaderProgram* shader;
    vec4 colors[6];
    Texture* test_texture;

    bool touch_pressed;
    vec2 prev_mouse_pos;
    Camera camera;
    Camera test_camera;
    float angle;
    Animation *test_animation;
    RenderTexture rt;

public:

    GameScene()
    {         
        angle = 0.0f;
        Renderer *render = Renderer::GetInstance();

        float h = 600.0f;
        float w = (h * render->GetWidth()) /render->GetHeight();
        camera.Create(-400.0f, -200.0f, 0.0f);
        camera.Ortho(0.0f, w, 0.0f, h, 0.0f, 1000.0f);
        
        test_camera.Ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
        test_camera.Create(0.0f, 0.0f, 0.0f);

        touch_pressed = false;

        test_animation = dynamic_cast<Animation*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Animations\\111.aml", RT_ANIMATION));
        shader = Engine::main_resource_factory.Load(ASSETS_ROOT + "Shaders\\diffuse.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");
        test_texture = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\Noise.png", RT_TEXTURE));
        test_texture->name = "text";

        Input::GetInstance()->Register(this);

        /*vertices[0] = vec2(0.0f, 0.5f);
        vertices[1] = vec2(-0.5f, -0.5f);
        vertices[3] = vec2(0.5f, -0.5f);*/

        colors[0] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[1] = vec4(0.9f, 1.0f, 1.0f, 1.0f);
        colors[2] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[3] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[4] = vec4(1.0f, 1.0f, 0.9f, 1.0f);
        colors[5] = vec4(1.0f, 1.0f, 1.0f, 1.0f);

        render->SetCurrentCamera(&camera);

        rt.Initialize(512, 512, "text");

        render->ClearColor(vec4_zero);
    }

    ~GameScene() { Input::GetInstance()->Unregister(this); }

    virtual void Update(double dt)
    {
        angle += dt;
        if(angle >= 1.0f)
            angle = 0.0f;
    }

    virtual void DrawFrame()
    {
        Renderer *render = Renderer::GetInstance();

        mat4 model = mat4_identity;
        //model = GLRotationZ(angle);
        angle = clamp(angle, 0.0f, 1.0f);
        
        rt.Begin();
        render->SetCurrentCamera(&camera);
        render->BindShaderProgram(shader);
        render->SetupCameraForShaderProgram(shader, mat4_identity);

        test_animation->GetAnimationClip("banana_level4")->SetModel(GLScale(1.0f, -1.0f, 1.0f), false);
        test_animation->GetAnimationClip("banana_level4")->Draw(angle);

        test_animation->GetAnimationClip("banana_level2")->SetModel(GLScale(1.0f, -1.0f, 1.0f) * GLTranslation(vec2(-400, 200)), false);
        test_animation->GetAnimationClip("banana_level2")->Draw(angle);
                
        test_animation->GetAnimationClip("banana_level3")->SetModel(GLScale(1.0f, -1.0f, 1.0f) * GLTranslation(vec2(-150, 150)), false);
        test_animation->GetAnimationClip("banana_level3")->Draw(angle);/**/
        rt.End();      
               
        render->EnableBlend(BT_ADDITIVE);
        render->SetCurrentCamera(&test_camera);
        render->BindShaderProgram(shader);
        render->SetupCameraForShaderProgram(shader, mat4_identity);
        render->BindTexture(rt.GetTexture(), 0);

        glVertexAttribPointer(shader->attribute_locations.color, 4, GL_FLOAT, GL_FALSE, 0, colors);
        glEnableVertexAttribArray(shader->attribute_locations.color);

        glVertexAttribPointer(shader->attribute_locations.texcoords, 2, GL_FLOAT, GL_FALSE, 0, texcoords);
        glEnableVertexAttribArray(shader->attribute_locations.texcoords);
        
        glVertexAttribPointer(shader->attribute_locations.position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glEnableVertexAttribArray(shader->attribute_locations.position);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        render->DisableBlend();
    }

    virtual void OnTouchDown(int x, int y, unsigned int touch_id = 0) { touch_pressed = true; prev_mouse_pos = vec2((float)x, (float)y); }
    virtual void OnTouchUp(int x, int y, unsigned int touch_id = 0) { touch_pressed = false; }

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

