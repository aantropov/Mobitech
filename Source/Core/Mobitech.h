#ifndef MOBITECH_H
#define MOBITECH_H

using namespace std;

#ifdef MOBITECH_ANDROID

#include <jni.h>
#include <errno.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>

#include <android/asset_manager_jni.h>
#include <android/sensor.h>
#include <android/log.h>

#else //MOBITECH_WIN32

#define GL_GLEXT_PROTOTYPES
#define GL3_PROTOTYPES 1
#include "windows.h"

#include <GL\glew.h>
#include <GL\wglew.h>

//#include "mygui 3.2.0/GL/glew.h"
//#include <GL\gl.h>

#include "GL\glext.h"
#include "GL\wglext.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")

#endif //MOBITECH_WIN32

#include "tinyxml\tinyxml.h"
#include "lodepng\lodepng.h"
#include "math\mathgl.h"
#include "font\bmfont.h"
#include "resources.h"
#include "renderer.h"
#include "physics.h"
//#include "2dgameanimation\Animation.h"

#ifdef MOBITECH_ANDROID
#include <tr1/memory>
using namespace std::tr1;
#else 
#include <memory>
#endif

extern void GameMain();

class Scene 
{
public:
    virtual void DrawFrame() = 0;
    virtual void Update(double delta_seconds) = 0;
};

class Engine : public Singleton<Engine>
{
    unsigned int fps;
    float elapsed_time;
    
    std::tr1::shared_ptr<Scene> current_scene;
        
    unsigned long long delta_time, begin_frame_time, last_update_time;  
    Engine(): APK_ROOT("") {}

public:
  
    string APK_ROOT;
    static ResourceFactory main_resource_factory;
    static Engine* GetInstance();
    static unsigned long long GetTimeMS();
    string GetApkRoot() const { return APK_ROOT; }

    bool Initialize();
    void Run();
    void OneFrame();
    void Stop();
    void Release();

    void SetScene(std::tr1::shared_ptr<Scene> scene) { current_scene = scene; }
    shared_ptr<Scene> GetScene() const { return current_scene; }
};

#endif //MOBITECH_H
