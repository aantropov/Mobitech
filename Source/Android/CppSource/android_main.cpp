#include "..\..\Core\Mobitech.h"

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_initResourceFactory(JNIEnv * env, jobject obj, jstring apkFilePath);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchDown(JNIEnv * env, jobject obj, jint x, jint y);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchUp(JNIEnv * env, jobject obj, jint x, jint y);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchMove(JNIEnv * env, jobject obj, jint x, jint y);
};

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_initResourceFactory(JNIEnv * env, jobject obj, jstring apkFilePath)
{
    /*Engine* eng = Engine::GetInstance();

    jboolean isCopy;
    eng->APK_ROOT = env->GetStringUTFChars(apkFilePath, &isCopy); 
    eng->rf.APKArchive = zip_open(eng->GetApkRoot().c_str(), 0, NULL); */
}

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
    Input::GetInstance()->OnTouchMove(x,y);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchUp(JNIEnv * env, jobject obj, jint x, jint y)
{
    Input::GetInstance()->OnTouchUp(x,y);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchMove(JNIEnv * env, jobject obj, jint x, jint y)
{
    Input::GetInstance()->OnTouchDown(x,y);
}