#include "android_viewer.h"
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#define LOG_TAG "JNIBridge"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" {

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeInit(JNIEnv* env, jobject thiz, jstring assetPath) {
    const char* path = env->GetStringUTFChars(assetPath, nullptr);
    LOGI("nativeInit: %s", path);
    AndroidViewer::getInstance().initialize(path);
    env->ReleaseStringUTFChars(assetPath, path);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeResize(JNIEnv* env, jobject thiz, jint width, jint height) {
    AndroidViewer::getInstance().resize(width, height);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeStep(JNIEnv* env, jobject thiz) {
    AndroidViewer::getInstance().step();
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeDestroy(JNIEnv* env, jobject thiz) {
    AndroidViewer::getInstance().destroy();
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeTouchEvent(JNIEnv* env, jobject thiz,
                                                     jint pointerId, jfloat x, jfloat y, jint action) {
    AndroidViewer::getInstance().touchEvent(pointerId, x, y, action);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeLoadModel(JNIEnv* env, jobject thiz, jstring path) {
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    AndroidViewer::getInstance().loadModel(cpath);
    env->ReleaseStringUTFChars(path, cpath);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeLoadMotion(JNIEnv* env, jobject thiz, jstring path) {
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    AndroidViewer::getInstance().loadMotion(cpath);
    env->ReleaseStringUTFChars(path, cpath);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeLoadScene(JNIEnv* env, jobject thiz, jstring path) {
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    AndroidViewer::getInstance().loadScene(cpath);
    env->ReleaseStringUTFChars(path, cpath);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeLoadGobot(JNIEnv* env, jobject thiz, jstring path) {
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    AndroidViewer::getInstance().loadGobot(cpath);
    env->ReleaseStringUTFChars(path, cpath);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeSetOrientation(JNIEnv* env, jobject thiz, jboolean isLandscape) {
    AndroidViewer::getInstance().setOrientation(isLandscape);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeSetFilter(JNIEnv* env, jobject thiz, jint filterIndex) {
    AndroidViewer::getInstance().setFilter(filterIndex);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeSetFilterParam(JNIEnv* env, jobject thiz, jint paramIndex, jfloat value) {
    AndroidViewer::getInstance().setFilterParam(paramIndex, value);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeSetEnvironmentParam(JNIEnv* env, jobject thiz, jint paramIndex, jfloat value) {
    AndroidViewer::getInstance().setEnvironmentParam(paramIndex, value);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeSetBoneTransform(JNIEnv* env, jobject thiz,
                                                          jint boneIndex, jfloat x, jfloat y, jfloat z,
                                                          jfloat rx, jfloat ry, jfloat rz) {
    AndroidViewer::getInstance().setBoneTransform(boneIndex, x, y, z, rx, ry, rz);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativePlayBuiltinAction(JNIEnv* env, jobject thiz, jint actionIndex) {
    AndroidViewer::getInstance().playBuiltinAction(actionIndex);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeStartRecording(JNIEnv* env, jobject thiz) {
    AndroidViewer::getInstance().startRecording();
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeStopRecording(JNIEnv* env, jobject thiz, jstring outputPath) {
    const char* cpath = env->GetStringUTFChars(outputPath, nullptr);
    AndroidViewer::getInstance().stopRecording(cpath);
    env->ReleaseStringUTFChars(outputPath, cpath);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeSetScreenOrientation(JNIEnv* env, jobject thiz, jint orientation) {
    AndroidViewer::getInstance().setScreenOrientation(orientation);
}

} // extern "C"