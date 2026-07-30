#include "material.hpp"
#include <assimp/scene.h>
#include "texture.hpp"

namespace pt
{
    std::array<MaterialData, MaterialManager::MAX_MATERIALS> MaterialManager::s_materials;
    std::array<uint32_t, MaterialManager::MAX_MATERIALS> MaterialManager::s_materialIDs;
    uint32_t MaterialManager::s_materialBuffer;
    uint32_t MaterialManager::s_materialCount = 0;

    void MaterialManager::init()
    {
        glCreateBuffers(1, &s_materialBuffer);
        glNamedBufferStorage(s_materialBuffer, sizeof(MaterialData) * s_materials.size(), s_materials.data(), GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MATERIAL_SSBO_BINDING_POINT, s_materialBuffer);
    }

    uint32_t MaterialManager::genID()
    {
        if (s_materialCount < MAX_MATERIALS)
            return s_materialCount++;
        else
            return MAX_MATERIALS;
    }

    void MaterialManager::loadMaterial(std::string& directory, aiMaterial* aimaterial, uint32_t materialid)
    {
        MaterialData material;

        // Data
        aiColor4D colorData;
        aiColor3D emissionData;
        ai_real roughnessData, metallicData, emissionIntensityData;
        if (aimaterial->Get(AI_MATKEY_BASE_COLOR, colorData) == AI_SUCCESS)
            material.color = { colorData.r, colorData.g, colorData.b, colorData.a };

        if (aimaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissionData) == AI_SUCCESS)
            material.emission = { emissionData.r, emissionData.g, emissionData.b, 1.0f };

        if (aimaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughnessData) == AI_SUCCESS)
            material.roughness = roughnessData;

        if (aimaterial->Get(AI_MATKEY_METALLIC_FACTOR, metallicData) == AI_SUCCESS)
            material.metallic = metallicData;
            
        if (aimaterial->Get(AI_MATKEY_EMISSIVE_INTENSITY, emissionIntensityData) == AI_SUCCESS)
            material.emission.a = emissionIntensityData;

        // Textures
        aiString textureString;
        if (aimaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &textureString) == AI_SUCCESS)
        {
            material.colorTex = genBindless2DTextureFromFile(directory + textureString.C_Str());
            material.texFlags |= TEX_FLAG::COLOR;
        }
        if (aimaterial->GetTexture(aiTextureType_GLTF_METALLIC_ROUGHNESS, 0, &textureString) == AI_SUCCESS)
        {
            material.ormTex = genBindless2DTextureFromFile(directory + textureString.C_Str());
            material.texFlags |= TEX_FLAG::ORM;
        }
        if (aimaterial->GetTexture(aiTextureType_NORMALS, 0, &textureString) == AI_SUCCESS)
        {
            material.normalTex = genBindless2DTextureFromFile(directory + textureString.C_Str());
            material.texFlags |= TEX_FLAG::NORM;
        }
        if (aimaterial->GetTexture(aiTextureType_EMISSIVE, 0, &textureString) == AI_SUCCESS)
        {
            material.emissionTex = genBindless2DTextureFromFile(directory + textureString.C_Str());
            material.texFlags |= TEX_FLAG::EMIT;
        }

        s_materials[materialid] = material;
    }

    void MaterialManager::update()
    {
        glNamedBufferSubData(s_materialBuffer, 0, sizeof(MaterialData) * s_materials.size(), s_materials.data());
    }
}