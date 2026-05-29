//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_BONEPANEL_H_
#define SABA_VIEWER_BONEPANEL_H_

#include "BoneManipulator.h"
#include "AnimationGenerator.h"

#include <memory>

namespace saba
{
    class GLMMDModel;

    class BonePanel
    {
    public:
        BonePanel();

        void SetModel(int modelIndex, GLMMDModel* model);
        void ClearModel();

        void Draw();

        BoneManipulator* GetBoneManipulator() { return m_boneManipulator.get(); }
        AnimationGenerator* GetAnimationGenerator() { return m_animationGenerator.get(); }

        bool IsBonePanelOpen() const { return m_bonePanelOpen; }
        bool IsAnimPanelOpen() const { return m_animPanelOpen; }

    private:
        void DrawBonePanel();
        void DrawAnimPanel();

        void ApplyBonePoseToModel();
        void ApplyAnimationToModel(float time, float deltaTime);

        int             m_modelIndex;
        GLMMDModel*     m_model;

        std::unique_ptr<BoneManipulator>    m_boneManipulator;
        std::unique_ptr<AnimationGenerator> m_animationGenerator;

        bool    m_bonePanelOpen;
        bool    m_animPanelOpen;

        int     m_selectedBoneIndex;

        float   m_rotX;
        float   m_rotY;
        float   m_rotZ;
        float   m_transX;
        float   m_transY;
        float   m_transZ;

        float   m_animTime;
        int     m_currentAnimItem;
    };
}

#endif