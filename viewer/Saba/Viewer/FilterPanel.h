//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_FILTERPANEL_H_
#define SABA_VIEWER_FILTERPANEL_H_

#include <Saba/GL/GLFilter.h>

namespace saba
{
    class ViewerContext;

    class FilterPanel
    {
    public:
        FilterPanel();
        ~FilterPanel();

        void SetViewerContext(ViewerContext* context);
        void SetGLFilter(GLFilter* filter);

        void Draw();

        bool IsEnabled() const { return m_enabled; }
        void SetEnabled(bool enabled) { m_enabled = enabled; }

    private:
        ViewerContext* m_context;
        GLFilter* m_filter;

        bool m_enabled;
        bool m_paramsChanged;
        FilterType m_selectedFilterType;
        FilterParameters m_editParams;
    };
}

#endif // !SABA_VIEWER_FILTERPANEL_H_