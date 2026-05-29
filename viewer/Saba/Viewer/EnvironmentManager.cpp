//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "EnvironmentManager.h"

#include <Saba/GL/GLSLUtil.h>
#include <Saba/GL/GLShaderUtil.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include <vector>
#include <cmath>

namespace saba
{
    static const char* g_primitiveVS = R"(
        #version 140
        in vec3 in_Pos;
        in vec3 in_Normal;

        out vec3 vs_Normal;
        out vec3 vs_WorldPos;

        uniform mat4 u_WVP;
        uniform mat4 u_World;

        void main()
        {
            vec4 worldPos = u_World * vec4(in_Pos, 1.0);
            vs_WorldPos = worldPos.xyz;
            vs_Normal = mat3(u_World) * in_Normal;
            gl_Position = u_WVP * vec4(in_Pos, 1.0);
        }
    )";

    static const char* g_primitiveFS = R"(
        #version 140
        in vec3 vs_Normal;
        in vec3 vs_WorldPos;

        out vec4 fs_Color;

        uniform vec3 u_Color;
        uniform vec3 u_LightDir;
        uniform vec3 u_LightColor;
        uniform float u_LightIntensity;
        uniform vec3 u_ViewPos;

        void main()
        {
            vec3 N = normalize(vs_Normal);
            vec3 L = normalize(-u_LightDir);
            float diff = max(dot(N, L), 0.0);
            vec3 V = normalize(u_ViewPos - vs_WorldPos);
            vec3 H = normalize(L + V);
            float spec = pow(max(dot(N, H), 0.0), 32.0);

            vec3 ambient = u_Color * 0.15;
            vec3 diffuse = u_Color * diff * u_LightColor * u_LightIntensity;
            vec3 specular = vec3(0.3) * spec * u_LightIntensity;

            fs_Color.rgb = ambient + diffuse + specular;
            fs_Color.a = 1.0;
        }
    )";

    static const char* g_groundVS = R"(
        #version 140
        in vec3 in_Pos;
        in vec3 in_Normal;

        out vec3 vs_Normal;
        out vec3 vs_WorldPos;

        uniform mat4 u_WVP;
        uniform mat4 u_World;

        void main()
        {
            vec4 worldPos = u_World * vec4(in_Pos, 1.0);
            vs_WorldPos = worldPos.xyz;
            vs_Normal = mat3(u_World) * in_Normal;
            gl_Position = u_WVP * vec4(in_Pos, 1.0);
        }
    )";

    static const char* g_groundFS = R"(
        #version 140
        in vec3 vs_Normal;
        in vec3 vs_WorldPos;

        out vec4 fs_Color;

        uniform vec3 u_Color;
        uniform vec3 u_LightDir;
        uniform vec3 u_LightColor;
        uniform float u_LightIntensity;

        void main()
        {
            vec3 N = normalize(vs_Normal);
            vec3 L = normalize(-u_LightDir);
            float diff = max(dot(N, L), 0.0);

            vec3 ambient = u_Color * 0.2;
            vec3 diffuse = u_Color * diff * u_LightColor * u_LightIntensity;

            fs_Color.rgb = ambient + diffuse;
            fs_Color.a = 1.0;
        }
    )";

    static const char* g_billboardVS = R"(
        #version 140
        in vec3 in_Pos;
        in vec2 in_UV;

        out vec2 vs_UV;

        uniform mat4 u_WVP;

        void main()
        {
            vs_UV = in_UV;
            gl_Position = u_WVP * vec4(in_Pos, 1.0);
        }
    )";

    static const char* g_billboardFS = R"(
        #version 140
        in vec2 vs_UV;

        out vec4 fs_Color;

        uniform vec4 u_Color;

        void main()
        {
            vec2 center = vs_UV - vec2(0.5);
            float dist = length(center);
            if (dist > 0.5)
                discard;
            float alpha = smoothstep(0.5, 0.3, dist);
            fs_Color = vec4(u_Color.rgb, u_Color.a * alpha);
        }
    )";

    static const char* g_sunVS = R"(
        #version 140
        in vec3 in_Pos;
        in vec2 in_UV;

        out vec2 vs_UV;

        uniform mat4 u_WVP;

        void main()
        {
            vs_UV = in_UV;
            gl_Position = u_WVP * vec4(in_Pos, 1.0);
        }
    )";

    static const char* g_sunFS = R"(
        #version 140
        in vec2 vs_UV;

        out vec4 fs_Color;

        uniform vec3 u_Color;
        uniform float u_Intensity;

        void main()
        {
            vec2 center = vs_UV - vec2(0.5);
            float dist = length(center);
            if (dist > 0.5)
                discard;
            float glow = exp(-dist * 6.0);
            float core = smoothstep(0.5, 0.0, dist);
            vec3 col = u_Color * (core * u_Intensity + glow * 0.5);
            fs_Color = vec4(col, core * 0.9);
        }
    )";

    EnvironmentManager::EnvironmentManager()
        : m_initialized(false)
    {
        m_sun.m_direction = glm::vec3(-0.5f, -1.0f, -0.5f);
        m_sun.m_intensity = 1.0f;
        m_sun.m_color = glm::vec3(1.0f, 0.95f, 0.8f);

        m_ground.m_type = GroundType::Floor;
        m_ground.m_color = GetGroundColorForType(GroundType::Floor);
    }

    bool EnvironmentManager::Initialize(const ViewerContext& ctxt)
    {
        m_primitiveProg = CreateShaderProgram(g_primitiveVS, g_primitiveFS);
        if (m_primitiveProg == 0)
        {
            return false;
        }
        m_primitive_uWVP = glGetUniformLocation(m_primitiveProg, "u_WVP");
        m_primitive_uWorld = glGetUniformLocation(m_primitiveProg, "u_World");
        m_primitive_uColor = glGetUniformLocation(m_primitiveProg, "u_Color");
        m_primitive_uLightDir = glGetUniformLocation(m_primitiveProg, "u_LightDir");
        m_primitive_uLightColor = glGetUniformLocation(m_primitiveProg, "u_LightColor");
        m_primitive_uLightIntensity = glGetUniformLocation(m_primitiveProg, "u_LightIntensity");
        m_primitive_uViewPos = glGetUniformLocation(m_primitiveProg, "u_ViewPos");

        m_groundProg = CreateShaderProgram(g_groundVS, g_groundFS);
        if (m_groundProg == 0)
        {
            return false;
        }
        m_ground_uWVP = glGetUniformLocation(m_groundProg, "u_WVP");
        m_ground_uWorld = glGetUniformLocation(m_groundProg, "u_World");
        m_ground_uColor = glGetUniformLocation(m_groundProg, "u_Color");
        m_ground_uLightDir = glGetUniformLocation(m_groundProg, "u_LightDir");
        m_ground_uLightColor = glGetUniformLocation(m_groundProg, "u_LightColor");
        m_ground_uLightIntensity = glGetUniformLocation(m_groundProg, "u_LightIntensity");

        m_billboardProg = CreateShaderProgram(g_billboardVS, g_billboardFS);
        if (m_billboardProg == 0)
        {
            return false;
        }
        m_billboard_uWVP = glGetUniformLocation(m_billboardProg, "u_WVP");
        m_billboard_uColor = glGetUniformLocation(m_billboardProg, "u_Color");

        m_sunProg = CreateShaderProgram(g_sunVS, g_sunFS);
        if (m_sunProg == 0)
        {
            return false;
        }
        m_sun_uWVP = glGetUniformLocation(m_sunProg, "u_WVP");
        m_sun_uColor = glGetUniformLocation(m_sunProg, "u_Color");
        m_sun_uIntensity = glGetUniformLocation(m_sunProg, "u_Intensity");

        BuildPrimitiveMeshes();
        BuildGroundMesh();
        BuildBillboardMesh();

        m_initialized = true;
        return true;
    }

    void EnvironmentManager::Uninitialize()
    {
        m_primitiveProg.Destroy();
        m_groundProg.Destroy();
        m_billboardProg.Destroy();
        m_sunProg.Destroy();

        m_primitives.clear();
        m_envObjects.clear();
        m_primitiveMeshes.clear();

        m_initialized = false;
    }

    void EnvironmentManager::SetSunDirection(float x, float y, float z)
    {
        m_sun.m_direction = glm::normalize(glm::vec3(x, y, z));
    }

    void EnvironmentManager::SetSunIntensity(float intensity)
    {
        m_sun.m_intensity = glm::clamp(intensity, 0.0f, 10.0f);
    }

    void EnvironmentManager::SetSunColor(float r, float g, float b)
    {
        m_sun.m_color = glm::vec3(r, g, b);
    }

    void EnvironmentManager::AddPrimitive(PrimitiveType type, const glm::vec3& size,
        const glm::vec3& pos, const glm::vec4& color)
    {
        Primitive prim;
        prim.m_type = type;
        prim.m_size = size;
        prim.m_position = pos;
        prim.m_rotation = glm::vec3(0.0f);
        prim.m_color = color;
        m_primitives.push_back(prim);
    }

    void EnvironmentManager::SetGroundType(GroundType groundType)
    {
        m_ground.m_type = groundType;
        m_ground.m_color = GetGroundColorForType(groundType);
    }

    void EnvironmentManager::AddEnvironmentObject(EnvObjectType objType, const glm::vec3& pos,
        const glm::vec3& size, const glm::vec4& color)
    {
        BillboardObject obj;
        obj.m_type = objType;
        obj.m_position = pos;
        obj.m_size = size;
        obj.m_color = color;
        m_envObjects.push_back(obj);
    }

    void EnvironmentManager::RemovePrimitive(size_t index)
    {
        if (index < m_primitives.size())
        {
            m_primitives.erase(m_primitives.begin() + index);
        }
    }

    void EnvironmentManager::RemoveEnvironmentObject(size_t index)
    {
        if (index < m_envObjects.size())
        {
            m_envObjects.erase(m_envObjects.begin() + index);
        }
    }

    void EnvironmentManager::ClearPrimitives()
    {
        m_primitives.clear();
    }

    void EnvironmentManager::ClearEnvironmentObjects()
    {
        m_envObjects.clear();
    }

    glm::vec4 EnvironmentManager::GetGroundColorForType(GroundType type) const
    {
        switch (type)
        {
        case GroundType::Floor:
            return glm::vec4(0.4f, 0.4f, 0.45f, 1.0f);
        case GroundType::Grass:
            return glm::vec4(0.3f, 0.6f, 0.2f, 1.0f);
        case GroundType::Mud:
            return glm::vec4(0.45f, 0.35f, 0.25f, 1.0f);
        case GroundType::Asphalt:
            return glm::vec4(0.25f, 0.25f, 0.28f, 1.0f);
        case GroundType::Sky:
            return glm::vec4(0.5f, 0.7f, 1.0f, 1.0f);
        case GroundType::Field:
            return glm::vec4(0.55f, 0.5f, 0.3f, 1.0f);
        default:
            return glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
    }

    EnvironmentManager::PrimitiveMesh EnvironmentManager::CreateCubeMesh()
    {
        PrimitiveMesh mesh;
        mesh.m_indexCount = 36;

        std::vector<glm::vec3> positions = {
            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3( 0.5f, -0.5f, -0.5f),
            glm::vec3( 0.5f,  0.5f, -0.5f),
            glm::vec3(-0.5f,  0.5f, -0.5f),
            glm::vec3(-0.5f, -0.5f,  0.5f),
            glm::vec3( 0.5f, -0.5f,  0.5f),
            glm::vec3( 0.5f,  0.5f,  0.5f),
            glm::vec3(-0.5f,  0.5f,  0.5f),
        };

        std::vector<glm::vec3> normals = {
            glm::vec3( 0.0f,  0.0f, -1.0f),
            glm::vec3( 0.0f,  0.0f,  1.0f),
            glm::vec3( 1.0f,  0.0f,  0.0f),
            glm::vec3(-1.0f,  0.0f,  0.0f),
            glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3( 0.0f, -1.0f,  0.0f),
        };

        std::vector<uint16_t> indices = {
            0,1,2, 0,2,3,
            5,4,7, 5,7,6,
            1,5,6, 1,6,2,
            4,0,3, 4,3,7,
            3,2,6, 3,6,7,
            4,5,1, 4,1,0,
        };

        std::vector<glm::vec3> vertPos;
        std::vector<glm::vec3> vertNorms;
        for (size_t i = 0; i < indices.size(); i++)
        {
            uint16_t idx = indices[i];
            vertPos.push_back(positions[idx]);

            int faceIdx = static_cast<int>(i / 6);
            vertNorms.push_back(normals[faceIdx]);
        }

        mesh.m_posVBO = CreateVBO(vertPos);
        mesh.m_normalVBO = CreateVBO(vertNorms);
        mesh.m_posBinder = MakeVertexBinder<glm::vec3>();
        mesh.m_normalBinder = MakeVertexBinder<glm::vec3>();

        if (mesh.m_vao.Create())
        {
            glBindVertexArray(mesh.m_vao);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.m_posVBO);
            mesh.m_posBinder.Bind(0, mesh.m_posVBO);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.m_normalVBO);
            mesh.m_normalBinder.Bind(1, mesh.m_normalVBO);
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
        }

        return mesh;
    }

    EnvironmentManager::PrimitiveMesh EnvironmentManager::CreateSphereMesh(int segments)
    {
        PrimitiveMesh mesh;

        std::vector<glm::vec3> vertPos;
        std::vector<glm::vec3> vertNorms;
        std::vector<uint16_t> indices;

        for (int lat = 0; lat <= segments; lat++)
        {
            float theta = glm::pi<float>() * static_cast<float>(lat) / static_cast<float>(segments);
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            for (int lon = 0; lon <= segments; lon++)
            {
                float phi = 2.0f * glm::pi<float>() * static_cast<float>(lon) / static_cast<float>(segments);
                float sinPhi = std::sin(phi);
                float cosPhi = std::cos(phi);

                glm::vec3 norm(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);
                vertPos.push_back(norm * 0.5f);
                vertNorms.push_back(norm);
            }
        }

        for (int lat = 0; lat < segments; lat++)
        {
            for (int lon = 0; lon < segments; lon++)
            {
                int first = lat * (segments + 1) + lon;
                int second = first + segments + 1;

                indices.push_back(static_cast<uint16_t>(first));
                indices.push_back(static_cast<uint16_t>(second));
                indices.push_back(static_cast<uint16_t>(first + 1));

                indices.push_back(static_cast<uint16_t>(second));
                indices.push_back(static_cast<uint16_t>(second + 1));
                indices.push_back(static_cast<uint16_t>(first + 1));
            }
        }

        std::vector<glm::vec3> expandedPos;
        std::vector<glm::vec3> expandedNorms;
        expandedPos.reserve(indices.size());
        expandedNorms.reserve(indices.size());
        for (size_t i = 0; i < indices.size(); i++)
        {
            expandedPos.push_back(vertPos[indices[i]]);
            expandedNorms.push_back(vertNorms[indices[i]]);
        }

        mesh.m_indexCount = static_cast<GLsizei>(indices.size());
        mesh.m_posVBO = CreateVBO(expandedPos);
        mesh.m_normalVBO = CreateVBO(expandedNorms);
        mesh.m_posBinder = MakeVertexBinder<glm::vec3>();
        mesh.m_normalBinder = MakeVertexBinder<glm::vec3>();

        if (mesh.m_vao.Create())
        {
            glBindVertexArray(mesh.m_vao);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.m_posVBO);
            mesh.m_posBinder.Bind(0, mesh.m_posVBO);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.m_normalVBO);
            mesh.m_normalBinder.Bind(1, mesh.m_normalVBO);
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
        }

        return mesh;
    }

    EnvironmentManager::PrimitiveMesh EnvironmentManager::CreateConeMesh(int segments)
    {
        PrimitiveMesh mesh;

        std::vector<glm::vec3> vertPos;
        std::vector<glm::vec3> vertNorms;

        glm::vec3 apex(0.0f, 0.5f, 0.0f);

        for (int i = 0; i <= segments; i++)
        {
            float angle = 2.0f * glm::pi<float>() * static_cast<float>(i) / static_cast<float>(segments);
            float x = 0.5f * std::cos(angle);
            float z = 0.5f * std::sin(angle);
            glm::vec3 basePos(x, -0.5f, z);

            glm::vec3 baseToApex = apex - basePos;
            glm::vec3 up(0.0f, 1.0f, 0.0f);
            glm::vec3 side = glm::normalize(glm::cross(baseToApex, up));
            glm::vec3 norm = glm::normalize(glm::cross(side, baseToApex));
            if (norm.y < 0.0f) norm = -norm;

            vertPos.push_back(basePos);
            vertNorms.push_back(norm);
        }

        for (int i = 0; i < segments; i++)
        {
            int next = (i + 1) % (segments + 1);
            vertPos.push_back(vertPos[i]);
            vertNorms.push_back(vertNorms[i]);
            vertPos.push_back(vertPos[next]);
            vertNorms.push_back(vertNorms[next]);
            vertPos.push_back(apex);
            vertNorms.push_back(glm::normalize(vertNorms[i] + vertNorms[next]));
        }

        glm::vec3 bottomNormal(0.0f, -1.0f, 0.0f);
        for (int i = 0; i <= segments; i++)
        {
            float angle = 2.0f * glm::pi<float>() * static_cast<float>(i) / static_cast<float>(segments);
            float x = 0.5f * std::cos(angle);
            float z = 0.5f * std::sin(angle);
            vertPos.push_back(glm::vec3(x, -0.5f, z));
            vertNorms.push_back(bottomNormal);
        }

        int baseStart = segments * 3;
        for (int i = 0; i < segments; i++)
        {
            int next = (i + 1) % (segments + 1);
            vertPos.push_back(glm::vec3(0.0f, -0.5f, 0.0f));
            vertNorms.push_back(bottomNormal);
            vertPos.push_back(vertPos[baseStart + i]);
            vertNorms.push_back(bottomNormal);
            vertPos.push_back(vertPos[baseStart + next]);
            vertNorms.push_back(bottomNormal);
        }

        mesh.m_indexCount = static_cast<GLsizei>(vertPos.size());
        mesh.m_posVBO = CreateVBO(vertPos);
        mesh.m_normalVBO = CreateVBO(vertNorms);
        mesh.m_posBinder = MakeVertexBinder<glm::vec3>();
        mesh.m_normalBinder = MakeVertexBinder<glm::vec3>();

        if (mesh.m_vao.Create())
        {
            glBindVertexArray(mesh.m_vao);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.m_posVBO);
            mesh.m_posBinder.Bind(0, mesh.m_posVBO);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.m_normalVBO);
            mesh.m_normalBinder.Bind(1, mesh.m_normalVBO);
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
        }

        return mesh;
    }

    EnvironmentManager::PrimitiveMesh EnvironmentManager::CreateCylinderMesh(int segments)
    {
        PrimitiveMesh mesh;

        std::vector<glm::vec3> vertPos;
        std::vector<glm::vec3> vertNorms;

        for (int i = 0; i <= segments; i++)
        {
            float angle = 2.0f * glm::pi<float>() * static_cast<float>(i) / static_cast<float>(segments);
            float x = 0.5f * std::cos(angle);
            float z = 0.5f * std::sin(angle);

            glm::vec3 norm = glm::normalize(glm::vec3(x, 0.0f, z));

            vertPos.push_back(glm::vec3(x, -0.5f, z));
            vertNorms.push_back(norm);
            vertPos.push_back(glm::vec3(x, 0.5f, z));
            vertNorms.push_back(norm);
        }

        std::vector<int> sideIndices;
        for (int i = 0; i < segments; i++)
        {
            int a = i * 2;
            int b = a + 2;
            sideIndices.push_back(a);
            sideIndices.push_back(b);
            sideIndices.push_back(a + 1);
            sideIndices.push_back(a + 1);
            sideIndices.push_back(b);
            sideIndices.push_back(b + 1);
        }

        std::vector<glm::vec3> expandedPos;
        std::vector<glm::vec3> expandedNorms;
        for (size_t i = 0; i < sideIndices.size(); i++)
        {
            expandedPos.push_back(vertPos[sideIndices[i]]);
            expandedNorms.push_back(vertNorms[sideIndices[i]]);
        }

        glm::vec3 topNormal(0.0f, 1.0f, 0.0f);
        glm::vec3 bottomNormal(0.0f, -1.0f, 0.0f);

        for (int i = 0; i <= segments; i++)
        {
            float angle = 2.0f * glm::pi<float>() * static_cast<float>(i) / static_cast<float>(segments);
            float x = 0.5f * std::cos(angle);
            float z = 0.5f * std::sin(angle);

            expandedPos.push_back(glm::vec3(x, 0.5f, z));
            expandedNorms.push_back(topNormal);
        }

        for (int i = 0; i < segments; i++)
        {
            expandedPos.push_back(glm::vec3(0.0f, 0.5f, 0.0f));
            expandedNorms.push_back(topNormal);
            expandedPos.push_back(expandedPos[expandedPos.size() - segments - 2 + i]);
            expandedNorms.push_back(topNormal);
            expandedPos.push_back(expandedPos[expandedPos.size() - segments - 2 + i + 1]);
            expandedNorms.push_back(topNormal);
        }

        int topStart = static_cast<int>(expandedPos.size());
        for (int i = 0; i <= segments; i++)
        {
            float angle = 2.0f * glm::pi<float>() * static_cast<float>(i) / static_cast<float>(segments);
            float x = 0.5f * std::cos(angle);
            float z = 0.5f * std::sin(angle);

            expandedPos.push_back(glm::vec3(x, -0.5f, z));
            expandedNorms.push_back(bottomNormal);
        }

        for (int i = 0; i < segments; i++)
        {
            expandedPos.push_back(glm::vec3(0.0f, -0.5f, 0.0f));
            expandedNorms.push_back(bottomNormal);
            expandedPos.push_back(expandedPos[topStart + i + 1]);
            expandedNorms.push_back(bottomNormal);
            expandedPos.push_back(expandedPos[topStart + i]);
            expandedNorms.push_back(bottomNormal);
        }

        mesh.m_indexCount = static_cast<GLsizei>(expandedPos.size());
        mesh.m_posVBO = CreateVBO(expandedPos);
        mesh.m_normalVBO = CreateVBO(expandedNorms);
        mesh.m_posBinder = MakeVertexBinder<glm::vec3>();
        mesh.m_normalBinder = MakeVertexBinder<glm::vec3>();

        if (mesh.m_vao.Create())
        {
            glBindVertexArray(mesh.m_vao);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.m_posVBO);
            mesh.m_posBinder.Bind(0, mesh.m_posVBO);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.m_normalVBO);
            mesh.m_normalBinder.Bind(1, mesh.m_normalVBO);
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
        }

        return mesh;
    }

    void EnvironmentManager::BuildPrimitiveMeshes()
    {
        m_primitiveMeshes.clear();
        m_primitiveMeshes.push_back(CreateCubeMesh());
        m_primitiveMeshes.push_back(CreateSphereMesh(16));
        m_primitiveMeshes.push_back(CreateConeMesh(16));
        m_primitiveMeshes.push_back(CreateCylinderMesh(16));
    }

    void EnvironmentManager::BuildGroundMesh()
    {
        float size = 50.0f;

        std::vector<glm::vec3> positions = {
            glm::vec3(-size, 0.0f, -size),
            glm::vec3( size, 0.0f, -size),
            glm::vec3( size, 0.0f,  size),
            glm::vec3(-size, 0.0f,  size),
        };

        std::vector<glm::vec3> normals = {
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
        };

        std::vector<uint16_t> indices = { 0, 1, 2, 0, 2, 3 };

        std::vector<glm::vec3> expandedPos;
        std::vector<glm::vec3> expandedNorms;
        for (size_t i = 0; i < indices.size(); i++)
        {
            expandedPos.push_back(positions[indices[i]]);
            expandedNorms.push_back(normals[indices[i]]);
        }

        m_groundMesh.m_indexCount = static_cast<GLsizei>(indices.size());
        m_groundMesh.m_posVBO = CreateVBO(expandedPos);
        m_groundMesh.m_normalVBO = CreateVBO(expandedNorms);
        m_groundMesh.m_posBinder = MakeVertexBinder<glm::vec3>();
        m_groundMesh.m_normalBinder = MakeVertexBinder<glm::vec3>();

        if (m_groundMesh.m_vao.Create())
        {
            glBindVertexArray(m_groundMesh.m_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_groundMesh.m_posVBO);
            m_groundMesh.m_posBinder.Bind(0, m_groundMesh.m_posVBO);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, m_groundMesh.m_normalVBO);
            m_groundMesh.m_normalBinder.Bind(1, m_groundMesh.m_normalVBO);
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
        }
    }

    void EnvironmentManager::BuildBillboardMesh()
    {
        std::vector<glm::vec3> positions = {
            glm::vec3(-0.5f, -0.5f, 0.0f),
            glm::vec3( 0.5f, -0.5f, 0.0f),
            glm::vec3( 0.5f,  0.5f, 0.0f),
            glm::vec3(-0.5f,  0.5f, 0.0f),
        };

        std::vector<glm::vec2> uvs = {
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(0.0f, 1.0f),
        };

        std::vector<uint16_t> indices = { 0, 1, 2, 0, 2, 3 };

        std::vector<glm::vec3> expandedPos;
        std::vector<glm::vec2> expandedUVs;
        for (size_t i = 0; i < indices.size(); i++)
        {
            expandedPos.push_back(positions[indices[i]]);
            expandedUVs.push_back(uvs[indices[i]]);
        }

        m_billboardMesh.m_indexCount = static_cast<GLsizei>(indices.size());
        m_billboardMesh.m_posVBO = CreateVBO(expandedPos);
        m_billboardMesh.m_uvVBO = CreateVBO(expandedUVs);
        m_billboardMesh.m_posBinder = MakeVertexBinder<glm::vec3>();
        m_billboardMesh.m_uvBinder = MakeVertexBinder<glm::vec2>();

        if (m_billboardMesh.m_vao.Create())
        {
            glBindVertexArray(m_billboardMesh.m_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_billboardMesh.m_posVBO);
            m_billboardMesh.m_posBinder.Bind(0, m_billboardMesh.m_posVBO);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, m_billboardMesh.m_uvVBO);
            m_billboardMesh.m_uvBinder.Bind(1, m_billboardMesh.m_uvVBO);
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
        }
    }

    void EnvironmentManager::DrawPrimitive(const Primitive& prim, const PrimitiveMesh& mesh,
        const glm::mat4& view, const glm::mat4& proj)
    {
        glm::mat4 world = glm::mat4(1.0f);
        world = glm::translate(world, prim.m_position);
        world = glm::rotate(world, prim.m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        world = glm::rotate(world, prim.m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        world = glm::rotate(world, prim.m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        world = glm::scale(world, prim.m_size);

        glm::mat4 wvp = proj * view * world;

        glUseProgram(m_primitiveProg);
        glBindVertexArray(mesh.m_vao);

        SetUniform(m_primitive_uWVP, wvp);
        SetUniform(m_primitive_uWorld, world);
        SetUniform(m_primitive_uColor, glm::vec3(prim.m_color));
        SetUniform(m_primitive_uLightDir, m_sun.m_direction);
        SetUniform(m_primitive_uLightColor, m_sun.m_color);
        SetUniform(m_primitive_uLightIntensity, m_sun.m_intensity);

        glDrawArrays(GL_TRIANGLES, 0, mesh.m_indexCount);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void EnvironmentManager::DrawGround(const glm::mat4& view, const glm::mat4& proj)
    {
        glm::mat4 world = glm::mat4(1.0f);
        glm::mat4 wvp = proj * view * world;

        glUseProgram(m_groundProg);
        glBindVertexArray(m_groundMesh.m_vao);

        SetUniform(m_ground_uWVP, wvp);
        SetUniform(m_ground_uWorld, world);
        SetUniform(m_ground_uColor, glm::vec3(m_ground.m_color));
        SetUniform(m_ground_uLightDir, m_sun.m_direction);
        SetUniform(m_ground_uLightColor, m_sun.m_color);
        SetUniform(m_ground_uLightIntensity, m_sun.m_intensity);

        glDrawArrays(GL_TRIANGLES, 0, m_groundMesh.m_indexCount);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void EnvironmentManager::DrawSun(const glm::mat4& view, const glm::mat4& proj)
    {
        glm::vec3 sunPos = m_sun.m_direction * 30.0f;

        glm::mat4 world = glm::mat4(1.0f);
        world = glm::translate(world, sunPos);

        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::vec3 forward = glm::normalize(-sunPos);
        if (std::abs(glm::dot(forward, up)) > 0.99f)
        {
            up = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        glm::vec3 right = glm::normalize(glm::cross(up, forward));
        up = glm::normalize(glm::cross(forward, right));

        glm::mat4 billboard(
            glm::vec4(right, 0.0f),
            glm::vec4(up, 0.0f),
            glm::vec4(forward, 0.0f),
            glm::vec4(sunPos, 1.0f)
        );
        world = glm::scale(billboard, glm::vec3(2.0f));

        glm::mat4 wvp = proj * view * world;

        glUseProgram(m_sunProg);
        glBindVertexArray(m_billboardMesh.m_vao);

        SetUniform(m_sun_uWVP, wvp);
        SetUniform(m_sun_uColor, m_sun.m_color);
        SetUniform(m_sun_uIntensity, m_sun.m_intensity);

        glDrawArrays(GL_TRIANGLES, 0, m_billboardMesh.m_indexCount);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void EnvironmentManager::DrawBillboards(const glm::mat4& view, const glm::mat4& proj)
    {
        for (const auto& obj : m_envObjects)
        {
            glm::mat4 world = glm::mat4(1.0f);
            world = glm::translate(world, obj.m_position);

            glm::vec3 up(0.0f, 1.0f, 0.0f);
            glm::vec3 forward = glm::vec3(
                view[0][2],
                view[1][2],
                view[2][2]
            );
            glm::vec3 right = glm::normalize(glm::cross(up, forward));
            up = glm::normalize(glm::cross(forward, right));

            glm::mat4 billboardMat(
                glm::vec4(right, 0.0f),
                glm::vec4(up, 0.0f),
                glm::vec4(forward, 0.0f),
                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
            );
            world = world * glm::scale(billboardMat, obj.m_size);

            glm::mat4 wvp = proj * view * world;

            glUseProgram(m_billboardProg);
            glBindVertexArray(m_billboardMesh.m_vao);

            SetUniform(m_billboard_uWVP, wvp);
            SetUniform(m_billboard_uColor, obj.m_color);

            glDrawArrays(GL_TRIANGLES, 0, m_billboardMesh.m_indexCount);

            glBindVertexArray(0);
            glUseProgram(0);
        }
    }

    void EnvironmentManager::Draw(const ViewerContext& ctxt)
    {
        if (!m_initialized)
        {
            return;
        }

        const auto& view = ctxt.GetCamera()->GetViewMatrix();
        const auto& proj = ctxt.GetCamera()->GetProjectionMatrix();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        DrawGround(view, proj);

        for (size_t i = 0; i < m_primitives.size(); i++)
        {
            int meshIdx = static_cast<int>(m_primitives[i].m_type);
            if (meshIdx >= 0 && meshIdx < static_cast<int>(m_primitiveMeshes.size()))
            {
                DrawPrimitive(m_primitives[i], m_primitiveMeshes[meshIdx], view, proj);
            }
        }

        DrawBillboards(view, proj);

        glDisable(GL_DEPTH_TEST);
        DrawSun(view, proj);
        glEnable(GL_DEPTH_TEST);

        glDisable(GL_CULL_FACE);
    }
}