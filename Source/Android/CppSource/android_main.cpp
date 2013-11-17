#include "..\..\Core\Mobitech.h"

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_initResourceFactory(JNIEnv * env, jclass jclazz, jobject java_asset_manager, jstring apk_path);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchDown(JNIEnv * env, jobject obj, jint x, jint y, jint touch_id);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchUp(JNIEnv * env, jobject obj, jint x, jint y, jint touch_id);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchMove(JNIEnv * env, jobject obj, jint x, jint y, jint touch_id);
};

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_initResourceFactory(JNIEnv * env, jclass jclazz, jobject java_asset_manager, jstring apk_path)
{
    Engine* eng = Engine::GetInstance();    
    jboolean isCopy;
    eng->main_resource_factory.asset_manager = AAssetManager_fromJava(env, java_asset_manager);
    eng->APK_ROOT = env->GetStringUTFChars(apk_path, &isCopy);
    //eng->main_resource_factory.apk_archive = zip_open(eng->GetApkRoot().c_str(), 0, NULL);
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

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchDown(JNIEnv * env, jobject obj, jint x, jint y, jint touch_id)
{
    Input::GetInstance()->OnTouchDown(x, y, touch_id);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchUp(JNIEnv * env, jobject obj, jint x, jint y, jint touch_id)
{
    Input::GetInstance()->OnTouchUp(x, y, touch_id);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchMove(JNIEnv * env, jobject obj, jint x, jint y, jint touch_id)
{    
    Input::GetInstance()->OnTouchMove(x, y, touch_id);
}


