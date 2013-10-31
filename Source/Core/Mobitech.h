#ifndef MOBITECH_H
#define MOBITECH_H

using namespace std;

#ifdef MOBITECH_ANDROID

#include <jni.h>
#include <errno.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>

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
#include "libzip\zip.h"
#include "math\mathgl.h"
#include "resources.h"
#include "renderer.h"

extern void GameMain();

class Scene 
{
public:
    virtual void DrawFrame() = 0;
    virtual void Update(float delta) = 0;
};

class Engine : public Singleton<Engine>
{
    unsigned int fps;
    float elapsedTime;
    Scene* currentScene;
        
    double deltaTime, beginFrameTime, fixedTimeStep;  
    Engine(): APK_ROOT("") { currentScene = NULL; }

public:
  
    string APK_ROOT;
    static ResourceFactory  rf;
    static Engine* GetInstance();

    string GetApkRoot() const { return APK_ROOT; }

    bool Initialize();
    void Run();
    void OneFrame();
    void Stop();
    void Release();

    void SetScene(Scene *scene) { currentScene = scene; }
    Scene* GetScene() const { return currentScene; }
};

#endif //MOBITECH_H
