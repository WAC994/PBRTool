#pragma once
#include <glm/glm.hpp>
#include <glad/gl.h>
#include <string>
#include <array>
#include <map>

struct aiMaterial;

namespace pt
{
    namespace TEX_FLAG
    {
        constexpr uint32_t NONE = 0x0;
        constexpr uint32_t COLOR = 0x1;
        constexpr uint32_t ORM = 0x2;
        constexpr uint32_t NORM = 0x4;
        constexpr uint32_t EMIT = 0x8;
        constexpr uint32_t REFLC = 0x16;
    }

    struct MaterialData
    {
        GLuint64 colorTex = 0;
        GLuint64 ormTex = 0;
        GLuint64 normalTex = 0;
        GLuint64 emissionTex = 0;
        glm::vec4 color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
        glm::vec4 emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        float occlusion = 1.0f;
        float roughness = 0.5f;
        float metallic = 0.0f;
        float refectance = 0.5f;
        alignas(16) uint32_t texFlags = TEX_FLAG::NONE;
    };

    class MaterialManager
    {
    public:
        static void init();
        static uint32_t genID();
        static void loadMaterial(std::string& directory, aiMaterial* aimaterial, uint32_t materialid);
        static void update();
    private:
        static constexpr uint32_t MATERIAL_SSBO_BINDING_POINT = 2;
        static constexpr uint32_t MAX_MATERIALS = 1024;
        static std::array<MaterialData, MAX_MATERIALS> s_materials;
        static std::array<uint32_t, MAX_MATERIALS> s_materialIDs;
        static uint32_t s_materialBuffer;
        static uint32_t s_materialCount;
    };
}