//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_ANIMATIONGENERATOR_H_
#define SABA_VIEWER_ANIMATIONGENERATOR_H_

#include <cstdint>
#include <map>
#include <string>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace saba
{
    class MMDModel;
    class MMDNode;

    enum class BuiltinAnimType
    {
        None,
        Idle,
        Walk,
        Run,
        Jump,
    };

    struct GeneratedBonePose
    {
        glm::vec3 m_translate;
        glm::quat m_rotate;
    };

    class AnimationGenerator
    {
    public:
        AnimationGenerator();

        void PlayBuiltinAnimation(BuiltinAnimType animType, int modelIndex);
        void StopBuiltinAnimation();

        BuiltinAnimType GetCurrentAnimType() const { return m_animType; }
        bool IsPlaying() const { return m_animType != BuiltinAnimType::None; }
        int GetModelIndex() const { return m_modelIndex; }
        float GetDuration() const;

        void Generate(float time, MMDModel* model, float blendWeight = 1.0f);

    private:
        void GenerateIdle(float time, MMDModel* model);
        void GenerateWalk(float time, MMDModel* model);
        void GenerateRun(float time, MMDModel* model);
        void GenerateJump(float time, MMDModel* model);

        void SetBonePose(MMDModel* model, const std::string& boneName,
                         const glm::vec3& translate, const glm::quat& rotate);

        static float EaseInOut(float t);
        static float EaseOut(float t);
        static float EaseIn(float t);
        static float SmoothStep(float t);

        BuiltinAnimType m_animType;
        int             m_modelIndex;

        static const float WalkStepDuration;
        static const float RunStepDuration;
        static const float JumpCrouchDuration;
        static const float JumpLeapDuration;
        static const float JumpAirborneDuration;
        static const float JumpLandDuration;
        static const float JumpRecoverDuration;
    };
}

#endif