#ifndef MOBITECH_H
#define MOBITECH_H

using namespace std;

#ifdef MOBITECH_ANDROID

#include <jni.h>
#include <errno.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <android/sensor.h>
#include <android/log.h>
//#include <android_native_app_glue.h>

#else //MOBITECH_WIN32

#define GL_GLEXT_PROTOTYPES
#define GL3_PROTOTYPES 1

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
#include "resources.h"


class Engine 
{
    unsigned int fps;
    float elapsedTime;

public:
    
    static ResourceFactory  rf;
    
    bool Initialize();
    void Run();
    void Stop();
    void Release();
};

#endif //MOBITECH_H
