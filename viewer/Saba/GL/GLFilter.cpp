//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "GLFilter.h"
#include "GLShaderUtil.h"

#include <Saba/Base/Log.h>

#include <iostream>
#include <glm/glm.hpp>

namespace saba
{
    namespace
    {
        const char* g_filterVS = R"(
#version 140

out vec2 vs_UV;

void main()
{
    const vec2 positions[4] = vec2[4](
        vec2(-1.0, 1.0),
        vec2(-1.0, -1.0),
        vec2(1.0, 1.0),
        vec2(1.0, -1.0)
    );

    const vec2 uvs[4] = vec2[4](
        vec2(0.0, 1.0),
        vec2(0.0, 0.0),
        vec2(1.0, 1.0),
        vec2(1.0, 0.0)
    );

    gl_Position = vec4(positions[gl_VertexID % 4], 0.0, 1.0);
    vs_UV = uvs[gl_VertexID % 4];
}
)";

        const char* g_filterFS = R"(
#version 140

in vec2 vs_UV;

uniform sampler2D u_Tex;
uniform float u_Exposure;
uniform float u_Contrast;
uniform float u_Saturation;
uniform float u_HueShift;
uniform float u_Temperature;
uniform float u_Tint;
uniform float u_Intensity;

out vec4 fs_Color;

vec3 hueShift(vec3 color, float shift)
{
    const vec3 k = vec3(0.57735, 0.57735, 0.57735);
    float cosAngle = cos(shift);
    float sinAngle = sin(shift);
    vec3 temp = cross(k, color);
    return color + temp * sinAngle + cross(k, temp) * (1.0 - cosAngle);
}

vec3 colorTemperature(vec3 color, float temp, float tint)
{
    float r = color.r;
    float g = color.g;
    float b = color.b;

    r += temp * 0.01;
    b -= temp * 0.01;

    g += tint * 0.01;
    b -= tint * 0.005;
    r -= tint * 0.005;

    return vec3(r, g, b);
}

void main()
{
    vec3 color = texture(u_Tex, vs_UV).rgb;

    vec3 outColor = color;

    outColor = outColor * u_Exposure;

    outColor = (outColor - 0.5) * u_Contrast + 0.5;

    float luminance = dot(outColor, vec3(0.299, 0.587, 0.114));
    outColor = mix(vec3(luminance), outColor, u_Saturation);

    float hueRad = u_HueShift * 3.14159265 / 180.0;
    outColor = hueShift(outColor, hueRad);

    outColor = colorTemperature(outColor, u_Temperature, u_Tint);

    outColor = clamp(outColor, 0.0, 1.0);

    fs_Color = vec4(mix(color, outColor, u_Intensity), 1.0);
}
)";
    }

    FilterParameters GLFilter::GetPresetParameters(FilterType filterType)
    {
        FilterParameters params;
        switch (filterType)
        {
        case FilterType::BrightWhite:
            params.m_exposure = 1.5f;
            params.m_contrast = 0.8f;
            params.m_saturation = 0.6f;
            params.m_hueShift = 0.0f;
            params.m_temperature = -10.0f;
            params.m_tint = 5.0f;
            break;
        case FilterType::SkinBright:
            params.m_exposure = 1.2f;
            params.m_contrast = 0.9f;
            params.m_saturation = 0.8f;
            params.m_hueShift = 0.0f;
            params.m_temperature = 5.0f;
            params.m_tint = 3.0f;
            break;
        case FilterType::Dark:
            params.m_exposure = 0.4f;
            params.m_contrast = 1.3f;
            params.m_saturation = 0.7f;
            params.m_hueShift = 0.0f;
            params.m_temperature = 0.0f;
            params.m_tint = 0.0f;
            break;
        case FilterType::GenshinDay:
            params.m_exposure = 1.1f;
            params.m_contrast = 1.1f;
            params.m_saturation = 1.3f;
            params.m_hueShift = 0.0f;
            params.m_temperature = 15.0f;
            params.m_tint = 10.0f;
            break;
        case FilterType::GenshinNight:
            params.m_exposure = 0.6f;
            params.m_contrast = 1.2f;
            params.m_saturation = 0.8f;
            params.m_hueShift = 0.0f;
            params.m_temperature = -20.0f;
            params.m_tint = -10.0f;
            break;
        case FilterType::GenshinDusk:
            params.m_exposure = 0.8f;
            params.m_contrast = 1.15f;
            params.m_saturation = 1.1f;
            params.m_hueShift = 0.0f;
            params.m_temperature = 30.0f;
            params.m_tint = 15.0f;
            break;
        default:
            break;
        }
        return params;
    }

    GLFilter::GLFilter()
        : m_currentFilter(FilterType::None)
        , m_intensity(1.0f)
        , m_initialized(false)
        , m_width(0)
        , m_height(0)
        , m_uTex(-1)
        , m_uExposure(-1)
        , m_uContrast(-1)
        , m_uSaturation(-1)
        , m_uHueShift(-1)
        , m_uTemperature(-1)
        , m_uTint(-1)
        , m_uIntensity(-1)
        , m_pingPongIndex(0)
    {
    }

    GLFilter::~GLFilter()
    {
        Destroy();
    }

    bool GLFilter::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        m_filterProg = CreateShaderProgram(g_filterVS, g_filterFS);
        if (m_filterProg.Get() == 0)
        {
            SABA_ERROR("GLFilter: Failed to create filter shader program.");
            return false;
        }

        m_uTex = glGetUniformLocation(m_filterProg, "u_Tex");
        m_uExposure = glGetUniformLocation(m_filterProg, "u_Exposure");
        m_uContrast = glGetUniformLocation(m_filterProg, "u_Contrast");
        m_uSaturation = glGetUniformLocation(m_filterProg, "u_Saturation");
        m_uHueShift = glGetUniformLocation(m_filterProg, "u_HueShift");
        m_uTemperature = glGetUniformLocation(m_filterProg, "u_Temperature");
        m_uTint = glGetUniformLocation(m_filterProg, "u_Tint");
        m_uIntensity = glGetUniformLocation(m_filterProg, "u_Intensity");

        m_vao.Create();

        m_initialized = true;

        SABA_INFO("GLFilter initialized successfully.");
        return true;
    }

    void GLFilter::Destroy()
    {
        m_filterProg.Destroy();
        m_vao.Destroy();

        for (int i = 0; i < 2; i++)
        {
            m_pingPongFBOs[i].Destroy();
            m_pingPongTextures[i].Destroy();
        }

        m_width = 0;
        m_height = 0;
        m_initialized = false;
    }

    void GLFilter::SetupFramebuffer(int width, int height)
    {
        for (int i = 0; i < 2; i++)
        {
            m_pingPongTextures[i].Create();
            glBindTexture(GL_TEXTURE_2D, m_pingPongTextures[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);

            m_pingPongFBOs[i].Create();
            glBindFramebuffer(GL_FRAMEBUFFER, m_pingPongFBOs[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingPongTextures[i], 0);
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE)
            {
                SABA_WARN("GLFilter: Framebuffer {} incomplete. Status: {}", i, status);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        m_width = width;
        m_height = height;
        m_pingPongIndex = 0;
    }

    void GLFilter::Resize(int width, int height)
    {
        if (m_width == width && m_height == height)
        {
            return;
        }

        for (int i = 0; i < 2; i++)
        {
            m_pingPongFBOs[i].Destroy();
            m_pingPongTextures[i].Destroy();
        }

        SetupFramebuffer(width, height);
    }

    void GLFilter::SetIntensity(float intensity)
    {
        m_intensity = glm::clamp(intensity, 0.0f, 1.0f);
    }

    void GLFilter::SetCustomParameters(const FilterParameters& params)
    {
        m_customParams = params;
    }

    void GLFilter::RenderPass(GLuint inputTexture, const FilterParameters& params, float intensity)
    {
        int nextIndex = 1 - m_pingPongIndex;

        glBindFramebuffer(GL_FRAMEBUFFER, m_pingPongFBOs[nextIndex]);
        glViewport(0, 0, m_width, m_height);

        glUseProgram(m_filterProg);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputTexture);
        SetUniform(m_uTex, 0);

        SetUniform(m_uExposure, params.m_exposure);
        SetUniform(m_uContrast, params.m_contrast);
        SetUniform(m_uSaturation, params.m_saturation);
        SetUniform(m_uHueShift, params.m_hueShift);
        SetUniform(m_uTemperature, params.m_temperature);
        SetUniform(m_uTint, params.m_tint);
        SetUniform(m_uIntensity, intensity);

        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_pingPongIndex = nextIndex;
    }

    void GLFilter::Apply(FilterType filterType)
    {
        if (!m_initialized)
        {
            SABA_WARN("GLFilter: Not initialized.");
            return;
        }

        m_currentFilter = filterType;

        if (filterType == FilterType::None)
        {
            return;
        }

        if (filterType == FilterType::Custom)
        {
            m_currentParams = m_customParams;
        }
        else
        {
            m_currentParams = GetPresetParameters(filterType);
        }
    }
}