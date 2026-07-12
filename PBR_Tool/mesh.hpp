#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "utilities.hpp"
#include <filesystem>

// Forward declare aiScene and aiMesh
struct aiScene;
struct aiMesh;

namespace pt
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec2 texCoord;
        float material;
    };

    class Mesh
    {
    public:
        Mesh() = default;
        Mesh(const std::filesystem::path& path, uint32_t materialid);
        ~Mesh();
        void loadASSIMPModel(const aiScene* model, uint32_t materialid);
        void loadASSIMPMesh(const aiMesh* mesh, uint32_t materialid);
        void draw();
    private:
        void setupVertexArray();
    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        size_t vertexCount = 0, vertexSize = 0, indexCount = 0, indexSize = 0;
    private:
        GLuint m_vArray, m_vBuffer, m_iBuffer;
        bool m_vertexArrayIsSet = false;
    };
}