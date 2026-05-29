#include "egl_core.h"
#include <android/log.h>

#define LOG_TAG "EGLCore"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

EGLCore::EGLCore()
    : mDisplay(EGL_NO_DISPLAY)
    , mContext(EGL_NO_CONTEXT)
    , mSurface(EGL_NO_SURFACE)
    , mConfig(nullptr)
    , mWindow(nullptr)
    , mWidth(0)
    , mHeight(0)
{
}

EGLCore::~EGLCore()
{
    destroy();
}

bool EGLCore::initialize(ANativeWindow* window)
{
    mWindow = window;

    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (mDisplay == EGL_NO_DISPLAY) {
        LOGE("Failed to get EGL display");
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(mDisplay, &major, &minor)) {
        LOGE("Failed to initialize EGL");
        return false;
    }
    LOGI("EGL version: %d.%d", major, minor);

    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

    EGLint numConfigs;
    if (!eglChooseConfig(mDisplay, attribs, &mConfig, 1, &numConfigs) || numConfigs == 0) {
        LOGE("Failed to choose EGL config");
        return false;
    }

    mSurface = eglCreateWindowSurface(mDisplay, mConfig, window, nullptr);
    if (mSurface == EGL_NO_SURFACE) {
        checkEGLError("eglCreateWindowSurface");
        return false;
    }

    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    mContext = eglCreateContext(mDisplay, mConfig, EGL_NO_CONTEXT, contextAttribs);
    if (mContext == EGL_NO_CONTEXT) {
        checkEGLError("eglCreateContext");
        return false;
    }

    if (!eglMakeCurrent(mDisplay, mSurface, mSurface, mContext)) {
        checkEGLError("eglMakeCurrent");
        return false;
    }

    EGLint w, h;
    eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &w);
    eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &h);
    mWidth = w;
    mHeight = h;

    LOGI("EGL initialized: %dx%d", mWidth, mHeight);
    return true;
}

void EGLCore::destroy()
{
    if (mDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (mSurface != EGL_NO_SURFACE) {
            eglDestroySurface(mDisplay, mSurface);
            mSurface = EGL_NO_SURFACE;
        }
        if (mContext != EGL_NO_CONTEXT) {
            eglDestroyContext(mDisplay, mContext);
            mContext = EGL_NO_CONTEXT;
        }
        eglTerminate(mDisplay);
        mDisplay = EGL_NO_DISPLAY;
    }
    mConfig = nullptr;
    mWindow = nullptr;
}

bool EGLCore::swapBuffers()
{
    if (mDisplay == EGL_NO_DISPLAY || mSurface == EGL_NO_SURFACE) {
        return false;
    }
    return eglSwapBuffers(mDisplay, mSurface);
}

bool EGLCore::checkEGLError(const char* tag)
{
    EGLint error = eglGetError();
    if (error != EGL_SUCCESS) {
        LOGE("EGL error [%s]: 0x%x", tag, error);
        return true;
    }
    return false;
}