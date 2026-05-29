#ifndef ACTION_RECORDER_H_
#define ACTION_RECORDER_H_

#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>
#include <map>

class ActionRecorder {
public:
    struct BoneKeyframe {
        glm::vec3 position;
        glm::quat rotation;
        float time;
    };

    struct ActionSegment {
        std::string name;
        std::map<std::string, std::vector<BoneKeyframe>> boneKeyframes;
        float gapTime;
    };

    ActionRecorder();
    ~ActionRecorder();

    void startRecording();
    void stopRecording(const std::string& outputPath);
    bool isRecording() const { return mIsRecording; }

    void recordFrame(const std::map<std::string, glm::vec3>& bonePositions,
                     const std::map<std::string, glm::quat>& boneRotations,
                     float deltaTime);

    void addSegment();
    void setSegmentGap(int segmentIndex, float gapTime);

    void loadAction(const std::string& path);
    void playAction(int segmentIndex);
    void stopAction();
    bool isPlaying() const { return mIsPlaying; }

    int getSegmentCount() const { return (int)mSegments.size(); }
    const ActionSegment& getSegment(int index) const { return mSegments[index]; }

    void exportToGobot(const std::string& path);

private:
    bool mIsRecording;
    bool mIsPlaying;
    float mRecordTime;
    float mPlayTime;
    int mCurrentSegment;

    std::vector<ActionSegment> mSegments;
    std::map<std::string, glm::vec3> mCurrentBonePositions;
    std::map<std::string, glm::quat> mCurrentBoneRotations;
};

#endif