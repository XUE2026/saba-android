#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#include <string>
#include <vector>
#include <memory>
#include <mutex>

#define LOG_TAG "SabaNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

static std::mutex sTouchMutex;
static std::vector<float> sTouchX;
static std::vector<float> sTouchY;
static std::vector<int> sTouchIds;
static int sTouchAction = -1;

static int sScreenWidth = 0;
static int sScreenHeight = 0;
static int sOrientation = 0;
static bool sInitialized = false;
static bool sPaused = false;

static AAssetManager* sAssetManager = nullptr;

extern "C" {

JNIEXPORT void JNICALL
Java_com_saba_viewer_SabaNativeSurfaceView_nativeOnSurfaceCreated(JNIEnv* env, jclass clazz) {
    LOGI("nativeOnSurfaceCreated");

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    sInitialized = true;
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_SabaNativeSurfaceView_nativeOnSurfaceChanged(JNIEnv* env, jclass clazz,
                                                                   jint width, jint height) {
    LOGI("nativeOnSurfaceChanged: %dx%d", width, height);

    sScreenWidth = width;
    sScreenHeight = height;

    glViewport(0, 0, width, height);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_SabaNativeSurfaceView_nativeOnDrawFrame(JNIEnv* env, jclass clazz) {
    if (sPaused) {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_SabaNativeSurfaceView_nativeOnTouchEvent(JNIEnv* env, jclass clazz,
                                                               jint action, jint pointerCount,
                                                               jintArray ids, jfloatArray x,
                                                               jfloatArray y) {
    std::lock_guard<std::mutex> lock(sTouchMutex);

    sTouchAction = action;

    jint* idsPtr = env->GetIntArrayElements(ids, nullptr);
    jfloat* xPtr = env->GetFloatArrayElements(x, nullptr);
    jfloat* yPtr = env->GetFloatArrayElements(y, nullptr);

    sTouchIds.resize(pointerCount);
    sTouchX.resize(pointerCount);
    sTouchY.resize(pointerCount);

    for (int i = 0; i < pointerCount; i++) {
        sTouchIds[i] = idsPtr[i];
        sTouchX[i] = xPtr[i];
        sTouchY[i] = yPtr[i];
    }

    env->ReleaseIntArrayElements(ids, idsPtr, JNI_ABORT);
    env->ReleaseFloatArrayElements(x, xPtr, JNI_ABORT);
    env->ReleaseFloatArrayElements(y, yPtr, JNI_ABORT);
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_SabaNativeSurfaceView_nativeOnSurfaceDestroyed(JNIEnv* env, jclass clazz) {
    LOGI("nativeOnSurfaceDestroyed");
    sInitialized = false;
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_SabaNativeSurfaceView_nativeOnResume(JNIEnv* env, jclass clazz) {
    LOGI("nativeOnResume");
    sPaused = false;
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_SabaNativeSurfaceView_nativeOnPause(JNIEnv* env, jclass clazz) {
    LOGI("nativeOnPause");
    sPaused = true;
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_SabaNativeSurfaceView_nativeOnDestroy(JNIEnv* env, jclass clazz) {
    LOGI("nativeOnDestroy");
    sInitialized = false;
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_SabaNativeSurfaceView_nativeOnConfigurationChanged(JNIEnv* env, jclass clazz,
                                                                          jint orientation) {
    LOGI("nativeOnConfigurationChanged: %d", orientation);
    sOrientation = orientation;
}

JNIEXPORT jboolean JNICALL
Java_com_saba_viewer_SabaNativeSurfaceView_nativeIsInitialized(JNIEnv* env, jclass clazz) {
    return sInitialized ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_MainActivity_nativeSetAssetManager(JNIEnv* env, jclass clazz,
                                                         jobject assetManager) {
    sAssetManager = AAssetManager_fromJava(env, assetManager);
    LOGI("AssetManager set: %p", sAssetManager);
}

} // extern "C"