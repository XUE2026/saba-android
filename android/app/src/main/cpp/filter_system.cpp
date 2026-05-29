#include "filter_system.h"
#include <android/log.h>
#include <cstring>
#include <cmath>

#define LOG_TAG "FilterSystem"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static const char* gVertexShader = R"(
#version 300 es
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;
out vec2 vTexCoord;
void main() {
    vTexCoord = aTexCoord;
    gl_Position = vec4(aPosition, 0.0, 1.0);
}
)";

static const char* getFilterFragmentShader(FilterSystem::FilterType type) {
    switch (type) {
        case FilterSystem::Filter_WhiteBright:
            return R"(
#version 300 es
precision highp float;
in vec2 vTexCoord;
out vec4 fragColor;
uniform sampler2D uTexture;
uniform float uIntensity;
uniform float uTime;
void main() {
    vec4 color = texture(uTexture, vTexCoord);
    float bright = 0.3 + 0.7 * uIntensity;
    float contrast = 1.0 - 0.4 * uIntensity;
    vec3 c = color.rgb * bright;
    c = (c - 0.5) * contrast + 0.5;
    c = clamp(c, 0.0, 1.0);
    float gray = dot(c, vec3(0.299, 0.587, 0.114));
    c = mix(c, vec3(gray), 0.15 * uIntensity);
    fragColor = vec4(c, color.a);
}
)";

        case FilterSystem::Filter_SkinBright:
            return R"(
#version 300 es
precision highp float;
in vec2 vTexCoord;
out vec4 fragColor;
uniform sampler2D uTexture;
uniform float uIntensity;
uniform float uTime;
void main() {
    vec4 color = texture(uTexture, vTexCoord);
    vec3 c = color.rgb;
    float warm = uIntensity * 0.15;
    c.r += warm;
    c.b -= warm * 0.5;
    float lum = dot(c, vec3(0.2126, 0.7152, 0.0722));
    vec3 skin = vec3(0.95, 0.85, 0.75);
    c = mix(c, skin * lum * 1.2, 0.2 * uIntensity);
    c = clamp(c, 0.0, 1.0);
    fragColor = vec4(c, color.a);
}
)";

        case FilterSystem::Filter_Dark:
            return R"(
#version 300 es
precision highp float;
in vec2 vTexCoord;
out vec4 fragColor;
uniform sampler2D uTexture;
uniform float uIntensity;
uniform float uTime;
void main() {
    vec4 color = texture(uTexture, vTexCoord);
    float dark = 1.0 - 0.5 * uIntensity;
    float contrast = 1.0 + 0.5 * uIntensity;
    vec3 c = color.rgb * dark;
    c = (c - 0.5) * contrast + 0.5;
    c = clamp(c, 0.0, 1.0);
    float gray = dot(c, vec3(0.299, 0.587, 0.114));
    c = mix(c, vec3(gray), 0.1 * uIntensity);
    c *= (1.0 - 0.1 * uIntensity);
    fragColor = vec4(c, color.a);
}
)";

        case FilterSystem::Filter_GenshinDay:
            return R"(
#version 300 es
precision highp float;
in vec2 vTexCoord;
out vec4 fragColor;
uniform sampler2D uTexture;
uniform float uIntensity;
uniform float uTime;
void main() {
    vec4 color = texture(uTexture, vTexCoord);
    vec3 c = color.rgb;
    float saturation = 1.0 + 0.4 * uIntensity;
    float gray = dot(c, vec3(0.299, 0.587, 0.114));
    c = mix(vec3(gray), c, saturation);
    c = pow(c, vec3(1.0 / 0.9));
    c = clamp(c, 0.0, 1.0);
    float cel = floor(c.r * 4.0) / 4.0;
    c.r = mix(c.r, cel, 0.3 * uIntensity);
    cel = floor(c.g * 4.0) / 4.0;
    c.g = mix(c.g, cel, 0.3 * uIntensity);
    cel = floor(c.b * 4.0) / 4.0;
    c.b = mix(c.b, cel, 0.3 * uIntensity);
    vec3 warm = vec3(1.0, 0.9, 0.7);
    c *= warm;
    c = clamp(c, 0.0, 1.0);
    fragColor = vec4(c, color.a);
}
)";

        case FilterSystem::Filter_GenshinNight:
            return R"(
#version 300 es
precision highp float;
in vec2 vTexCoord;
out vec4 fragColor;
uniform sampler2D uTexture;
uniform float uIntensity;
uniform float uTime;
void main() {
    vec4 color = texture(uTexture, vTexCoord);
    vec3 c = color.rgb;
    c *= (1.0 - 0.25 * uIntensity);
    float gray = dot(c, vec3(0.299, 0.587, 0.114));
    float contrast = 1.0 + 0.3 * uIntensity;
    c = (c - 0.5) * contrast + 0.5;
    vec3 blueShift = vec3(0.7, 0.8, 1.0);
    c *= blueShift;
    vec3 shadow = vec3(0.1, 0.1, 0.25);
    float shadowMask = 1.0 - smoothstep(0.0, 0.5, gray);
    c = mix(c, shadow, shadowMask * 0.5 * uIntensity);
    c = clamp(c, 0.0, 1.0);
    fragColor = vec4(c, color.a);
}
)";

        case FilterSystem::Filter_GenshinDusk:
            return R"(
#version 300 es
precision highp float;
in vec2 vTexCoord;
out vec4 fragColor;
uniform sampler2D uTexture;
uniform float uIntensity;
uniform float uTime;
void main() {
    vec4 color = texture(uTexture, vTexCoord);
    vec3 c = color.rgb;
    float gray = dot(c, vec3(0.299, 0.587, 0.114));
    float saturation = 1.0 + 0.2 * uIntensity;
    c = mix(vec3(gray), c, saturation);
    vec3 sunsetOrange = vec3(1.0, 0.6, 0.2);
    vec3 sunsetPurple = vec3(0.6, 0.2, 0.8);
    float t = gray;
    vec3 duskColor = mix(sunsetOrange, sunsetPurple, t);
    c = mix(c, duskColor, 0.3 * uIntensity);
    c.r *= 1.1;
    c.b *= 0.9;
    c = clamp(c, 0.0, 1.0);
    fragColor = vec4(c, color.a);
}
)";

        case FilterSystem::Filter_None:
        default:
            return R"(
#version 300 es
precision highp float;
in vec2 vTexCoord;
out vec4 fragColor;
uniform sampler2D uTexture;
uniform float uIntensity;
uniform float uTime;
void main() {
    vec4 color = texture(uTexture, vTexCoord);
    fragColor = color;
}
)";
    }
}

FilterSystem::FilterSystem()
    : mCurrentFilter(Filter_None)
    , mIntensity(1.0f)
    , mWidth(0)
    , mHeight(0)
    , mVAO(0)
    , mVBO(0)
    , mProgram(0)
    , uTexture(-1)
    , uIntensity(-1)
    , uTime(-1) {
    LOGI("FilterSystem created");
}

FilterSystem::~FilterSystem() {
    destroy();
}

bool FilterSystem::initialize(int width, int height) {
    mWidth = width;
    mHeight = height;

    float vertices[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    if (!compileShaders()) {
        LOGE("Failed to compile initial shaders");
        return false;
    }

    LOGI("FilterSystem initialized (%dx%d)", width, height);
    return true;
}

void FilterSystem::destroy() {
    if (mVAO) {
        glDeleteVertexArrays(1, &mVAO);
        mVAO = 0;
    }
    if (mVBO) {
        glDeleteBuffers(1, &mVBO);
        mVBO = 0;
    }
    if (mProgram) {
        glDeleteProgram(mProgram);
        mProgram = 0;
    }
}

void FilterSystem::resize(int width, int height) {
    mWidth = width;
    mHeight = height;
}

void FilterSystem::setFilter(FilterType type) {
    mCurrentFilter = type;
    compileShaders();
    LOGI("Filter set to %s", getFilterName(type));
}

void FilterSystem::apply(GLuint inputTexture, int width, int height) {
    if (!mProgram) return;

    glUseProgram(mProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glUniform1i(uTexture, 0);
    glUniform1f(uIntensity, mIntensity);
    glUniform1f(uTime, 0.0f);

    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glUseProgram(0);
}

const char* FilterSystem::getFilterName(FilterType type) {
    switch (type) {
        case Filter_None:        return "None";
        case Filter_WhiteBright: return "White Bright";
        case Filter_SkinBright:  return "Skin Bright";
        case Filter_Dark:        return "Dark";
        case Filter_GenshinDay:  return "Genshin Day";
        case Filter_GenshinNight: return "Genshin Night";
        case Filter_GenshinDusk: return "Genshin Dusk";
        default:                 return "Unknown";
    }
}

bool FilterSystem::compileShaders() {
    const char* fragmentSource = getFilterFragmentShader(mCurrentFilter);
    GLuint program = createShader(gVertexShader, fragmentSource);
    if (!program) {
        LOGE("Failed to compile filter shader");
        return false;
    }

    if (mProgram) {
        glDeleteProgram(mProgram);
    }
    mProgram = program;

    glUseProgram(mProgram);
    uTexture = glGetUniformLocation(mProgram, "uTexture");
    uIntensity = glGetUniformLocation(mProgram, "uIntensity");
    uTime = glGetUniformLocation(mProgram, "uTime");
    glUseProgram(0);

    return true;
}

GLuint FilterSystem::createShader(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    GLint compiled;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char* infoLog = new char[infoLen];
            glGetShaderInfoLog(vertexShader, infoLen, nullptr, infoLog);
            LOGE("Vertex shader compile error: %s", infoLog);
            delete[] infoLog;
        }
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char* infoLog = new char[infoLen];
            glGetShaderInfoLog(fragmentShader, infoLen, nullptr, infoLog);
            LOGE("Fragment shader compile error: %s", infoLog);
            delete[] infoLog;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char* infoLog = new char[infoLen];
            glGetProgramInfoLog(program, infoLen, nullptr, infoLog);
            LOGE("Program link error: %s", infoLog);
            delete[] infoLog;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}