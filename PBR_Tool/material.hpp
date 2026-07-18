#pragma once
#include "utilities.hpp"
#include <array>
#include <string>

// Forward declare aiMaterial
struct aiMaterial;

namespace pt
{
    struct MaterialData
    {
        GLuint64 colorTex;
        GLuint64 ormsTex;
        GLuint64 normalTex;
        GLuint64 emissionTex;
    };
    
    class MaterialManager
    {
    public:
        MaterialManager();
        ~MaterialManager();
        static MaterialData& getMaterial(uint32_t material);
        void update();
    public:
        static std::array<MaterialData, MAX_MATERIALS> s_materials;
    private:
        GLuint m_buffer;
    };
}