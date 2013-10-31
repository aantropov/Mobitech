#include "..\..\Core\Mobitech.h"

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
};

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
    GameMain();    
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj)
{
    Engine::GetInstance()->OneFrame();
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchDown(JNIEnv * env, jobject obj, jint x, jint y)
{
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchUp(JNIEnv * env, jobject obj, jint x, jint y)
{
}