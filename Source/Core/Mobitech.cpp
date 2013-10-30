#include "Mobitech.h"
#include "Renderer.h"

ResourceFactory Engine::rf;

bool Engine::Initialize()
{
    elapsedTime = 0;
    fps = 0;

    if(Window::IsRunning())
        return true;

    Logger::Message("Start application");
    Renderer::GetInstance()->Initialize();
    
    return true;
}

void Engine:: Stop()
{
    Window::SetRunning(false);
}

void Engine:: OneFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
	Renderer::GetInstance()->drawCalls = 0;

    if(currentScene != NULL)
        currentScene->DrawFrame();

#ifdef MOBITECH_WIN32
    SwapBuffers(Window::GetHDC());
#endif //MOBITECH_WIN32

    deltaTime += GetTickCount() - beginFrameTime;
            
    if(currentScene != NULL)
        currentScene->Update(GetTickCount() - beginFrameTime);

    elapsedTime += (float)(GetTickCount() - beginFrameTime)/1000.0f;
    ++fps;
}

void Engine::Run()
{
    #ifdef MOBITECH_WIN32
    MSG msg;
    
    Window::SetActive(true);
    Window::SetRunning(true); 	

    deltaTime      = 0.0;
    fixedTimeStep  = 1.0 / 100.0;

    while (Window::IsRunning())
    {
        while (PeekMessage(&msg, Window::GetHWND(), 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
			{
                Window::SetRunning(false);
                break;
            }
            DispatchMessage(&msg);
        }
        
        beginFrameTime = GetTickCount();

        //if (Input::IsKeyPressed(VK_ESCAPE))
          //  this->Stop();

        if (Window::IsRunning() && Window::IsActive())
        {
            OneFrame();

            if (elapsedTime >= 1.0f)
            {
                char buff[50];
                sprintf_s(buff, "Mobitech FPS: %u, Draw Calls: %u", fps, Renderer::GetInstance()->drawCalls);

                Window::SetWindowTitle(buff);
                fps = 0;
                elapsedTime = 0.0f;
            }
        }
    }

    Window::SetActive(false);
    Window::SetRunning(false);
    #endif //MOBITECH_WIN32
}

void Engine::Release()
{
    rf.ReleaseAll();
    Renderer::GetInstance()->Release();
    Renderer::Free();
    Logger::Free();
}