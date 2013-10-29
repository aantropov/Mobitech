#ifndef _UTILS_H_
#define _UTILS_H_

using namespace std;

#ifdef MOBITECH_WIN32
#include <windows.h>
#endif // MOBITECH_WIN32

#include <string>
#include <stdio.h>
#include <list>

#ifdef MOBITECH_ANDROID

#include <jni.h>
#include <errno.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <android/sensor.h>
#include <android/log.h>

#define sprintf_s sprintf
#endif

#define MAX_UNIFORM_LOCATIONS 1000
#define MAXCHAR 1000

#ifdef MOBITECH_ANDROID
const string MOBITECH_PLATFORM = "android";
#else
const string MOBITECH_PLATFORM = "win32";
#endif //MOBITECH_

#ifdef MOBITECH_DEBUG
#define OPENGL_GET_PROC(p,n) \
    n = (p)wglGetProcAddress(#n); \
    if (n == NULL) \
    { \
    Logger::Message("Loading extension \"" #n "\" is failed", LOG_TYPE::LT_ERROR); \
        /*return false;*/ \
    }

#define OPENGL_CHECK_FOR_ERRORS() \
    if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
    { \
        char message[MAXCHAR]; \
        sprintf_s(message, "OpenGL error 0x%X", (unsigned)g_OpenGLError); \
        Logger::Message(message, LT_ERROR);  \
    }

#define OPENGL_CALL(expression) \
    { \
        expression; \
        if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
        { \
            char message[MAXCHAR]; \
            sprintf_s(message,"OpenGL expression \"" #expression "\" error %d\n", (int)g_OpenGLError); \
            Logger::Message(message, LT_ERROR); \
        } \
    }
//MOBITECH_DEBUG
#else //MOBITECH_RELEASE
#define OPENGL_CHECK_FOR_ERRORS() ;
//#undef OPENGL_GET_PROC(p,n)
//#undef OPENGL_CALL(expression)

#define OPENGL_CALL(expression) expression;
#define OPENGL_GET_PROC(p,n) \
    n = (p)wglGetProcAddress(#n); \
    if (n == NULL) \
    { \
    Logger::Message("Loading extension \"" #n "\" is failed", LT_ERROR); \
        return false; \
    }
#endif //MOBITECH_RELEASE

const string LOG_FILE_NAME = "log.html";
const string ASSETS_ROOT = "..\\..\\Assets\\";

enum RESOURCE_TYPE
{
    RT_TEXTURE,
    RT_SHADER,
    RT_SHADER_PROGRAM,
    RT_AUDIO
};

enum SHADER_TYPE
{
    ST_VERTEX,
    ST_FRAGMENT
};

enum BUFFER_TYPE
{
    STATIC,
    DYNAMIC
};

enum LOG_TYPE
{
    LT_INFO,
    LT_WARNING,
    LT_ERROR
};

template<typename T>
class Singleton
{
protected:

    static T* instance;
    Singleton(){}

public:

    static T* GetInstance();
    static void Free()
    {
        delete instance;
    }
};

template<typename T>
T* Singleton<T>:: instance = NULL;

class Logger : public Singleton<Logger>
{
protected:
    FILE* fLog;

    Logger() 
    { 
        #ifdef MOBITECH_WIN32
        fopen_s(&fLog, LOG_FILE_NAME.c_str(), "a"); 
        #endif //MOBITECH_WIN32
    }

    void LOG(string text, LOG_TYPE msg_type = LT_INFO)
    {        
        if(msg_type == LT_WARNING)
            text = "<font color=\"orange\">" + text + "</font>";
        else if(msg_type == LT_ERROR)
            text = "<font color=\"red\">" + text + "</font>";
        else if(msg_type == LT_INFO)
           text = "<font color=\"black\">" + text + "</font>";
        
        #ifdef MOBITECH_WIN32
        fprintf(fLog, text.c_str());
        #endif //MOBITECH_WIN32

        #ifdef MOBITECH_ANDROID
         if(msg_type == LT_ERROR)
            __android_log_print(ANDROID_LOG_ERROR, "Mobitech", text.c_str());
         else
            __android_log_print(ANDROID_LOG_INFO, "Mobitech", text.c_str());
        #endif //MOBITECH_ANDROID
    }

public:
    static const string ULOG_FILE_PATH;
    static Logger* GetInstance()
    {
        if(instance == NULL)
        {
            instance = new Logger();
        }
        return instance;
    }
    
    static void Message(string text, LOG_TYPE msg_type = LT_INFO)
    {
        GetInstance()->LOG(text, msg_type);
    }

    static void Message(LOG_TYPE msg_type, const char *msg, ...)
    {
        GetInstance()->LOG(msg, msg_type);
    }

    ~Logger()
    {
        #ifdef MOBITECH_WIN32
        fclose(fLog);
        #endif //MOBITECH_WIN32
    }
};

class IInputListener
{
public:
    virtual void OnTouchDown(int x, int y) = 0;
    virtual void OnTouchUp(int x, int y) = 0;
    virtual void OnMove(int x, int y) = 0;
};

class Input: public Singleton<Input>
{
protected:

    Input() {}
    std::list<IInputListener*> listeners;  

public:
    static const string ULOG_FILE_PATH;
    static Input* GetInstance()
    {
        if(instance == NULL)
        {
            instance = new Input();
        }
        return instance;
    }
    
    void Register(IInputListener* listener)
    {
        GetInstance()->listeners.push_back(listener);
    }

    void Unregister(IInputListener* listener)
    {
        GetInstance()->listeners.remove(listener);
    }

    void Move(int x, int y)
    {
        for(std::list<IInputListener*>::iterator i = listeners.begin(); i != listeners.end(); ++i)
            (*i)->OnMove(x, y);
    }

    void OnTouchUp(int x, int y)
    {
       for(std::list<IInputListener*>::iterator i = listeners.begin(); i != listeners.end(); ++i)
            (*i)->OnTouchUp(x, y);
    }

    void OnTouchDown(int x, int y)
    {
        for(std::list<IInputListener*>::iterator i = listeners.begin(); i != listeners.end(); ++i)
            (*i)->OnTouchDown(x, y);
    }

    ~Input() {}
};

#endif //_UTILS_H_