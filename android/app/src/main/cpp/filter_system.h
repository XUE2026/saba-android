#ifndef FILTER_SYSTEM_H_
#define FILTER_SYSTEM_H_

#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <string>

class FilterSystem {
public:
    enum FilterType {
        Filter_None = 0,
        Filter_WhiteBright,
        Filter_SkinBright,
        Filter_Dark,
        Filter_GenshinDay,
        Filter_GenshinNight,
        Filter_GenshinDusk,
        Filter_Count
    };

    FilterSystem();
    ~FilterSystem();

    bool initialize(int width, int height);
    void destroy();
    void resize(int width, int height);

    void setFilter(FilterType type);
    FilterType getFilter() const { return mCurrentFilter; }
    void setIntensity(float intensity) { mIntensity = intensity; }
    float getIntensity() const { return mIntensity; }

    void apply(GLuint inputTexture, int width, int height);

    static const char* getFilterName(FilterType type);

private:
    bool compileShaders();
    GLuint createShader(const char* vertexSource, const char* fragmentSource);

    FilterType mCurrentFilter;
    float mIntensity;
    int mWidth;
    int mHeight;

    GLuint mVAO;
    GLuint mVBO;
    GLuint mProgram;

    GLint uTexture;
    GLint uIntensity;
    GLint uTime;
};

#endif