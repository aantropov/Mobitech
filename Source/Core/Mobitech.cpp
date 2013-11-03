#include "Mobitech.h"
#include "Renderer.h"

ResourceFactory Engine::main_resource_factory;

Engine* Engine::GetInstance()
{
    if(instance == NULL)
        instance = new Engine();        
    return instance;
}

bool Engine::Initialize()
{
    elapsed_time = 0;
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

    if(current_scene != NULL)
        current_scene->DrawFrame();

    int currentTick = 0;
#ifdef MOBITECH_WIN32
    SwapBuffers(Window::GetHDC());
    currentTick = GetTickCount();
#else if MOBITECH_ANDROID
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    currentTick = ts.tv_nsec;
#endif //MOBITECH_WIN32    
    
    delta_time +=  currentTick - begin_frame_time;

    if(current_scene != NULL)
        current_scene->Update(currentTick - begin_frame_time);

    elapsed_time += (float)(currentTick - begin_frame_time)/1000.0f;
    ++fps;
}

void Engine::Run()
{
    #ifdef MOBITECH_WIN32
    MSG msg;
    
    Window::SetActive(true);
    Window::SetRunning(true); 	

    delta_time      = 0.0;
    fixed_time_step  = 1.0 / 100.0;

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
        
        begin_frame_time = GetTickCount();

        //if (Input::IsKeyPressed(VK_ESCAPE))
          //  this->Stop();

        if (Window::IsRunning() && Window::IsActive())
        {
            OneFrame();

            if (elapsed_time >= 1.0f)
            {
                char buff[50];
                sprintf_s(buff, "Mobitech FPS: %u, Draw Calls: %u", fps, Renderer::GetInstance()->drawCalls);

                Window::SetWindowTitle(buff);
                fps = 0;
                elapsed_time = 0.0f;
            }
        }
    }

    Window::SetActive(false);
    Window::SetRunning(false);
    #endif //MOBITECH_WIN32
}

void Engine::Release()
{
    main_resource_factory.ReleaseAll();
    Renderer::GetInstance()->Release();
    Renderer::Free();
    Logger::Free();
}