#ifndef ENVIRONMENT_SYSTEM_H_
#define ENVIRONMENT_SYSTEM_H_

#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

class EnvironmentSystem {
public:
    enum PrimitiveType {
        Primitive_Sun = 0,
        Primitive_Cube,
        Primitive_Sphere,
        Primitive_Cone,
        Primitive_Cylinder,
        Primitive_Count
    };

    enum GroundType {
        Ground_Floor = 0,
        Ground_Grass,
        Ground_Dirt,
        Ground_Asphalt,
        Ground_Sky,
        Ground_Count
    };

    EnvironmentSystem();
    ~EnvironmentSystem();

    bool initialize();
    void destroy();

    void setBrightness(float value) { mBrightness = value; }
    float getBrightness() const { return mBrightness; }
    void setContrast(float value) { mContrast = value; }
    float getContrast() const { return mContrast; }
    void setToonIntensity(float value) { mToonIntensity = value; }
    float getToonIntensity() const { return mToonIntensity; }

    void setGroundType(GroundType type) { mGroundType = type; }
    GroundType getGroundType() const { return mGroundType; }

    void addPrimitive(PrimitiveType type, const glm::vec3& position, const glm::vec3& scale);
    void removePrimitive(int index);
    void clearPrimitives();

    void render(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& lightDir);

    void loadScene(const std::string& path);

private:
    struct Primitive {
        PrimitiveType type;
        glm::vec3 position;
        glm::vec3 scale;
        glm::vec3 color;
        GLuint vao;
        int vertexCount;
    };

    bool createPrimitiveGeometry(Primitive& prim);

    float mBrightness;
    float mContrast;
    float mToonIntensity;
    GroundType mGroundType;

    GLuint mGroundVAO;
    GLuint mProgram;

    std::vector<Primitive> mPrimitives;
};

#endif