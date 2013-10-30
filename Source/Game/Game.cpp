#include "../Core/Mobitech.h"

const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f,
        0.5f, -0.5f };

class GameScene : public Scene, IInputListener
{
    ShaderProgram* shader;

public:
    
    GameScene() 
    {  
        shader = Engine::rf.Load(ASSETS_ROOT + "Shaders\\diffuse.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");
        shader->Instantiate();
        Input::GetInstance()->Register(this); 
    }
    ~GameScene() { Input::GetInstance()->Unregister(this); }

    virtual void Update(float dt)
    {
    
    }

    virtual void DrawFrame()
    {
        Renderer::GetInstance()->SetShaderProgram(shader);

        glVertexAttribPointer(shader->attributeLocations.position, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
        glEnableVertexAttribArray(shader->attributeLocations.position);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    virtual void OnTouchDown(int x, int y) {}
    virtual void OnTouchUp(int x, int y) {}
    virtual void OnMove(int x, int y) {}
};

void GameMain()
{
    Engine engine = *(Engine::GetInstance());
    engine.Initialize();
    
    GameScene gameScene;
    engine.SetScene(&gameScene);
    
    engine.Run();
    engine.Stop();
}

