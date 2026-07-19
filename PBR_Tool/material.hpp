#pragma once
#include "utilities.hpp"
#include <array>
#include <string>
#include <type_traits>
#include <glm/glm.hpp>

// Forward declare aiMaterial
struct aiMaterial;

namespace pt
{
    namespace TEX_FLAG
    {
        constexpr uint32_t NONE  = 0x0;
        constexpr uint32_t COLOR = 0x1;
        constexpr uint32_t ORMS  = 0x2;
        constexpr uint32_t NORM  = 0x4;
        constexpr uint32_t EMIT  = 0x8;
    }

    struct MaterialData
    {
        GLuint64 colorTex = 0;
        GLuint64 ormsTex = 0;
        GLuint64 normalTex = 0;
        GLuint64 emissionTex = 0;
        glm::vec4 color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
        glm::vec4 emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        float occlusion = 1.0f;
        float roughness = 0.5f;
        float metallic = 0.0f;
        float specular = 0.5f;
        alignas(16) uint32_t texFlags = TEX_FLAG::NONE;
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