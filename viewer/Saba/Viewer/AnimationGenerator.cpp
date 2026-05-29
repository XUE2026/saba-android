//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "AnimationGenerator.h"
#include <Saba/Model/MMD/MMDModel.h>
#include <Saba/Model/MMD/MMDNode.h>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <algorithm>
#include <cmath>

namespace saba
{
    const float AnimationGenerator::WalkStepDuration = 0.5f;
    const float AnimationGenerator::RunStepDuration = 0.3f;
    const float AnimationGenerator::JumpCrouchDuration = 0.2f;
    const float AnimationGenerator::JumpLeapDuration = 0.15f;
    const float AnimationGenerator::JumpAirborneDuration = 0.3f;
    const float AnimationGenerator::JumpLandDuration = 0.15f;
    const float AnimationGenerator::JumpRecoverDuration = 0.2f;

    AnimationGenerator::AnimationGenerator()
        : m_animType(BuiltinAnimType::None)
        , m_modelIndex(-1)
    {
    }

    void AnimationGenerator::PlayBuiltinAnimation(BuiltinAnimType animType, int modelIndex)
    {
        m_animType = animType;
        m_modelIndex = modelIndex;
    }

    void AnimationGenerator::StopBuiltinAnimation()
    {
        m_animType = BuiltinAnimType::None;
        m_modelIndex = -1;
    }

    float AnimationGenerator::GetDuration() const
    {
        switch (m_animType)
        {
        case BuiltinAnimType::Idle:
            return 2.0f;
        case BuiltinAnimType::Walk:
            return WalkStepDuration * 2.0f;
        case BuiltinAnimType::Run:
            return RunStepDuration * 2.0f;
        case BuiltinAnimType::Jump:
            return JumpCrouchDuration + JumpLeapDuration + JumpAirborneDuration + JumpLandDuration + JumpRecoverDuration;
        default:
            return 0.0f;
        }
    }

    float AnimationGenerator::EaseInOut(float t)
    {
        t = glm::clamp(t, 0.0f, 1.0f);
        if (t < 0.5f)
        {
            return 2.0f * t * t;
        }
        else
        {
            return -1.0f + (4.0f - 2.0f * t) * t;
        }
    }

    float AnimationGenerator::EaseOut(float t)
    {
        t = glm::clamp(t, 0.0f, 1.0f);
        return 1.0f - (1.0f - t) * (1.0f - t);
    }

    float AnimationGenerator::EaseIn(float t)
    {
        t = glm::clamp(t, 0.0f, 1.0f);
        return t * t;
    }

    float AnimationGenerator::SmoothStep(float t)
    {
        t = glm::clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    void AnimationGenerator::SetBonePose(MMDModel* model, const std::string& boneName,
                                          const glm::vec3& translate, const glm::quat& rotate)
    {
        auto nodeMan = model->GetNodeManager();
        if (nodeMan == nullptr)
        {
            return;
        }

        MMDNode* node = nodeMan->GetMMDNode(boneName);
        if (node == nullptr)
        {
            return;
        }

        node->SetAnimationTranslate(translate);
        node->SetAnimationRotate(rotate);
    }

    void AnimationGenerator::Generate(float time, MMDModel* model, float blendWeight)
    {
        if (model == nullptr || m_animType == BuiltinAnimType::None)
        {
            return;
        }

        switch (m_animType)
        {
        case BuiltinAnimType::Idle:
            GenerateIdle(time, model);
            break;
        case BuiltinAnimType::Walk:
            GenerateWalk(time, model);
            break;
        case BuiltinAnimType::Run:
            GenerateRun(time, model);
            break;
        case BuiltinAnimType::Jump:
            GenerateJump(time, model);
            break;
        default:
            break;
        }
    }

    void AnimationGenerator::GenerateIdle(float time, MMDModel* model)
    {
        float breathCycle = 2.0f;
        float breathPhase = fmod(time, breathCycle) / breathCycle;
        float breath = sin(breathPhase * glm::two_pi<float>()) * 0.02f;

        glm::quat upperBodyBreath = glm::angleAxis(breath, glm::vec3(0, 0, 1));

        auto nodeMan = model->GetNodeManager();
        if (nodeMan == nullptr)
        {
            return;
        }

        MMDNode* upperBody = nodeMan->GetMMDNode(std::string("\xE4\xB8\x8A\xE5\x8D\x8A\xE8\xBA\xAB"));
        if (upperBody != nullptr)
        {
            glm::quat currentRot = upperBody->GetAnimationRotate();
            upperBody->SetAnimationRotate(currentRot * upperBodyBreath);
        }

        float swayCycle = 4.0f;
        float swayPhase = fmod(time, swayCycle) / swayCycle;
        float sway = sin(swayPhase * glm::two_pi<float>()) * 0.005f;

        MMDNode* center = nodeMan->GetMMDNode(std::string("\xE3\x82\xBB\xE3\x83\xB3\xE3\x82\xBF\xE3\x83\xBC"));
        if (center != nullptr)
        {
            glm::vec3 currentT = center->GetAnimationTranslate();
            center->SetAnimationTranslate(currentT + glm::vec3(sway, 0, 0));
        }
    }

    void AnimationGenerator::GenerateWalk(float time, MMDModel* model)
    {
        float stepCycle = WalkStepDuration * 2.0f;
        float phase = fmod(time, stepCycle) / stepCycle;

        float legSwing = sin(phase * glm::two_pi<float>());
        float armSwing = sin((phase + 0.5f) * glm::two_pi<float>());
        float hipBob = abs(sin(phase * glm::pi<float>()));
        float hipSway = sin(phase * glm::two_pi<float>());

        float legAngle = legSwing * 25.0f;
        float armAngle = armSwing * 20.0f;
        float hipTilt = hipSway * 3.0f;
        float hipVertical = hipBob * 2.0f;

        auto nodeMan = model->GetNodeManager();
        if (nodeMan == nullptr)
        {
            return;
        }

        MMDNode* leftLeg = nodeMan->GetMMDNode(std::string("\xE5\xB7\xA6\xE8\xB6\xB3"));
        if (leftLeg != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(legAngle), glm::vec3(1, 0, 0));
            leftLeg->SetAnimationRotate(rot);
        }

        MMDNode* rightLeg = nodeMan->GetMMDNode(std::string("\xE5\x8F\xB3\xE8\xB6\xB3"));
        if (rightLeg != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(-legAngle), glm::vec3(1, 0, 0));
            rightLeg->SetAnimationRotate(rot);
        }

        MMDNode* leftKnee = nodeMan->GetMMDNode(std::string("\xE5\xB7\xA6\xE3\x81\xB2\xE3\x81\x96"));
        if (leftKnee != nullptr)
        {
            float kneeAngle = legAngle > 0 ? legAngle * 0.3f : 0.0f;
            glm::quat rot = glm::angleAxis(glm::radians(kneeAngle), glm::vec3(1, 0, 0));
            leftKnee->SetAnimationRotate(rot);
        }

        MMDNode* rightKnee = nodeMan->GetMMDNode(std::string("\xE5\x8F\xB3\xE3\x81\xB2\xE3\x81\x96"));
        if (rightKnee != nullptr)
        {
            float kneeAngle = legAngle < 0 ? -legAngle * 0.3f : 0.0f;
            glm::quat rot = glm::angleAxis(glm::radians(kneeAngle), glm::vec3(1, 0, 0));
            rightKnee->SetAnimationRotate(rot);
        }

        MMDNode* leftArm = nodeMan->GetMMDNode(std::string("\xE5\xB7\xA6\xE8\x85\x95"));
        if (leftArm != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(-armAngle), glm::vec3(1, 0, 0));
            leftArm->SetAnimationRotate(rot);
        }

        MMDNode* rightArm = nodeMan->GetMMDNode(std::string("\xE5\x8F\xB3\xE8\x85\x95"));
        if (rightArm != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(armAngle), glm::vec3(1, 0, 0));
            rightArm->SetAnimationRotate(rot);
        }

        MMDNode* lowerBody = nodeMan->GetMMDNode(std::string("\xE4\xB8\x8B\xE5\x8D\x8A\xE8\xBA\xAB"));
        if (lowerBody != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(hipTilt), glm::vec3(0, 0, 1));
            lowerBody->SetAnimationRotate(rot);
            glm::vec3 t = lowerBody->GetAnimationTranslate();
            lowerBody->SetAnimationTranslate(glm::vec3(t.x, t.y - hipVertical * 3.0f, t.z));
        }

        MMDNode* center = nodeMan->GetMMDNode(std::string("\xE3\x82\xBB\xE3\x83\xB3\xE3\x82\xBF\xE3\x83\xBC"));
        if (center != nullptr)
        {
            glm::vec3 t = center->GetAnimationTranslate();
            center->SetAnimationTranslate(glm::vec3(t.x, t.y - hipVertical, t.z));
        }
    }

    void AnimationGenerator::GenerateRun(float time, MMDModel* model)
    {
        float stepCycle = RunStepDuration * 2.0f;
        float phase = fmod(time, stepCycle) / stepCycle;

        float legSwing = sin(phase * glm::two_pi<float>());
        float armSwing = sin((phase + 0.5f) * glm::two_pi<float>());
        float hipBob = abs(sin(phase * glm::pi<float>()));
        float hipSway = sin(phase * glm::two_pi<float>());

        float legAngle = legSwing * 40.0f;
        float armAngle = armSwing * 35.0f;
        float hipTilt = hipSway * 5.0f;
        float hipVertical = hipBob * 4.0f;

        auto nodeMan = model->GetNodeManager();
        if (nodeMan == nullptr)
        {
            return;
        }

        MMDNode* leftLeg = nodeMan->GetMMDNode(std::string("\xE5\xB7\xA6\xE8\xB6\xB3"));
        if (leftLeg != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(legAngle), glm::vec3(1, 0, 0));
            leftLeg->SetAnimationRotate(rot);
        }

        MMDNode* rightLeg = nodeMan->GetMMDNode(std::string("\xE5\x8F\xB3\xE8\xB6\xB3"));
        if (rightLeg != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(-legAngle), glm::vec3(1, 0, 0));
            rightLeg->SetAnimationRotate(rot);
        }

        MMDNode* leftKnee = nodeMan->GetMMDNode(std::string("\xE5\xB7\xA6\xE3\x81\xB2\xE3\x81\x96"));
        if (leftKnee != nullptr)
        {
            float kneeAngle = legAngle > 0 ? legAngle * 0.4f : 0.0f;
            glm::quat rot = glm::angleAxis(glm::radians(kneeAngle), glm::vec3(1, 0, 0));
            leftKnee->SetAnimationRotate(rot);
        }

        MMDNode* rightKnee = nodeMan->GetMMDNode(std::string("\xE5\x8F\xB3\xE3\x81\xB2\xE3\x81\x96"));
        if (rightKnee != nullptr)
        {
            float kneeAngle = legAngle < 0 ? -legAngle * 0.4f : 0.0f;
            glm::quat rot = glm::angleAxis(glm::radians(kneeAngle), glm::vec3(1, 0, 0));
            rightKnee->SetAnimationRotate(rot);
        }

        MMDNode* leftArm = nodeMan->GetMMDNode(std::string("\xE5\xB7\xA6\xE8\x85\x95"));
        if (leftArm != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(-armAngle), glm::vec3(1, 0, 0));
            leftArm->SetAnimationRotate(rot);
        }

        MMDNode* rightArm = nodeMan->GetMMDNode(std::string("\xE5\x8F\xB3\xE8\x85\x95"));
        if (rightArm != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(armAngle), glm::vec3(1, 0, 0));
            rightArm->SetAnimationRotate(rot);
        }

        MMDNode* lowerBody = nodeMan->GetMMDNode(std::string("\xE4\xB8\x8B\xE5\x8D\x8A\xE8\xBA\xAB"));
        if (lowerBody != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(hipTilt), glm::vec3(0, 0, 1));
            lowerBody->SetAnimationRotate(rot);
            glm::vec3 t = lowerBody->GetAnimationTranslate();
            lowerBody->SetAnimationTranslate(glm::vec3(t.x, t.y - hipVertical * 2.0f, t.z));
        }

        MMDNode* center = nodeMan->GetMMDNode(std::string("\xE3\x82\xBB\xE3\x83\xB3\xE3\x83\xBF\xE3\x83\xBC"));
        if (center != nullptr)
        {
            glm::vec3 t = center->GetAnimationTranslate();
            center->SetAnimationTranslate(glm::vec3(t.x, t.y - hipVertical * 0.5f, t.z));
        }

        MMDNode* upperBody = nodeMan->GetMMDNode(std::string("\xE4\xB8\x8A\xE5\x8D\x8A\xE8\xBA\xAB"));
        if (upperBody != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(hipSway * 2.0f), glm::vec3(0, 0, 1));
            upperBody->SetAnimationRotate(rot);
        }
    }

    void AnimationGenerator::GenerateJump(float time, MMDModel* model)
    {
        float totalDuration = JumpCrouchDuration + JumpLeapDuration + JumpAirborneDuration + JumpLandDuration + JumpRecoverDuration;
        float t = fmod(time, totalDuration);

        float crouchT = glm::clamp(t / JumpCrouchDuration, 0.0f, 1.0f);
        float leapT = 0.0f;
        float airborneT = 0.0f;
        float landT = 0.0f;
        float recoverT = 0.0f;

        if (t >= JumpCrouchDuration)
        {
            float remaining = t - JumpCrouchDuration;
            if (remaining < JumpLeapDuration)
            {
                leapT = remaining / JumpLeapDuration;
            }
            else
            {
                leapT = 1.0f;
                remaining -= JumpLeapDuration;
                if (remaining < JumpAirborneDuration)
                {
                    airborneT = remaining / JumpAirborneDuration;
                }
                else
                {
                    airborneT = 1.0f;
                    remaining -= JumpAirborneDuration;
                    if (remaining < JumpLandDuration)
                    {
                        landT = remaining / JumpLandDuration;
                    }
                    else
                    {
                        landT = 1.0f;
                        remaining -= JumpLandDuration;
                        if (remaining < JumpRecoverDuration)
                        {
                            recoverT = remaining / JumpRecoverDuration;
                        }
                        else
                        {
                            recoverT = 1.0f;
                        }
                    }
                }
            }
        }

        auto nodeMan = model->GetNodeManager();
        if (nodeMan == nullptr)
        {
            return;
        }

        float crouchHeight = 0.0f;
        float bodyLean = 0.0f;
        float legBend = 0.0f;
        float armSwing = 0.0f;

        if (crouchT < 1.0f && leapT == 0.0f && airborneT == 0.0f && landT == 0.0f)
        {
            float eased = EaseInOut(crouchT);
            crouchHeight = eased * 8.0f;
            bodyLean = eased * 10.0f;
            legBend = eased * 60.0f;
            armSwing = eased * -30.0f;
        }
        else if (leapT > 0.0f && leapT < 1.0f)
        {
            float eased = EaseOut(leapT);
            crouchHeight = (1.0f - eased) * 8.0f;
            bodyLean = (1.0f - eased) * 10.0f;
            legBend = (1.0f - eased) * 60.0f;
            armSwing = (1.0f - eased) * -30.0f + eased * 45.0f;
        }
        else if (airborneT > 0.0f)
        {
            float eased = EaseInOut(airborneT);
            crouchHeight = 0.0f;
            bodyLean = 0.0f;
            legBend = sin(eased * glm::pi<float>()) * 30.0f;
            armSwing = 45.0f + (1.0f - eased) * 15.0f;
        }
        else if (landT > 0.0f && landT < 1.0f)
        {
            float eased = EaseIn(landT);
            crouchHeight = eased * 6.0f;
            bodyLean = eased * 8.0f;
            legBend = eased * 50.0f;
            armSwing = 45.0f - eased * 60.0f;
        }
        else if (recoverT > 0.0f)
        {
            float eased = EaseOut(recoverT);
            crouchHeight = (1.0f - eased) * 6.0f;
            bodyLean = (1.0f - eased) * 8.0f;
            legBend = (1.0f - eased) * 50.0f;
            armSwing = (1.0f - eased) * -15.0f;
        }

        MMDNode* lowerBody = nodeMan->GetMMDNode(std::string("\xE4\xB8\x8B\xE5\x8D\x8A\xE8\xBA\xAB"));
        if (lowerBody != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(bodyLean), glm::vec3(1, 0, 0));
            lowerBody->SetAnimationRotate(rot);
            glm::vec3 t = lowerBody->GetAnimationTranslate();
            lowerBody->SetAnimationTranslate(glm::vec3(t.x, t.y - crouchHeight, t.z));
        }

        MMDNode* leftLeg = nodeMan->GetMMDNode(std::string("\xE5\xB7\xA6\xE8\xB6\xB3"));
        if (leftLeg != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(legBend), glm::vec3(1, 0, 0));
            leftLeg->SetAnimationRotate(rot);
        }

        MMDNode* rightLeg = nodeMan->GetMMDNode(std::string("\xE5\x8F\xB3\xE8\xB6\xB3"));
        if (rightLeg != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(legBend), glm::vec3(1, 0, 0));
            rightLeg->SetAnimationRotate(rot);
        }

        MMDNode* leftKnee = nodeMan->GetMMDNode(std::string("\xE5\xB7\xA6\xE3\x81\xB2\xE3\x81\x96"));
        if (leftKnee != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(legBend * 0.5f), glm::vec3(1, 0, 0));
            leftKnee->SetAnimationRotate(rot);
        }

        MMDNode* rightKnee = nodeMan->GetMMDNode(std::string("\xE5\x8F\xB3\xE3\x81\xB2\xE3\x81\x96"));
        if (rightKnee != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(legBend * 0.5f), glm::vec3(1, 0, 0));
            rightKnee->SetAnimationRotate(rot);
        }

        MMDNode* leftArm = nodeMan->GetMMDNode(std::string("\xE5\xB7\xA6\xE8\x85\x95"));
        if (leftArm != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(armSwing), glm::vec3(1, 0, 0));
            leftArm->SetAnimationRotate(rot);
        }

        MMDNode* rightArm = nodeMan->GetMMDNode(std::string("\xE5\x8F\xB3\xE8\x85\x95"));
        if (rightArm != nullptr)
        {
            glm::quat rot = glm::angleAxis(glm::radians(armSwing), glm::vec3(1, 0, 0));
            rightArm->SetAnimationRotate(rot);
        }

        MMDNode* center = nodeMan->GetMMDNode(std::string("\xE3\x82\xBB\xE3\x83\xB3\xE3\x83\xBF\xE3\x83\xBC"));
        if (center != nullptr)
        {
            glm::vec3 t = center->GetAnimationTranslate();
            center->SetAnimationTranslate(glm::vec3(t.x, t.y - crouchHeight * 0.3f, t.z));
        }
    }
}