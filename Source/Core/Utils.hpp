#ifndef _UTILS_H_
#define _UTILS_H_

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

#endif //_UTILS_H_