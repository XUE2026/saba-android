#include "action_recorder.h"
#include <android/log.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

#define LOG_TAG "ActionRecorder"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static std::string quatToString(const glm::quat& q) {
    std::ostringstream oss;
    oss << q.w << "," << q.x << "," << q.y << "," << q.z;
    return oss.str();
}

static glm::quat stringToQuat(const std::string& str) {
    glm::quat q;
    sscanf(str.c_str(), "%f,%f,%f,%f", &q.w, &q.x, &q.y, &q.z);
    return q;
}

static std::string vec3ToString(const glm::vec3& v) {
    std::ostringstream oss;
    oss << v.x << "," << v.y << "," << v.z;
    return oss.str();
}

static glm::vec3 stringToVec3(const std::string& str) {
    glm::vec3 v;
    sscanf(str.c_str(), "%f,%f,%f", &v.x, &v.y, &v.z);
    return v;
}

ActionRecorder::ActionRecorder()
    : mIsRecording(false)
    , mIsPlaying(false)
    , mRecordTime(0.0f)
    , mPlayTime(0.0f)
    , mCurrentSegment(0) {
    LOGI("ActionRecorder created");
}

ActionRecorder::~ActionRecorder() {
    stopAction();
}

void ActionRecorder::startRecording() {
    mIsRecording = true;
    mRecordTime = 0.0f;
    mSegments.clear();

    ActionSegment initialSegment;
    initialSegment.name = "Segment_0";
    initialSegment.gapTime = 0.0f;
    mSegments.push_back(initialSegment);

    mCurrentSegment = 0;
    LOGI("Recording started");
}

void ActionRecorder::stopRecording(const std::string& outputPath) {
    if (!mIsRecording) return;
    mIsRecording = false;
    LOGI("Recording stopped, saving to: %s", outputPath.c_str());

    exportToGobot(outputPath);
}

void ActionRecorder::recordFrame(const std::map<std::string, glm::vec3>& bonePositions,
                                  const std::map<std::string, glm::quat>& boneRotations,
                                  float deltaTime) {
    if (!mIsRecording) return;

    mRecordTime += deltaTime;
    mCurrentBonePositions = bonePositions;
    mCurrentBoneRotations = boneRotations;

    ActionSegment& currentSegment = mSegments[mCurrentSegment];

    for (const auto& bp : bonePositions) {
        const std::string& boneName = bp.first;
        BoneKeyframe kf;
        kf.position = bp.second;
        auto it = boneRotations.find(boneName);
        if (it != boneRotations.end()) {
            kf.rotation = it->second;
        } else {
            kf.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        }
        kf.time = mRecordTime;
        currentSegment.boneKeyframes[boneName].push_back(kf);
    }
}

void ActionRecorder::addSegment() {
    if (!mIsRecording) return;

    ActionSegment newSegment;
    newSegment.name = "Segment_" + std::to_string(mSegments.size());
    newSegment.gapTime = 0.0f;
    mSegments.push_back(newSegment);
    mCurrentSegment = (int)mSegments.size() - 1;
    LOGI("Added segment: %s", newSegment.name.c_str());
}

void ActionRecorder::setSegmentGap(int segmentIndex, float gapTime) {
    if (segmentIndex >= 0 && segmentIndex < (int)mSegments.size()) {
        mSegments[segmentIndex].gapTime = gapTime;
    }
}

void ActionRecorder::loadAction(const std::string& path) {
    LOGI("Loading action from: %s", path.c_str());

    mSegments.clear();
    std::ifstream file(path);
    if (!file.is_open()) {
        LOGE("Failed to open action file: %s", path.c_str());
        return;
    }

    ActionSegment currentSegment;
    std::string line;
    bool inSegment = false;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line[0] == '[') {
            if (inSegment) {
                mSegments.push_back(currentSegment);
            }
            currentSegment = ActionSegment();
            currentSegment.name = line.substr(1, line.find(']') - 1);

            size_t commaPos = currentSegment.name.find(',');
            if (commaPos != std::string::npos) {
                currentSegment.gapTime = std::stof(currentSegment.name.substr(commaPos + 1));
                currentSegment.name = currentSegment.name.substr(0, commaPos);
            } else {
                currentSegment.gapTime = 0.0f;
            }
            inSegment = true;
        } else if (inSegment && line.find(":") != std::string::npos) {
            size_t colonPos = line.find(':');
            std::string boneName = line.substr(0, colonPos);
            std::string dataStr = line.substr(colonPos + 1);

            std::istringstream dataStream(dataStr);
            std::string token;
            while (std::getline(dataStream, token, ';')) {
                if (token.empty()) continue;
                std::istringstream tokenStream(token);
                std::string timeStr, posStr, rotStr;
                if (std::getline(tokenStream, timeStr, '|') &&
                    std::getline(tokenStream, posStr, '|') &&
                    std::getline(tokenStream, rotStr, '|')) {
                    BoneKeyframe kf;
                    kf.time = std::stof(timeStr);
                    kf.position = stringToVec3(posStr);
                    kf.rotation = stringToQuat(rotStr);
                    currentSegment.boneKeyframes[boneName].push_back(kf);
                }
            }
        }
    }

    if (inSegment) {
        mSegments.push_back(currentSegment);
    }

    file.close();
    LOGI("Action loaded: %zu segments", mSegments.size());
}

void ActionRecorder::playAction(int segmentIndex) {
    if (segmentIndex < 0 || segmentIndex >= (int)mSegments.size()) {
        LOGE("Invalid segment index: %d", segmentIndex);
        return;
    }

    mIsPlaying = true;
    mCurrentSegment = segmentIndex;
    mPlayTime = 0.0f;
    LOGI("Playing segment: %s", mSegments[segmentIndex].name.c_str());
}

void ActionRecorder::stopAction() {
    if (mIsPlaying) {
        mIsPlaying = false;
        mPlayTime = 0.0f;
        LOGI("Action stopped");
    }
}

void ActionRecorder::exportToGobot(const std::string& path) {
    LOGI("Exporting to Gobot format: %s", path.c_str());

    std::ofstream file(path);
    if (!file.is_open()) {
        LOGE("Failed to create output file: %s", path.c_str());
        return;
    }

    file << "{\n";
    file << "  \"version\": 1,\n";
    file << "  \"segments\": [\n";

    for (size_t s = 0; s < mSegments.size(); s++) {
        const auto& seg = mSegments[s];
        file << "    {\n";
        file << "      \"name\": \"" << seg.name << "\",\n";
        file << "      \"gapTime\": " << seg.gapTime << ",\n";
        file << "      \"bones\": {\n";

        size_t boneCount = 0;
        for (const auto& bk : seg.boneKeyframes) {
            if (boneCount > 0) file << ",\n";
            file << "        \"" << bk.first << "\": [\n";

            for (size_t k = 0; k < bk.second.size(); k++) {
                const auto& kf = bk.second[k];
                if (k > 0) file << ",\n";
                file << "          {\"t\": " << kf.time
                     << ", \"px\": " << kf.position.x
                     << ", \"py\": " << kf.position.y
                     << ", \"pz\": " << kf.position.z
                     << ", \"qw\": " << kf.rotation.w
                     << ", \"qx\": " << kf.rotation.x
                     << ", \"qy\": " << kf.rotation.y
                     << ", \"qz\": " << kf.rotation.z << "}";
            }
            file << "\n        ]";
            boneCount++;
        }

        file << "\n      }\n";
        file << "    }";
        if (s < mSegments.size() - 1) file << ",";
        file << "\n";
    }

    file << "  ]\n";
    file << "}\n";

    file.close();
    LOGI("Export complete: %zu segments, %zu bytes", mSegments.size(), (size_t)file.tellp());
}