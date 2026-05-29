//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_ENVIRONMENTPANEL_H_
#define SABA_VIEWER_ENVIRONMENTPANEL_H_

#include "EnvironmentManager.h"

namespace saba
{
    class EnvironmentPanel
    {
    public:
        EnvironmentPanel();

        void SetEnvironmentManager(EnvironmentManager* envManager);
        void Draw();

    private:
        void DrawSunControl();
        void DrawGroundControl();
        void DrawPrimitiveControl();
        void DrawPrimitiveList();
        void DrawEnvObjectControl();

        EnvironmentManager* m_envManager;

        int m_selectedPrimitiveType;
        float m_primitiveSize[3];
        float m_primitivePosition[3];
        float m_primitiveColor[4];

        int m_selectedEnvObjectType;
        float m_envObjectPosition[3];
        float m_envObjectSize[3];
        float m_envObjectColor[4];

        int m_selectedGroundType;

        bool m_enablePanel;
    };
}

#endif // !SABA_VIEWER_ENVIRONMENTPANEL_H_