#ifndef EGL_CORE_H_
#define EGL_CORE_H_

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

class EGLCore {
public:
    EGLCore();
    ~EGLCore();

    bool initialize(ANativeWindow* window);
    void destroy();
    bool swapBuffers();

    EGLDisplay getDisplay() const { return mDisplay; }
    EGLContext getContext() const { return mContext; }
    EGLSurface getSurface() const { return mSurface; }
    EGLConfig getConfig() const { return mConfig; }

    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }

    bool checkEGLError(const char* tag);

private:
    EGLDisplay mDisplay;
    EGLContext mContext;
    EGLSurface mSurface;
    EGLConfig mConfig;
    ANativeWindow* mWindow;
    int mWidth;
    int mHeight;
};

#endif // EGL_CORE_H_