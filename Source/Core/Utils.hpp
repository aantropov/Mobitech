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
#else 

#include <GL\glew.h>
#include <GL\wglew.h>

#include "GL\glext.h"
#include "GL\wglext.h"

#endif

#define MAX_UNIFORM_LOCATIONS 1000
#define BUFFER_LENGTH 1000
#define NANOSEC_TO_MILLISEC 0.0001f

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
    Logger::Message("Loading extension \"" #n "\" is failed<br>", LOG_TYPE::LT_ERROR); \
        /*return false;*/ \
    }

#define OPENGL_CHECK_FOR_ERRORS() \
    if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
    { \
        char message[BUFFER_LENGTH]; \
        sprintf_s(message, "OpenGL error 0x%X<br>", (unsigned)g_OpenGLError); \
        Logger::Message(message);  \
    }

#define OPENGL_CALL(expression) \
    { \
        expression; \
        if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
        { \
            char message[BUFFER_LENGTH]; \
            sprintf_s(message,"OpenGL expression \"" #expression "\" error %d<br>", (int)g_OpenGLError); \
            Logger::Message(message); \
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
    Logger::Message("Loading extension \"" #n "\" is failed<br>", LT_ERROR); \
        return false; \
    }
#endif //MOBITECH_RELEASE

#define fatof(a) static_cast<float>(atof(a))

const string LOG_FILE_NAME = "log.html";

#ifdef MOBITECH_WIN32
const string ASSETS_ROOT = "..\\..\\Assets\\";
#endif

#ifdef MOBITECH_ANDROID
const string ASSETS_ROOT = "";
#endif

extern GLuint g_OpenGLError;

enum FRAMEBUFFER_ATTACHMENT 
{
    FB_ATTACHMENT_DEPTH = 0x8D00,
    FB_ATTACHMENT_COLOR0 = 0x8CE0,
    FB_ATTACHMENT_COLOR1 = 0x8CE1,
    FB_ATTACHMENT_COLOR2 = 0x8CE2,
    FB_ATTACHMENT_COLOR3 = 0x8CE3,
    FB_ATTACHMENT_COLOR4 = 0x8CE4,
    FB_ATTACHMENT_COLOR5 = 0x8CE5,
    FB_ATTACHMENT_COLOR6 = 0x8CE6
};

enum BLEND_TYPE
{
    BT_ADDITIVE,
    BT_ALPHA_BLEND
};

enum RESOURCE_TYPE
{
    RT_TEXTURE,
    RT_SHADER,
    RT_SHADER_PROGRAM,
    RT_ANIMATION,
    RT_AUDIO
};

enum SHADER_TYPE
{
    ST_VERTEX = GL_VERTEX_SHADER,
    ST_FRAGMENT = GL_FRAGMENT_SHADER
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
T* Singleton<T>::instance = NULL;

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

    void LOG(const string msg, LOG_TYPE msg_type = LT_INFO)
    {   
        string text = msg;
        #ifdef MOBITECH_WIN32
        if(msg_type == LT_WARNING)
            text = "<font color=\"orange\">" + text + "</font>"+ "<br>";
        else if(msg_type == LT_ERROR)
            text = "<font color=\"red\">" + text + "</font>"+ "<br>";
        else if(msg_type == LT_INFO)
           text = "<font color=\"black\">" + text + "</font>"+ "<br>";

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
    
    static void Message(const string text, LOG_TYPE msg_type = LT_INFO)
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
    virtual void OnTouchDown(int x, int y, unsigned int touch_id) = 0;
    virtual void OnTouchUp(int x, int y, unsigned int touch_id) = 0;
    virtual void OnMove(int x, int y, unsigned int touch_id) = 0;
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

    void OnTouchMove(int x, int y, unsigned int touch_id = 0)
    {
        for(std::list<IInputListener*>::iterator i = listeners.begin(); i != listeners.end(); ++i)
            (*i)->OnMove(x, y, touch_id);
    }

    void OnTouchUp(int x, int y, unsigned int touch_id = 0)
    {
       for(std::list<IInputListener*>::iterator i = listeners.begin(); i != listeners.end(); ++i)
            (*i)->OnTouchUp(x, y, touch_id);
    }

    void OnTouchDown(int x, int y, unsigned int touch_id = 0)
    {
        for(std::list<IInputListener*>::iterator i = listeners.begin(); i != listeners.end(); ++i)
            (*i)->OnTouchDown(x, y, touch_id);
    }

    ~Input() {}
};

struct Vertex
{
    static const int offsetPos = sizeof(vec3) + sizeof(vec3) + sizeof(vec3) + sizeof(vec2);

    vec3 pos;
    vec3 normal;
    vec3 color;
    vec2 texcoord;

    Vertex(): pos(vec3_zero), normal(vec3_zero), texcoord(vec2_zero) {}
    Vertex(vec3 p): pos(p), normal(vec3_zero), texcoord(vec2_zero) {}
    Vertex(vec3 p, vec3 n): pos(p), normal(n), texcoord(vec2_zero) {}
    Vertex(vec3 p, vec3 n, vec2 tc): pos(p), normal(n), texcoord(tc) {}
    Vertex(vec3 p, vec3 n, vec3 c, vec2 tc): pos(p), normal(n), texcoord(tc), color(c) {}
};

class GLObject
{
protected:
    int _id;

public:

    int GetId() const { return _id; }
    bool IsInitialized() const { return _id > 0;}
    virtual bool Instantiate() = 0;

    GLObject(void) { _id = -1; }
    virtual ~GLObject(void) { _id = -1; } 
};

class Buffer: public GLObject
{
    BUFFER_TYPE type;

public:

    virtual void* Lock() const = 0;
    virtual void Unlock() const = 0;

    virtual void* GetPointer() const = 0;
    virtual unsigned int GetNum() = 0;
    virtual void Create(int num) = 0;
    virtual bool Instantiate() = 0;
    virtual void Free() = 0;

    Buffer(void);
    virtual ~Buffer(void) {}
};

#endif //_UTILS_H_