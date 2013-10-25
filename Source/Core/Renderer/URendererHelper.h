#pragma once

#include "windows.h"
#include "..\Utils\ULogger.h"
#include "..\Utils\enum.h"


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

#endif //MOBITECH_ANDROID

#include <algorithm>
#include <cctype>
#include <cstdlib>

// Global variable for error reporting
extern GLenum g_OpenGLError;

// Safe OpenGL functions calling
#ifdef UE_DEBUG

// Get function address from the driver
#define OPENGL_GET_PROC(p,n) \
    n = (p)wglGetProcAddress(#n); \
    if (n == NULL) \
    { \
    ULogger::GetInstance()->Message("Loading extension \"" #n "\" is failed", ULOG_MSG_ERROR); \
        /*return false;*/ \
    }

#define DEVIL_CHECK_FOR_ERRORS() \
    if ((g_DevILError = ilGetError()) != IL_NO_ERROR) \
    { \
        char message[UE_MAXCHAR]; \
        sprintf_s(message, "DevIL error 0x%X", (unsigned)g_DevILError); \
        ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR, ULOG_OUT_MSG); \
    }

#define OPENGL_CHECK_FOR_ERRORS() \
    if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
    { \
        char message[UE_MAXCHAR]; \
        sprintf_s(message, "OpenGL error 0x%X", (unsigned)g_OpenGLError); \
        ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR, ULOG_OUT_MSG); \
    }

#define OPENGL_CALL(expression) \
    { \
        expression; \
        if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
        { \
            char message[UE_MAXCHAR]; \
            sprintf_s(message,"OpenGL expression \"" #expression "\" error %d\n", (int)g_OpenGLError); \
            ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR, ULOG_OUT_MSG); \
        } \
    }

#define DEVIL_CALL(expression) \
    { \
        expression; \
        if ((g_DevILError = ilGetError()) != IL_NO_ERROR) \
        { \
            char message[UE_MAXCHAR]; \
            sprintf_s(message,"DevIL expression \"" #expression "\" error %d\n", (int)g_DevILError); \
            ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR, ULOG_OUT_MSG); \
        } \
    }
#endif //UE_DEBUG

#ifdef UE_RELEASE
#define DEVIL_CHECK_FOR_ERRORS() ;
#define OPENGL_CHECK_FOR_ERRORS() ;
//#undef OPENGL_GET_PROC(p,n)
//#undef OPENGL_CALL(expression)

#define OPENGL_CALL(expression) expression;
#define OPENGL_GET_PROC(p,n) \
    n = (p)wglGetProcAddress(#n); \
    if (n == NULL) \
    { \
    ULogger::GetInstance()->Message("Loading extension \"" #n "\" is failed", ULOG_MSG_ERROR); \
        return false; \
    }

#define DEVIL_CALL(expression) expression;
#endif //UE_RELEASE