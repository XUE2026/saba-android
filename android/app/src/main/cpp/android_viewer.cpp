#include "android_viewer.h"
#include "filter_system.h"
#include "environment_system.h"
#include "action_recorder.h"
#include "bone_controller.h"

#include <Saba/Base/Log.h>
#include <Saba/Base/Path.h>
#include <Saba/Base/Singleton.h>
#include <Saba/Base/Time.h>

#include <Saba/Model/MMD/PMXModel.h>
#include <Saba/Model/MMD/PMDModel.h>
#include <Saba/Model/MMD/VMDFile.h>

#include <Saba/GL/Model/MMD/GLMMDModel.h>
#include <Saba/GL/Model/MMD/GLMMDModelDrawContext.h>
#include <Saba/GL/Model/MMD/GLMMDModelDrawer.h>
#include <Saba/Viewer/ViewerContext.h>

#include <android/log.h>
#include <GLES3/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <algorithm>

#define LOG_TAG "AndroidViewer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static const float TOUCH_SCALE_ORBIT = 0.005f;
static const float TOUCH_SCALE_DOLLY = 0.01f;
static const float PINCH_DOLLY_FACTOR = 0.005f;
static const float MIN_PINCH_DIST = 10.0f;

AndroidViewer::AndroidViewer()
    : mInitialized(false)
    , mFilterSystem(nullptr)
    , mEnvironmentSystem(nullptr)
    , mActionRecorder(nullptr)
    , mBoneController(nullptr)
    , mTouchCount(0)
    , mLastPinchDist(0.0f)
    , mIsPinching(false)
    , mCurrentFilter(0)
    , mFilterIntensity(1.0f)
    , mOrientation(0)
    , mWidth(0)
    , mHeight(0)
{
    for (int i = 0; i < 10; i++) {
        mTouches[i].id = -1;
        mTouches[i].x = 0.0f;
        mTouches[i].y = 0.0f;
        mTouches[i].active = false;
    }
}

AndroidViewer::~AndroidViewer()
{
    destroy();
}

AndroidViewer& AndroidViewer::getInstance()
{
    static AndroidViewer instance;
    return instance;
}

bool AndroidViewer::initialize(const char* assetPath)
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (mInitialized) {
        LOGI("Already initialized, skipping.");
        return true;
    }

    mAssetPath = assetPath ? assetPath : "";

    LOGI("Initializing AndroidViewer with asset path: %s", mAssetPath.c_str());

    saba::Singleton<saba::Logger>::Get();

    mViewerContext = std::make_unique<saba::ViewerContext>();
    if (!mViewerContext->Initialize()) {
        LOGE("Failed to initialize ViewerContext.");
        return false;
    }

    mDrawContext = std::make_unique<saba::GLMMDModelDrawContext>(mViewerContext.get());

    if (!mViewerContext->GetShadowMap()->InitializeShader(mViewerContext.get())) {
        LOGI("ShadowMap shader initialization skipped (non-critical).");
    }

    mCamera.Initialize(glm::vec3(0.0f), 10.0f);
    mCamera.SetFovY(glm::radians(45.0f));
    mCamera.SetSize(1280.0f, 800.0f);
    mCamera.SetClip(0.1f, 1000.0f);
    mCamera.UpdateMatrix();

    mLight.SetLightColor(glm::vec3(1.0f, 1.0f, 1.0f));
    mLight.SetLightDirection(glm::vec3(0.0f, -1.0f, 1.0f));

    mFilterSystem = std::make_unique<FilterSystem>();
    mEnvironmentSystem = std::make_unique<EnvironmentSystem>();
    mActionRecorder = std::make_unique<ActionRecorder>();
    mBoneController = std::make_unique<BoneController>();

    gobot::GobotFramework::getInstance().initialize(mAssetPath);

    mEnvironmentSystem->initialize();

    mInitialized = true;
    LOGI("AndroidViewer initialized successfully.");
    return true;
}

void AndroidViewer::resize(int width, int height)
{
    std::lock_guard<std::mutex> lock(mMutex);

    mWidth = width;
    mHeight = height;

    if (!mInitialized) {
        return;
    }

    mCamera.SetSize(static_cast<float>(width), static_cast<float>(height));
    mCamera.UpdateMatrix();

    glViewport(0, 0, width, height);

    LOGI("Resized to %dx%d", width, height);
}

void AndroidViewer::step()
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (!mInitialized) {
        return;
    }

    double currentTime = saba::GetTime();
    static double prevTime = currentTime;
    double elapsed = currentTime - prevTime;
    if (elapsed > 0.1) elapsed = 0.016;
    prevTime = currentTime;

    mAnimationTime += elapsed;

    mViewerContext->SetAnimationTime(mAnimationTime);
    mViewerContext->SetElapsedTime(elapsed);
    mViewerContext->SetPlayMode(saba::ViewerContext::PlayMode::Play);
    mViewerContext->SetFrameBufferSize(mWidth, mHeight);
    mViewerContext->SetCamera(mCamera);
    mViewerContext->SetLight(mLight);

    for (auto& drawer : mModelDrawers) {
        drawer->Update(mViewerContext.get());
    }

    glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    for (auto& drawer : mModelDrawers) {
        drawer->Draw(mViewerContext.get());
    }

    if (mEnvironmentSystem) {
        const auto& view = mCamera.GetViewMatrix();
        const auto& proj = mCamera.GetProjectionMatrix();
        mEnvironmentSystem->render(view, proj, mLight.GetLightDirection());
    }
}

void AndroidViewer::destroy()
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (!mInitialized) {
        return;
    }

    LOGI("Destroying AndroidViewer...");

    mModelDrawers.clear();

    mDrawContext.reset();

    if (mViewerContext) {
        mViewerContext->Uninitialize();
        mViewerContext.reset();
    }

    mActionRecorder.reset();
    mBoneController.reset();
    mFilterSystem.reset();
    mEnvironmentSystem.reset();

    mInitialized = false;
    mTouchCount = 0;

    LOGI("AndroidViewer destroyed.");
}

void AndroidViewer::touchEvent(int pointerId, float x, float y, int action)
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (!mInitialized) {
        return;
    }

    if (pointerId < 0 || pointerId >= 10) {
        return;
    }

    switch (action) {
        case 0: {
            TouchPoint& tp = mTouches[pointerId];
            tp.id = pointerId;
            tp.x = x;
            tp.y = y;
            tp.active = true;
            mTouchCount++;

            if (mTouchCount == 1) {
                mIsPinching = false;
            } else if (mTouchCount == 2) {
                float dx = mTouches[0].x - mTouches[1].x;
                float dy = mTouches[0].y - mTouches[1].y;
                mLastPinchDist = std::sqrt(dx * dx + dy * dy);
                mIsPinching = true;
            }
            break;
        }
        case 1:
        case 3: {
            TouchPoint& tp = mTouches[pointerId];
            if (tp.active) {
                tp.active = false;
                mTouchCount--;
            }
            if (mTouchCount < 2) {
                mIsPinching = false;
            }
            break;
        }
        case 2: {
            TouchPoint& tp = mTouches[pointerId];
            if (!tp.active) {
                tp.active = true;
                mTouchCount++;
            }

            if (mTouchCount == 1) {
                float dx = x - tp.x;
                float dy = y - tp.y;
                mCamera.Orbit(dx * TOUCH_SCALE_ORBIT, dy * TOUCH_SCALE_ORBIT);
                mCamera.UpdateMatrix();
            } else if (mTouchCount >= 2 && mIsPinching) {
                int t0 = -1, t1 = -1;
                for (int i = 0; i < 10; i++) {
                    if (mTouches[i].active) {
                        if (t0 < 0) t0 = i;
                        else if (t1 < 0) t1 = i;
                    }
                }
                if (t0 >= 0 && t1 >= 0) {
                    float dx = mTouches[t0].x - mTouches[t1].x;
                    float dy = mTouches[t0].y - mTouches[t1].y;
                    float curDist = std::sqrt(dx * dx + dy * dy);

                    if (mLastPinchDist > MIN_PINCH_DIST) {
                        float delta = (mLastPinchDist - curDist) * PINCH_DOLLY_FACTOR;
                        mCamera.Dolly(delta);
                        mCamera.UpdateMatrix();
                    }
                    mLastPinchDist = curDist;
                }
            }

            tp.x = x;
            tp.y = y;
            break;
        }
    }
}

void AndroidViewer::processTouchEvents()
{
}

void AndroidViewer::loadModel(const std::string& path)
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (!mInitialized) {
        LOGE("Cannot load model: viewer not initialized.");
        return;
    }

    LOGI("Loading model: %s", path.c_str());

    std::string mmdDataDir = saba::PathUtil::GetDirectoryName(path);

    std::shared_ptr<saba::MMDModel> mmdModel;

    auto pmxModel = std::make_shared<saba::PMXModel>();
    if (pmxModel->Load(path, mmdDataDir)) {
        mmdModel = pmxModel;
        LOGI("PMX model loaded: %s", path.c_str());
    } else {
        auto pmdModel = std::make_shared<saba::PMDModel>();
        if (pmdModel->Load(path, mmdDataDir)) {
            mmdModel = pmdModel;
            LOGI("PMD model loaded: %s", path.c_str());
        } else {
            LOGE("Failed to load model (PMX/PMD): %s", path.c_str());
            return;
        }
    }

    auto glModel = std::make_shared<saba::GLMMDModel>();
    if (!glModel->Create(mmdModel)) {
        LOGE("Failed to create GLMMDModel.");
        return;
    }

    auto drawer = std::make_shared<saba::GLMMDModelDrawer>(mDrawContext.get(), glModel);
    if (!drawer->Create()) {
        LOGE("Failed to create GLMMDModelDrawer.");
        return;
    }

    drawer->SetBBox(mmdModel->GetBBoxMin(), mmdModel->GetBBoxMax());
    mModelDrawers.push_back(drawer);

    LOGI("Model loaded successfully: %s", path.c_str());
}

void AndroidViewer::loadMotion(const std::string& path)
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (!mInitialized) {
        LOGE("Cannot load motion: viewer not initialized.");
        return;
    }

    LOGI("Loading motion: %s", path.c_str());

    if (mModelDrawers.empty()) {
        LOGE("No model loaded to apply motion.");
        return;
    }

    auto drawer = mModelDrawers.back();
    auto* glModel = drawer->GetModel();
    if (glModel == nullptr) {
        LOGE("Drawer has no model.");
        return;
    }

    saba::VMDFile vmd;
    if (!saba::ReadVMDFile(&vmd, path.c_str())) {
        LOGE("Failed to read VMD file: %s", path.c_str());
        return;
    }

    if (!glModel->LoadAnimation(vmd)) {
        LOGE("Failed to apply animation to model.");
        return;
    }

    mAnimationTime = 0.0;
    LOGI("Motion loaded successfully: %s", path.c_str());
}

void AndroidViewer::loadScene(const std::string& path)
{
    std::lock_guard<std::mutex> lock(mMutex);
    LOGI("Loading scene: %s", path.c_str());
    if (mEnvironmentSystem) {
        mEnvironmentSystem->loadScene(path);
    }
}

void AndroidViewer::loadGobot(const std::string& path)
{
    std::lock_guard<std::mutex> lock(mMutex);
    LOGI("Loading gobot: %s", path.c_str());

    if (gobot::GobotFramework::getInstance().loadActionFile(path)) {
        mActionRecorder->loadAction(path);
        LOGI("Gobot action file loaded: %s", path.c_str());
    }
}

void AndroidViewer::setFilter(int filterIndex)
{
    std::lock_guard<std::mutex> lock(mMutex);
    LOGI("Setting filter to %d", filterIndex);
    mCurrentFilter = filterIndex;
    if (mFilterSystem) {
        mFilterSystem->setFilter(static_cast<FilterSystem::FilterType>(filterIndex));
    }
}

void AndroidViewer::setFilterParam(int paramIndex, float value)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mFilterIntensity = value;
    if (mFilterSystem) {
        mFilterSystem->setIntensity(value);
    }
}

void AndroidViewer::setEnvironmentParam(int paramIndex, float value)
{
    std::lock_guard<std::mutex> lock(mMutex);
    if (mEnvironmentSystem) {
        switch (paramIndex) {
            case 0: mEnvironmentSystem->setBrightness(value); break;
            case 1: mEnvironmentSystem->setContrast(value); break;
            case 2: mEnvironmentSystem->setToonIntensity(value); break;
        }
    }
}

void AndroidViewer::setBoneTransform(int boneIndex, float x, float y, float z,
                                      float rx, float ry, float rz)
{
    std::lock_guard<std::mutex> lock(mMutex);
    if (mBoneController) {
        std::string boneName = "Bone" + std::to_string(boneIndex);
        mBoneController->setBoneTransform(boneName,
            glm::vec3(x, y, z), glm::vec3(rx, ry, rz));
    }
}

void AndroidViewer::playBuiltinAction(int actionIndex)
{
    std::lock_guard<std::mutex> lock(mMutex);
    LOGI("Playing builtin action: %d", actionIndex);
    if (mBoneController) {
        mBoneController->playBuiltinAction(actionIndex, 0.0f);
    }
}

void AndroidViewer::startRecording()
{
    std::lock_guard<std::mutex> lock(mMutex);
    LOGI("Starting recording");
    if (mActionRecorder) {
        mActionRecorder->startRecording();
    }
}

void AndroidViewer::stopRecording(const std::string& outputPath)
{
    std::lock_guard<std::mutex> lock(mMutex);
    LOGI("Stopping recording to: %s", outputPath.c_str());
    if (mActionRecorder) {
        mActionRecorder->stopRecording(outputPath);
    }
}

void AndroidViewer::setOrientation(bool isLandscape)
{
    std::lock_guard<std::mutex> lock(mMutex);
    LOGI("Setting orientation: %s", isLandscape ? "landscape" : "portrait");
}

void AndroidViewer::setScreenOrientation(int orientation)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mOrientation = orientation;
}