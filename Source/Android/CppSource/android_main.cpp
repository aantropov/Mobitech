#include "..\..\Core\Mobitech.h"

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_initResourceFactory(JNIEnv * env, jclass jclazz, jobject java_asset_manager, jstring apkFilePath);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchDown(JNIEnv * env, jobject obj, jint x, jint y);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchUp(JNIEnv * env, jobject obj, jint x, jint y);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchMove(JNIEnv * env, jobject obj, jint x, jint y);
};

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_initResourceFactory(JNIEnv * env, jclass jclazz, jobject java_asset_manager, jstring apkFilePath)
{
    Engine* eng = Engine::GetInstance();    
    jboolean isCopy;
    eng->APK_ROOT = env->GetStringUTFChars(apkFilePath, &isCopy); 
    eng->main_resource_factory.apk_archive = zip_open(eng->GetApkRoot().c_str(), 0, NULL);
    eng->main_resource_factory.asset_manager = AAssetManager_fromJava(env, java_asset_manager);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
    Renderer::GetInstance()->SetWidth(width);
    Renderer::GetInstance()->SetHeight(height);
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


