#include "Mobitech.h"
#include "Renderer.h"
#include <time.h>

#ifdef MOBITECH_WIN32
#include <sys/timeb.h>
#include <windows.h>
#endif //MOBITECH_WIN32

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
    return Renderer::GetInstance()->Initialize();
}

void Engine::Stop()
{
    Window::SetRunning(false);
}

unsigned long long Engine::GetTimeMS()
{

#ifdef MOBITECH_WIN32
   SYSTEMTIME time;
   GetSystemTime(&time);
   return (((time.wDay*24 + time.wHour)*60 + time.wMinute)*60 + time.wSecond)*1000 + time.wMilliseconds;
#endif

#ifdef MOBITECH_ANDROID
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return ((unsigned long long)now.tv_sec)*1000LL + ((unsigned long long)now.tv_nsec)/1000000LL;
    
    /*struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000LL + tv.tv_usec / 1000LL;*/
#endif //MOBITECH_WIN32    
}

void Engine::OneFrame()
{
    begin_frame_time = GetTimeMS();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
	Renderer::GetInstance()->draw_calls = 0;

    if(current_scene.get() != NULL)
        current_scene->DrawFrame();

    unsigned long long current_tick = 0;
#ifdef MOBITECH_WIN32
    SwapBuffers(Window::GetHDC());
#endif //MOBITECH_WIN32

    current_tick = GetTimeMS();
    delta_time += current_tick - begin_frame_time;

    double delta = ((double)(current_tick - begin_frame_time))/1000.0;
    
    Physics::GetInstance()->Update(delta);

    if(current_scene.get() != NULL)
        current_scene->Update(delta);

    elapsed_time += ((float)(current_tick - begin_frame_time))/1000.0f;
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
        
        begin_frame_time = GetTimeMS();

        //if (Input::IsKeyPressed(VK_ESCAPE))
          //  this->Stop();

        if (Window::IsRunning() && Window::IsActive())
        {
            OneFrame();

            if (elapsed_time >= 1.0f)
            {
                char buff[50];
                sprintf_s(buff, "Mobitech FPS: %u, Draw Calls: %u", fps, Renderer::GetInstance()->draw_calls);

                Window::SetWindowTitle(buff);
                fps = 0;
                elapsed_time = 0.0f;
            }
        }
    }

    Window::SetActive(false);
    Window::SetRunning(false);
    #endif //MOBITECH_WIN32

#ifdef MOBITECH_ANDROID
    delta_time = 0.0;
    fixed_time_step = 1.0/ 100.0;    
#endif // MOBITECH_ANDROID
}

void Engine::Release()
{
    main_resource_factory.ReleaseAll();
    Renderer::GetInstance()->Release();
    Renderer::Free();
    Logger::Free();
}