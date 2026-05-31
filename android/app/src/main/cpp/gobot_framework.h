//
// Copyright (c) 2026 xueyixuan2026
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//
// Gobot Framework Integration - Android MMD Viewer
// 

#ifndef GOBOT_FRAMEWORK_H_
#define GOBOT_FRAMEWORK_H_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace gobot {

// Forward declarations
struct ActionData;
struct SceneData;
struct ProjectData;

// Core types
using BoneTransform = std::vector<float>; // [x, y, z, rx, ry, rz]
using KeyFrameMap = std::map<float, BoneTransform>; // time -> transforms
using BoneAnimationMap = std::map<std::string, KeyFrameMap>;

// .gobot action file format
struct ActionData {
    std::string name;
    std::string description;
    float duration;
    BoneAnimationMap bones;
};

// .gobot scene preset format
struct SceneData {
    std::string name;
    std::string description;
    std::string groundType;
    std::vector<float> skyColor;
    struct Light {
        std::vector<float> direction;
        std::vector<float> color;
        float intensity;
    } light;
    float toonIntensity;
};

// .gobot project format
struct ProjectData {
    std::string name;
    std::string version;
    std::string author;
    std::string license;
    std::vector<std::string> actions;
    std::vector<std::string> scenes;
};

// Gobot Framework
class GobotFramework {
public:
    static GobotFramework& getInstance();

    bool initialize(const std::string& dataPath);
    
    // Action management
    bool loadActionFile(const std::string& path);
    bool saveActionFile(const std::string& path, const ActionData& action);
    ActionData* getAction(const std::string& name);
    std::vector<std::string> getActionNames() const;
    
    // Scene management
    bool loadSceneFile(const std::string& path);
    bool saveSceneFile(const std::string& path, const SceneData& scene);
    SceneData* getScene(const std::string& name);
    std::vector<std::string> getSceneNames() const;
    
    // Project management
    bool loadProject(const std::string& path);
    bool saveProject(const std::string& path, const ProjectData& project);
    
    // Execution
    bool executeAction(const std::string& name, float time, 
                       std::function<void(const std::string&, const BoneTransform&)> applyBone);
    bool applyScene(const std::string& name);
    
    // Built-in actions
    void loadBuiltinActions();

private:
    GobotFramework();
    ~GobotFramework();
    GobotFramework(const GobotFramework&) = delete;
    GobotFramework& operator=(const GobotFramework&) = delete;

    std::map<std::string, ActionData> mActions;
    std::map<std::string, SceneData> mScenes;
    ProjectData mCurrentProject;
    std::string mDataPath;
};

} // namespace gobot

#endif // GOBOT_FRAMEWORK_H_