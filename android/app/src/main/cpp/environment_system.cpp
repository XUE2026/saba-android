#include "environment_system.h"
#include <android/log.h>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>

#define LOG_TAG "EnvironmentSystem"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static const char* gEnvVertexShader = R"(
#version 300 es
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
uniform mat4 uViewProj;
uniform mat4 uModel;
uniform vec3 uLightDir;
uniform float uBrightness;
uniform float uContrast;
uniform float uToonIntensity;
out vec3 vColor;
void main() {
    vec3 worldPos = (uModel * vec4(aPosition, 1.0)).xyz;
    vec3 worldNormal = normalize(mat3(uModel) * aNormal);
    float diff = max(dot(worldNormal, -uLightDir), 0.0);
    float toonDiff = floor(diff * 3.0) / 3.0;
    diff = mix(diff, toonDiff, uToonIntensity);
    vec3 baseColor = aColor * uBrightness;
    baseColor = (baseColor - 0.5) * uContrast + 0.5;
    vColor = baseColor * (0.3 + 0.7 * diff);
    gl_Position = uViewProj * vec4(worldPos, 1.0);
}
)";

static const char* gEnvFragmentShader = R"(
#version 300 es
precision highp float;
in vec3 vColor;
out vec4 fragColor;
void main() {
    fragColor = vec4(vColor, 1.0);
}
)";

static const float kGroundSize = 50.0f;

static void generateCubeData(std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& colors) {
    float size = 1.0f;
    float h = size * 0.5f;

    float pos[] = {
        -h, -h, -h,  h, -h, -h,  h,  h, -h, -h, -h, -h,  h,  h, -h, -h,  h, -h,
        -h, -h,  h, -h,  h,  h,  h,  h,  h, -h, -h,  h,  h,  h,  h,  h, -h,  h,
        -h,  h, -h,  h,  h, -h,  h,  h,  h, -h,  h, -h,  h,  h,  h, -h,  h,  h,
        -h, -h, -h, -h, -h,  h,  h, -h,  h, -h, -h, -h,  h, -h,  h,  h, -h, -h,
         h, -h, -h,  h,  h, -h,  h,  h,  h,  h, -h, -h,  h,  h,  h,  h, -h,  h,
        -h, -h, -h, -h, -h,  h, -h,  h,  h, -h, -h, -h, -h,  h,  h, -h,  h, -h
    };

    float nrm[] = {
         0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,
         0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,
         0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,
         0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,
         1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,
        -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0
    };

    for (int i = 0; i < 36; i++) {
        vertices.push_back(pos[i * 3]);
        vertices.push_back(pos[i * 3 + 1]);
        vertices.push_back(pos[i * 3 + 2]);
        normals.push_back(nrm[i * 3]);
        normals.push_back(nrm[i * 3 + 1]);
        normals.push_back(nrm[i * 3 + 2]);
        colors.push_back(0.8f);
        colors.push_back(0.8f);
        colors.push_back(0.8f);
    }
}

static void generateSphereData(std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& colors, int subdivisions = 3) {
    struct Triangle {
        int v0, v1, v2;
    };
    std::vector<glm::vec3> verts;
    std::vector<Triangle> tris;

    float t = (1.0f + sqrtf(5.0f)) / 2.0f;
    verts.push_back(glm::vec3(-1,  t,  0));
    verts.push_back(glm::vec3( 1,  t,  0));
    verts.push_back(glm::vec3(-1, -t,  0));
    verts.push_back(glm::vec3( 1, -t,  0));
    verts.push_back(glm::vec3( 0, -1,  t));
    verts.push_back(glm::vec3( 0,  1,  t));
    verts.push_back(glm::vec3( 0, -1, -t));
    verts.push_back(glm::vec3( 0,  1, -t));
    verts.push_back(glm::vec3( t,  0, -1));
    verts.push_back(glm::vec3( t,  0,  1));
    verts.push_back(glm::vec3(-t,  0, -1));
    verts.push_back(glm::vec3(-t,  0,  1));

    tris.push_back({0, 11, 5});
    tris.push_back({0, 5, 1});
    tris.push_back({0, 1, 7});
    tris.push_back({0, 7, 10});
    tris.push_back({0, 10, 11});
    tris.push_back({1, 5, 9});
    tris.push_back({5, 11, 4});
    tris.push_back({11, 10, 2});
    tris.push_back({10, 7, 6});
    tris.push_back({7, 1, 8});
    tris.push_back({3, 9, 4});
    tris.push_back({3, 4, 2});
    tris.push_back({3, 2, 6});
    tris.push_back({3, 6, 8});
    tris.push_back({3, 8, 9});
    tris.push_back({4, 9, 5});
    tris.push_back({2, 4, 11});
    tris.push_back({6, 2, 10});
    tris.push_back({8, 6, 7});
    tris.push_back({9, 8, 1});

    for (int s = 0; s < subdivisions; s++) {
        std::vector<Triangle> newTris;
        for (const auto& tri : tris) {
            int a = (int)verts.size();
            verts.push_back(glm::normalize((verts[tri.v0] + verts[tri.v1]) * 0.5f));
            int b = (int)verts.size();
            verts.push_back(glm::normalize((verts[tri.v1] + verts[tri.v2]) * 0.5f));
            int c = (int)verts.size();
            verts.push_back(glm::normalize((verts[tri.v2] + verts[tri.v0]) * 0.5f));
            newTris.push_back({tri.v0, a, c});
            newTris.push_back({tri.v1, b, a});
            newTris.push_back({tri.v2, c, b});
            newTris.push_back({a, b, c});
        }
        tris = newTris;
    }

    for (const auto& tri : tris) {
        glm::vec3 v0 = verts[tri.v0];
        glm::vec3 v1 = verts[tri.v1];
        glm::vec3 v2 = verts[tri.v2];
        vertices.push_back(v0.x); vertices.push_back(v0.y); vertices.push_back(v0.z);
        vertices.push_back(v1.x); vertices.push_back(v1.y); vertices.push_back(v1.z);
        vertices.push_back(v2.x); vertices.push_back(v2.y); vertices.push_back(v2.z);
        normals.push_back(v0.x); normals.push_back(v0.y); normals.push_back(v0.z);
        normals.push_back(v1.x); normals.push_back(v1.y); normals.push_back(v1.z);
        normals.push_back(v2.x); normals.push_back(v2.y); normals.push_back(v2.z);
        for (int i = 0; i < 3; i++) {
            colors.push_back(0.7f);
            colors.push_back(0.7f);
            colors.push_back(0.9f);
        }
    }
}

static void generateConeData(std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& colors, int segments = 24) {
    float h = 1.0f;
    float r = 0.5f;

    for (int i = 0; i < segments; i++) {
        float a0 = (float)i / (float)segments * 2.0f * 3.14159265f;
        float a1 = (float)(i + 1) / (float)segments * 2.0f * 3.14159265f;
        float x0 = cosf(a0) * r;
        float z0 = sinf(a0) * r;
        float x1 = cosf(a1) * r;
        float z1 = sinf(a1) * r;

        vertices.push_back(0.0f); vertices.push_back(h); vertices.push_back(0.0f);
        vertices.push_back(x0); vertices.push_back(-h); vertices.push_back(z0);
        vertices.push_back(x1); vertices.push_back(-h); vertices.push_back(z1);

        glm::vec3 n0 = glm::normalize(glm::vec3(x0, 0.0f, z0));
        glm::vec3 n1 = glm::normalize(glm::vec3(x1, 0.0f, z1));
        normals.push_back(n0.x); normals.push_back(0.5f); normals.push_back(n0.z);
        normals.push_back(n0.x); normals.push_back(-0.3f); normals.push_back(n0.z);
        normals.push_back(n1.x); normals.push_back(-0.3f); normals.push_back(n1.z);

        for (int j = 0; j < 3; j++) {
            colors.push_back(0.8f);
            colors.push_back(0.7f);
            colors.push_back(0.6f);
        }

        vertices.push_back(0.0f); vertices.push_back(-h); vertices.push_back(0.0f);
        vertices.push_back(x1); vertices.push_back(-h); vertices.push_back(z1);
        vertices.push_back(x0); vertices.push_back(-h); vertices.push_back(z0);

        glm::vec3 up(0.0f, -1.0f, 0.0f);
        normals.push_back(up.x); normals.push_back(up.y); normals.push_back(up.z);
        normals.push_back(up.x); normals.push_back(up.y); normals.push_back(up.z);
        normals.push_back(up.x); normals.push_back(up.y); normals.push_back(up.z);

        for (int j = 0; j < 3; j++) {
            colors.push_back(0.8f);
            colors.push_back(0.7f);
            colors.push_back(0.6f);
        }
    }
}

static void generateCylinderData(std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& colors, int segments = 24) {
    float h = 1.0f;
    float r = 0.5f;

    for (int i = 0; i < segments; i++) {
        float a0 = (float)i / (float)segments * 2.0f * 3.14159265f;
        float a1 = (float)(i + 1) / (float)segments * 2.0f * 3.14159265f;
        float x0 = cosf(a0) * r;
        float z0 = sinf(a0) * r;
        float x1 = cosf(a1) * r;
        float z1 = sinf(a1) * r;

        vertices.push_back(x0); vertices.push_back(-h); vertices.push_back(z0);
        vertices.push_back(x1); vertices.push_back(-h); vertices.push_back(z1);
        vertices.push_back(x0); vertices.push_back(h); vertices.push_back(z0);
        vertices.push_back(x1); vertices.push_back(-h); vertices.push_back(z1);
        vertices.push_back(x1); vertices.push_back(h); vertices.push_back(z1);
        vertices.push_back(x0); vertices.push_back(h); vertices.push_back(z0);

        glm::vec3 n0 = glm::normalize(glm::vec3(x0, 0.0f, z0));
        glm::vec3 n1 = glm::normalize(glm::vec3(x1, 0.0f, z1));
        for (int j = 0; j < 6; j++) {
            float nx = (j < 3) ? n0.x : n1.x;
            float nz = (j < 3) ? n0.z : n1.z;
            if (j % 3 == 2) { nx = n0.x; nz = n0.z; }
            normals.push_back(nx); normals.push_back(0.0f); normals.push_back(nz);
        }

        for (int j = 0; j < 6; j++) {
            colors.push_back(0.6f);
            colors.push_back(0.6f);
            colors.push_back(0.8f);
        }

        vertices.push_back(0.0f); vertices.push_back(h); vertices.push_back(0.0f);
        vertices.push_back(x1); vertices.push_back(h); vertices.push_back(z1);
        vertices.push_back(x0); vertices.push_back(h); vertices.push_back(z0);
        normals.push_back(0.0f); normals.push_back(1.0f); normals.push_back(0.0f);
        normals.push_back(0.0f); normals.push_back(1.0f); normals.push_back(0.0f);
        normals.push_back(0.0f); normals.push_back(1.0f); normals.push_back(0.0f);
        for (int j = 0; j < 3; j++) { colors.push_back(0.6f); colors.push_back(0.6f); colors.push_back(0.8f); }

        vertices.push_back(0.0f); vertices.push_back(-h); vertices.push_back(0.0f);
        vertices.push_back(x0); vertices.push_back(-h); vertices.push_back(z0);
        vertices.push_back(x1); vertices.push_back(-h); vertices.push_back(z1);
        normals.push_back(0.0f); normals.push_back(-1.0f); normals.push_back(0.0f);
        normals.push_back(0.0f); normals.push_back(-1.0f); normals.push_back(0.0f);
        normals.push_back(0.0f); normals.push_back(-1.0f); normals.push_back(0.0f);
        for (int j = 0; j < 3; j++) { colors.push_back(0.6f); colors.push_back(0.6f); colors.push_back(0.8f); }
    }
}

EnvironmentSystem::EnvironmentSystem()
    : mBrightness(1.0f)
    , mContrast(1.0f)
    , mToonIntensity(0.0f)
    , mGroundType(Ground_Floor)
    , mGroundVAO(0)
    , mProgram(0) {
    LOGI("EnvironmentSystem created");
}

EnvironmentSystem::~EnvironmentSystem() {
    destroy();
}

static GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexSrc, nullptr);
    glCompileShader(vs);
    GLint compiled;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint len = 0;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char* log = new char[len];
            glGetShaderInfoLog(vs, len, nullptr, log);
            LOGE("Env vertex shader error: %s", log);
            delete[] log;
        }
        glDeleteShader(vs);
        return 0;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentSrc, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint len = 0;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char* log = new char[len];
            glGetShaderInfoLog(fs, len, nullptr, log);
            LOGE("Env fragment shader error: %s", log);
            delete[] log;
        }
        glDeleteShader(vs);
        glDeleteShader(fs);
        return 0;
    }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    GLint linked;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char* log = new char[len];
            glGetProgramInfoLog(prog, len, nullptr, log);
            LOGE("Env program link error: %s", log);
            delete[] log;
        }
        glDeleteShader(vs);
        glDeleteShader(fs);
        glDeleteProgram(prog);
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

bool EnvironmentSystem::initialize() {
    mProgram = createShaderProgram(gEnvVertexShader, gEnvFragmentShader);
    if (!mProgram) {
        LOGE("Failed to create environment shader program");
        return false;
    }

    float gs = kGroundSize;
    float groundVerts[] = {
        -gs, 0.0f, -gs,
         gs, 0.0f, -gs,
        -gs, 0.0f,  gs,
         gs, 0.0f,  gs
    };
    float groundNormals[] = {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    float groundColors[] = {
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f
    };

    GLuint vbo[3];
    glGenVertexArrays(1, &mGroundVAO);
    glGenBuffers(3, vbo);

    glBindVertexArray(mGroundVAO);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVerts), groundVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundNormals), groundNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundColors), groundColors, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    LOGI("EnvironmentSystem initialized");
    return true;
}

void EnvironmentSystem::destroy() {
    for (auto& prim : mPrimitives) {
        if (prim.vao) {
            glDeleteVertexArrays(1, &prim.vao);
        }
    }
    mPrimitives.clear();

    if (mGroundVAO) {
        GLuint vbo[3];
        glBindVertexArray(mGroundVAO);
        glGetVertexAttribPointerv(0, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, (GLvoid**)&vbo[0]);
        glDeleteBuffers(3, vbo);
        glDeleteVertexArrays(1, &mGroundVAO);
        mGroundVAO = 0;
    }

    if (mProgram) {
        glDeleteProgram(mProgram);
        mProgram = 0;
    }
}

static void getGroundColor(EnvironmentSystem::GroundType type, float r, float g, float b) {
    switch (type) {
        case EnvironmentSystem::Ground_Floor:
            r = 0.5f; g = 0.5f; b = 0.5f; break;
        case EnvironmentSystem::Ground_Grass:
            r = 0.2f; g = 0.6f; b = 0.2f; break;
        case EnvironmentSystem::Ground_Dirt:
            r = 0.5f; g = 0.35f; b = 0.2f; break;
        case EnvironmentSystem::Ground_Asphalt:
            r = 0.25f; g = 0.25f; b = 0.25f; break;
        case EnvironmentSystem::Ground_Sky:
            r = 0.4f; g = 0.6f; b = 0.9f; break;
        default:
            r = 0.5f; g = 0.5f; b = 0.5f; break;
    }
}

void EnvironmentSystem::addPrimitive(PrimitiveType type, const glm::vec3& position, const glm::vec3& scale) {
    Primitive prim;
    prim.type = type;
    prim.position = position;
    prim.scale = scale;
    prim.vao = 0;
    prim.vertexCount = 0;

    switch (type) {
        case Primitive_Sun:
            prim.color = glm::vec3(1.0f, 0.9f, 0.5f);
            break;
        case Primitive_Cube:
            prim.color = glm::vec3(0.8f, 0.8f, 0.8f);
            break;
        case Primitive_Sphere:
            prim.color = glm::vec3(0.7f, 0.7f, 0.9f);
            break;
        case Primitive_Cone:
            prim.color = glm::vec3(0.8f, 0.7f, 0.6f);
            break;
        case Primitive_Cylinder:
            prim.color = glm::vec3(0.6f, 0.6f, 0.8f);
            break;
        default:
            prim.color = glm::vec3(1.0f, 1.0f, 1.0f);
            break;
    }

    if (createPrimitiveGeometry(prim)) {
        mPrimitives.push_back(prim);
        LOGI("Added primitive type=%d at (%.2f, %.2f, %.2f)", type, position.x, position.y, position.z);
    }
}

bool EnvironmentSystem::createPrimitiveGeometry(Primitive& prim) {
    std::vector<float> verts;
    std::vector<float> norms;
    std::vector<float> cols;

    switch (prim.type) {
        case Primitive_Cube:
            generateCubeData(verts, norms, cols);
            break;
        case Primitive_Sphere:
            generateSphereData(verts, norms, cols);
            break;
        case Primitive_Cone:
            generateConeData(verts, norms, cols);
            break;
        case Primitive_Cylinder:
            generateCylinderData(verts, norms, cols);
            break;
        default:
            return false;
    }

    prim.vertexCount = (int)verts.size() / 3;

    GLuint vbo[3];
    glGenVertexArrays(1, &prim.vao);
    glGenBuffers(3, vbo);

    glBindVertexArray(prim.vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, norms.size() * sizeof(float), norms.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, cols.size() * sizeof(float), cols.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return true;
}

void EnvironmentSystem::removePrimitive(int index) {
    if (index >= 0 && index < (int)mPrimitives.size()) {
        if (mPrimitives[index].vao) {
            glDeleteVertexArrays(1, &mPrimitives[index].vao);
        }
        mPrimitives.erase(mPrimitives.begin() + index);
    }
}

void EnvironmentSystem::clearPrimitives() {
    for (auto& prim : mPrimitives) {
        if (prim.vao) {
            glDeleteVertexArrays(1, &prim.vao);
        }
    }
    mPrimitives.clear();
}

void EnvironmentSystem::render(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& lightDir) {
    if (!mProgram) return;

    glUseProgram(mProgram);

    glm::mat4 viewProj = proj * view;

    GLint uViewProj = glGetUniformLocation(mProgram, "uViewProj");
    GLint uModel = glGetUniformLocation(mProgram, "uModel");
    GLint uLightDir = glGetUniformLocation(mProgram, "uLightDir");
    GLint uBrightness = glGetUniformLocation(mProgram, "uBrightness");
    GLint uContrast = glGetUniformLocation(mProgram, "uContrast");
    GLint uToonIntensity = glGetUniformLocation(mProgram, "uToonIntensity");

    glUniformMatrix4fv(uViewProj, 1, GL_FALSE, &viewProj[0][0]);
    glUniform3fv(uLightDir, 1, &lightDir[0]);
    glUniform1f(uBrightness, mBrightness);
    glUniform1f(uContrast, mContrast);
    glUniform1f(uToonIntensity, mToonIntensity);

    glUniformMatrix4fv(uModel, 1, GL_FALSE, &glm::mat4(1.0f)[0][0]);

    glBindVertexArray(mGroundVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    for (const auto& prim : mPrimitives) {
        if (!prim.vao) continue;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), prim.position);
        model = glm::scale(model, prim.scale);
        glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);

        glBindVertexArray(prim.vao);
        glDrawArrays(GL_TRIANGLES, 0, prim.vertexCount);
        glBindVertexArray(0);
    }

    glUseProgram(0);
}

void EnvironmentSystem::loadScene(const std::string& path) {
    LOGI("Loading scene from: %s", path.c_str());
    clearPrimitives();

    std::ifstream file(path);
    if (!file.is_open()) {
        LOGE("Failed to open scene file: %s", path.c_str());
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string typeStr;
        float px, py, pz, sx, sy, sz;
        if (!(iss >> typeStr >> px >> py >> pz >> sx >> sy >> sz)) continue;

        PrimitiveType ptype = Primitive_Cube;
        if (typeStr == "cube") ptype = Primitive_Cube;
        else if (typeStr == "sphere") ptype = Primitive_Sphere;
        else if (typeStr == "cone") ptype = Primitive_Cone;
        else if (typeStr == "cylinder") ptype = Primitive_Cylinder;
        else if (typeStr == "sun") ptype = Primitive_Sun;
        else {
            LOGE("Unknown primitive type: %s", typeStr.c_str());
            continue;
        }

        addPrimitive(ptype, glm::vec3(px, py, pz), glm::vec3(sx, sy, sz));
    }

    file.close();
    LOGI("Scene loaded: %zu primitives", mPrimitives.size());
}