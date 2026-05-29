//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_GL_GLFILTER_H_
#define SABA_GL_GLFILTER_H_

#include "GLObject.h"

#include <GL/gl3w.h>
#include <glm/glm.hpp>

namespace saba
{
    enum class FilterType
    {
        None,
        BrightWhite,
        SkinBright,
        Dark,
        GenshinDay,
        GenshinNight,
        GenshinDusk,
        Custom,
    };

    struct FilterParameters
    {
        float m_exposure = 1.0f;
        float m_contrast = 1.0f;
        float m_saturation = 1.0f;
        float m_hueShift = 0.0f;
        float m_temperature = 0.0f;
        float m_tint = 0.0f;

        bool operator==(const FilterParameters& rhs) const
        {
            return m_exposure == rhs.m_exposure
                && m_contrast == rhs.m_contrast
                && m_saturation == rhs.m_saturation
                && m_hueShift == rhs.m_hueShift
                && m_temperature == rhs.m_temperature
                && m_tint == rhs.m_tint;
        }

        bool operator!=(const FilterParameters& rhs) const
        {
            return !(*this == rhs);
        }
    };

    class GLFilter
    {
    public:
        GLFilter();
        ~GLFilter();

        bool Initialize();
        void Destroy();

        void Resize(int width, int height);

        void Apply(FilterType filterType);
        void SetIntensity(float intensity);
        void SetCustomParameters(const FilterParameters& params);

        GLuint GetOutputTexture() const { return m_pingPongTextures[m_pingPongIndex]; }

        FilterType GetCurrentFilterType() const { return m_currentFilter; }
        float GetIntensity() const { return m_intensity; }
        const FilterParameters& GetCustomParameters() const { return m_customParams; }
        const FilterParameters& GetCurrentParameters() const { return m_currentParams; }

        static FilterParameters GetPresetParameters(FilterType filterType);

    private:
        void RenderPass(GLuint inputTexture, const FilterParameters& params, float intensity);
        void SetupFramebuffer(int width, int height);

        FilterType m_currentFilter;
        float m_intensity;
        FilterParameters m_customParams;
        FilterParameters m_currentParams;

        bool m_initialized;
        int m_width;
        int m_height;

        GLProgramObject m_filterProg;
        GLVertexArrayObject m_vao;

        GLint m_uTex;
        GLint m_uExposure;
        GLint m_uContrast;
        GLint m_uSaturation;
        GLint m_uHueShift;
        GLint m_uTemperature;
        GLint m_uTint;
        GLint m_uIntensity;

        GLFramebufferObject m_pingPongFBOs[2];
        GLTextureObject m_pingPongTextures[2];
        int m_pingPongIndex;
    };
}

#endif // !SABA_GL_GLFILTER_H_