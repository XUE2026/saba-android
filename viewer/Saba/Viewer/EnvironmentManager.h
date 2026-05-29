//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_ENVIRONMENTMANAGER_H_
#define SABA_VIEWER_ENVIRONMENTMANAGER_H_

#include "ViewerContext.h"

#include "../GL/GLObject.h"
#include "../GL/GLVertexUtil.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <memory>
#include <string>

namespace saba
{
    class EnvironmentManager
    {
    public:
        enum class PrimitiveType
        {
            Cube,
            Sphere,
            Cone,
            Cylinder
        };

        enum class GroundType
        {
            Floor,
            Grass,
            Mud,
            Asphalt,
            Sky,
            Field
        };

        enum class EnvObjectType
        {
            Tree,
            Flower,
            Bird,
            Butterfly
        };

        struct SunLight
        {
            glm::vec3   m_direction;
            float       m_intensity;
            glm::vec3   m_color;
        };

        struct Primitive
        {
            PrimitiveType   m_type;
            glm::vec3       m_size;
            glm::vec3       m_position;
            glm::vec3       m_rotation;
            glm::vec4       m_color;
        };

        struct Ground
        {
            GroundType  m_type;
            glm::vec4   m_color;
        };

        struct BillboardObject
        {
            EnvObjectType   m_type;
            glm::vec3       m_position;
            glm::vec3       m_size;
            glm::vec4       m_color;
        };

        EnvironmentManager();

        bool Initialize(const ViewerContext& ctxt);
        void Uninitialize();

        void SetSunDirection(float x, float y, float z);
        void SetSunIntensity(float intensity);
        void SetSunColor(float r, float g, float b);

        void AddPrimitive(PrimitiveType type, const glm::vec3& size,
            const glm::vec3& pos, const glm::vec4& color);

        void SetGroundType(GroundType groundType);

        void AddEnvironmentObject(EnvObjectType objType, const glm::vec3& pos,
            const glm::vec3& size = glm::vec3(1.0f),
            const glm::vec4& color = glm::vec4(1.0f));

        void RemovePrimitive(size_t index);
        void RemoveEnvironmentObject(size_t index);
        void ClearPrimitives();
        void ClearEnvironmentObjects();

        const SunLight& GetSunLight() const { return m_sun; }
        const Ground& GetGround() const { return m_ground; }
        const std::vector<Primitive>& GetPrimitives() const { return m_primitives; }
        const std::vector<BillboardObject>& GetEnvObjects() const { return m_envObjects; }

        Primitive& GetPrimitive(size_t index) { return m_primitives[index]; }
        BillboardObject& GetEnvObject(size_t index) { return m_envObjects[index]; }

        void Draw(const ViewerContext& ctxt);

    private:
        struct PrimitiveMesh
        {
            GLVertexArrayObject m_vao;
            GLBufferObject      m_posVBO;
            GLBufferObject      m_normalVBO;
            GLBufferObject      m_ibm;
            GLsizei             m_indexCount;
            VertexBinder        m_posBinder;
            VertexBinder        m_normalBinder;
        };

        struct BillboardMesh
        {
            GLVertexArrayObject m_vao;
            GLBufferObject      m_posVBO;
            GLBufferObject      m_uvVBO;
            GLBufferObject      m_ibm;
            GLsizei             m_indexCount;
            VertexBinder        m_posBinder;
            VertexBinder        m_uvBinder;
        };

        struct GroundMesh
        {
            GLVertexArrayObject m_vao;
            GLBufferObject      m_posVBO;
            GLBufferObject      m_normalVBO;
            GLBufferObject      m_ibm;
            GLsizei             m_indexCount;
            VertexBinder        m_posBinder;
            VertexBinder        m_normalBinder;
        };

        void BuildPrimitiveMeshes();
        void BuildGroundMesh();
        void BuildBillboardMesh();

        PrimitiveMesh CreateCubeMesh();
        PrimitiveMesh CreateSphereMesh(int segments);
        PrimitiveMesh CreateConeMesh(int segments);
        PrimitiveMesh CreateCylinderMesh(int segments);

        void DrawPrimitive(const Primitive& prim, const PrimitiveMesh& mesh,
            const glm::mat4& view, const glm::mat4& proj);
        void DrawGround(const glm::mat4& view, const glm::mat4& proj);
        void DrawSun(const glm::mat4& view, const glm::mat4& proj);
        void DrawBillboards(const glm::mat4& view, const glm::mat4& proj);

        glm::vec4 GetGroundColorForType(GroundType type) const;

        SunLight    m_sun;
        Ground      m_ground;
        GroundMesh  m_groundMesh;

        std::vector<Primitive>      m_primitives;
        std::vector<PrimitiveMesh>  m_primitiveMeshes;

        std::vector<BillboardObject>    m_envObjects;
        BillboardMesh                   m_billboardMesh;

        GLProgramObject m_primitiveProg;
        GLProgramObject m_groundProg;
        GLProgramObject m_billboardProg;
        GLProgramObject m_sunProg;

        GLint m_primitive_uWVP;
        GLint m_primitive_uWorld;
        GLint m_primitive_uColor;
        GLint m_primitive_uLightDir;
        GLint m_primitive_uLightColor;
        GLint m_primitive_uLightIntensity;
        GLint m_primitive_uViewPos;

        GLint m_ground_uWVP;
        GLint m_ground_uWorld;
        GLint m_ground_uColor;
        GLint m_ground_uLightDir;
        GLint m_ground_uLightColor;
        GLint m_ground_uLightIntensity;

        GLint m_billboard_uWVP;
        GLint m_billboard_uColor;

        GLint m_sun_uWVP;
        GLint m_sun_uColor;
        GLint m_sun_uIntensity;

        bool m_initialized;
    };
}

#endif // !SABA_VIEWER_ENVIRONMENTMANAGER_H_