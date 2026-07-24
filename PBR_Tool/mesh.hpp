#pragma once
#include <filesystem>
#include <vector>
#include <utility>
#include <glm/glm.hpp>
#include "utilities.hpp"

// Forward declare aiScene and aiMesh
struct aiScene;
struct aiMesh;

namespace pt
{
    struct Vertex
    {
        Vertex() = default;
        Vertex(const glm::vec3& pos, const glm::vec3& nor, const glm::vec3& tan, const glm::vec2& tex, float mat)
            : position(pos), normal(nor), tangent(tan), texCoord(tex), material(mat)
        {
        }
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec2 texCoord;
        float material;
    };

    struct MaterialPair
    {
        uint32_t index;
        uint32_t id;
    };

    class Mesh
    {
    public:
        Mesh() = default;
        Mesh(const std::filesystem::path& path);
        ~Mesh();
        void loadASSIMPModel(const aiScene* model);
        void draw();
        const std::vector<MaterialPair>& getMaterialPairs();
    private:
        void setupMeshData();
        void setupVertexArray();
    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        DEIC drawCommand;
        bool batched = false;
        size_t vertexCount = 0, vertexSize = 0, indexCount = 0, indexSize = 0;
    private:
        GLuint m_vArray, m_vBuffer, m_iBuffer;
        bool m_vertexArrayIsSet = false;
        // Material Indices from importer
        std::vector<MaterialPair> m_materialIDIndexPairs;
    };
}