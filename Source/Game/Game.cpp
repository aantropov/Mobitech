#include "../Core/Mobitech.h"
float vertices[] = { 0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };

class GameScene : public Scene, IInputListener
{
    ShaderProgram* shader;
    vec4 colors[3];

public:
    
    GameScene()
    {  
        shader = Engine::main_resource_factory.Load(ASSETS_ROOT + "Shaders\\diffuse.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");
        Input::GetInstance()->Register(this);

        /*vertices[0] = vec2(0.0f, 0.5f);
        vertices[1] = vec2(-0.5f, -0.5f);
        vertices[3] = vec2(0.5f, -0.5f);*/

        colors[0] = vec4(0.5f, 0.5f, 0.5f, 1.0f);
        colors[1] = vec4(0.5f, 0.5f, 0.5f, 1.0f);
        colors[2] = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    }
    ~GameScene() { Input::GetInstance()->Unregister(this); }

    virtual void Update(float dt)
    {
    
    }

    virtual void DrawFrame()
    {
        Renderer::GetInstance()->SetShaderProgram(shader);
     
        glVertexAttribPointer(shader->attribute_locations.position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glEnableVertexAttribArray(shader->attribute_locations.position);
        
        glVertexAttribPointer(shader->attribute_locations.color, 4, GL_FLOAT, GL_FALSE, 0, colors);
        glEnableVertexAttribArray(shader->attribute_locations.color);
     
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    virtual void OnTouchDown(int x, int y) {}
    virtual void OnTouchUp(int x, int y) {}
    
    virtual void OnMove(int x, int y)
    {
        Renderer* renderer = Renderer::GetInstance();
        float t = (float)x / renderer->GetWidth();
        float v = (float)y / renderer->GetHeight();

        colors[0] = vec4(t-v, v*v, t*t, 1.0f);
        colors[1] = vec4(1.0f - t, 1.0f/t, v*2.0f, 1.0f);
        colors[2] = vec4(v-t, v*t, v+t, 1.0f);
    }
};

void GameMain()
{
    Engine *engine = Engine::GetInstance();
    engine->Initialize();
    
    std::tr1::shared_ptr<GameScene> gameScene(new GameScene());
    engine->SetScene(gameScene);
    
    engine->Run();
    //engine->Stop();
}

