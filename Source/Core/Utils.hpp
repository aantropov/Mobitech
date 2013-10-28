#ifndef _UTILS_H_
#define _UTILS_H_

using namespace std;

#include <string>
#include <stdio.h>
#include <windows.h>

const string LOG_FILE_NAME = "log.html";

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
T* Singleton<T>:: instance = nullptr;

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

    void LOG(string text, LOG_TYPE msg_type = LOG_TYPE::LT_INFO)
    {        
        if(msg_type == LOG_TYPE::LT_WARNING)
            text = "<font color=\"orange\">" + text + "</font>";
        else if(msg_type == LOG_TYPE::LT_ERROR)
            text = "<font color=\"red\">" + text + "</font>";
        else if(msg_type == LOG_TYPE::LT_INFO)
           text = "<font color=\"black\">" + text + "</font>";
        
        #ifdef MOBITECH_WIN32
        fprintf(fLog, text.c_str());
        #endif //MOBITECH_WIN32

        #ifdef MOBITECH_ANDROID
         if(msg_type == LOG_TYPE::LT_ERROR)
            __android_log_print(ANDROID_LOG_ERROR, "Mobitech", text);
         else
            __android_log_print(ANDROID_LOG_INFO, "Mobitech", text);
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
    
    static void Message(string text, LOG_TYPE msg_type = LOG_TYPE::LT_INFO)
    {
        GetInstance()->LOG(text, msg_type);
    }
    
    ~Logger()
    {
        #ifdef MOBITECH_WIN32
        fclose(fLog);
        #endif //MOBITECH_WIN32
    }
};

#endif //_UTILS_H_