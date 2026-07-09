#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "utilities.hpp"
#include <filesystem>

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
        Mesh(const std::filesystem::path& path);
        ~Mesh();
        void draw();
    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        size_t vCount = 0, vSize = 0, iCount = 0, iSize = 0; 
    private:
        GLuint m_vArray, m_vBuffer, m_iBuffer;
    };
}