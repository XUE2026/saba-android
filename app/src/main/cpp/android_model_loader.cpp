#include <jni.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <memory>

#define LOG_TAG "SabaModelLoader"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#include <Saba/Model/MMD/PMXFile.h>
#include <Saba/Model/MMD/PMDFile.h>
#include <Saba/Model/MMD/VMDFile.h>
#include <Saba/Model/MMD/VPDFile.h>
#include <Saba/Model/MMD/PMXModel.h>
#include <Saba/Model/MMD/VMDAnimation.h>
#include <Saba/Model/MMD/ModelCache.h>
#include <Saba/Model/OBJ/OBJModel.h>
#include <Saba/Model/XFile/XFileModel.h>
#include <Saba/Base/Path.h>
#include <Saba/Base/File.h>

namespace saba {
namespace android {

class AndroidModelLoader {
public:
    AndroidModelLoader() = default;
    ~AndroidModelLoader() = default;

    bool LoadPMX(const std::string& path) {
        LOGI("Loading PMX: %s", path.c_str());
        PMXFile pmx;
        if (!pmx.Load(path)) {
            LOGE("Failed to load PMX: %s", path.c_str());
            return false;
        }
        LOGI("PMX loaded: %zu vertices, %zu faces", pmx.m_vertices.size(), pmx.m_indices.size() / 3);
        return true;
    }

    bool LoadVMD(const std::string& path) {
        LOGI("Loading VMD: %s", path.c_str());
        VMDFile vmd;
        if (!vmd.Load(path)) {
            LOGE("Failed to load VMD: %s", path.c_str());
            return false;
        }
        LOGI("VMD loaded: %zu bone frames, %zu camera frames",
             vmd.m_boneFrames.size(), vmd.m_cameraFrames.size());
        return true;
    }
};

} // namespace android
} // namespace saba