#ifndef ANDROID_VIEWER_H_
#define ANDROID_VIEWER_H_

#include "egl_core.h"

#include <Saba/Viewer/Camera.h>
#include <Saba/Viewer/Light.h>
#include <Saba/GL/Model/MMD/GLMMDModel.h>
#include <Saba/GL/Model/MMD/GLMMDModelDrawContext.h>

#include <memory>
#include <string>
#include <mutex>
#include <vector>

class FilterSystem;
class EnvironmentSystem;
class ActionRecorder;
class BoneController;

class AndroidViewer {
public:
    static AndroidViewer& getInstance();

    bool initialize(const char* assetPath);
    void resize(int width, int height);
    void step();
    void destroy();

    void touchEvent(int pointerId, float x, float y, int action);

    void loadModel(const std::string& path);
    void loadMotion(const std::string& path);
    void loadScene(const std::string& path);
    void loadGobot(const std::string& path);

    void setFilter(int filterIndex);
    void setFilterParam(int paramIndex, float value);
    void setEnvironmentParam(int paramIndex, float value);
    void setBoneTransform(int boneIndex, float x, float y, float z, float rx, float ry, float rz);
    void playBuiltinAction(int actionIndex);
    void startRecording();
    void stopRecording(const std::string& outputPath);
    void setOrientation(bool isLandscape);
    void setScreenOrientation(int orientation);

    bool isInitialized() const { return mInitialized; }

private:
    AndroidViewer();
    ~AndroidViewer();
    AndroidViewer(const AndroidViewer&) = delete;
    AndroidViewer& operator=(const AndroidViewer&) = delete;

    void processTouchEvents();

    bool mInitialized;
    std::string mAssetPath;

    saba::Camera mCamera;
    saba::Light mLight;
    std::unique_ptr<saba::GLMMDModelDrawContext> mDrawContext;
    std::vector<std::shared_ptr<saba::GLMMDModel>> mGLModels;

    std::unique_ptr<FilterSystem> mFilterSystem;
    std::unique_ptr<EnvironmentSystem> mEnvironmentSystem;
    std::unique_ptr<ActionRecorder> mActionRecorder;
    std::unique_ptr<BoneController> mBoneController;

    std::mutex mMutex;

    struct TouchPoint {
        int id;
        float x;
        float y;
        bool active;
    };
    TouchPoint mTouches[10];
    int mTouchCount;

    double mAnimationTime;
    float mLastPinchDist;
    bool mIsPinching;

    int mCurrentFilter;
    float mFilterIntensity;
    int mOrientation;
    float mDensity;
    int mWidth;
    int mHeight;
};

#endif // ANDROID_VIEWER_H_