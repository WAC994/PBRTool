#include "material.hpp"
#include <assimp/scene.h>
#include <iostream>
#include "texture.hpp"

namespace pt
{
    std::array<MaterialData, MAX_MATERIALS> MaterialManager::s_materials;

    MaterialManager::MaterialManager()
    {
        glCreateBuffers(1, &m_buffer);
        glNamedBufferStorage(m_buffer, sizeof(MaterialData) * MAX_MATERIALS, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MATERIAL_SSBO_BINDING_POINT, m_buffer);
    }

    MaterialManager::~MaterialManager()
    {
        glDeleteBuffers(1, &m_buffer);
    }

    MaterialData& MaterialManager::getMaterial(uint32_t material)
    {
        if (material < MAX_MATERIALS)
        {
            return s_materials[material];
        }
        return s_materials[0];
    }

    void MaterialManager::update()
    {
        glNamedBufferSubData(m_buffer, 0, sizeof(MaterialData) * MAX_MATERIALS, s_materials.data());
    }
}