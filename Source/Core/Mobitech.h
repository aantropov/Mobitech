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

#endif //MOBITECH_ANDROID

#ifdef MOBITECH_WIN32
#endif //MOBITECH_WIN32

#include "resources.h"
#include "math\mathgl.h"

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
