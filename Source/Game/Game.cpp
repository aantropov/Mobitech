#include "../Core/Mobitech.h"

class GameScene : public Scene, IInputListener
{
public:

    GameScene() { Input::GetInstance()->Register(this); }
    ~GameScene() { Input::GetInstance()->Unregister(this); }

    virtual void Update(float dt)
    {
    
    }

    virtual void DrawFrame()
    {
    
    }

    virtual void OnTouchDown(int x, int y) {}
    virtual void OnTouchUp(int x, int y) {}
    virtual void OnMove(int x, int y) {}
};

void GameMain()
{
    GameScene gameScene;

    Engine engine;
    engine.Initialize();
    engine.SetScene(&gameScene);
    engine.Run();
    engine.Stop();
}