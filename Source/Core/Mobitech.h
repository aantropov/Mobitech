#ifndef MOBITECH_H
#define MOBITECH_H

using namespace std;

#ifdef MOBITECH_ANDROID
#include <jni.h>
#include <errno.h>

#include <EGL/egl.h
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
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
