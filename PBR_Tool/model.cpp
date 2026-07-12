#include "model.hpp"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include "texture.hpp"

namespace pt
{
    uint32_t Model::s_materialIDOffset = 0;
    Model::Model(const std::filesystem::path& path) : m_directory(path.parent_path().string() + '/')
    {
        name = path.stem().string();
        Assimp::Importer importer;
        const aiScene* model = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_CalcTangentSpace);
        if (!model || model->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !model->mRootNode)
        {
            std::cerr << "Error: ASSIMP could not import the model\nERROR CODE: " << importer.GetErrorString() << "\n";
            return;
        }

        // Load mesh
        mesh.loadASSIMPModel(model, s_materialIDOffset);

        uint32_t materialCount = model->mNumMaterials;

        for (uint32_t i = 0; i < materialCount; i++)
        {
            aiMaterial* aimaterial = model->mMaterials[i];
            auto& material = MaterialManager::getMaterial(i + s_materialIDOffset);
            aiString texturFileName;

            // Base color
            if (aimaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &texturFileName) == AI_SUCCESS)
                material.colorTex = genBindless2DTextureFromFile(m_directory + texturFileName.C_Str());

            // ORMS
            if (aimaterial->GetTexture(aiTextureType_GLTF_METALLIC_ROUGHNESS, 0, &texturFileName) == AI_SUCCESS)
                material.ormsTex = genBindless2DTextureFromFile(m_directory + texturFileName.C_Str());

            // Normal
            if (aimaterial->GetTexture(aiTextureType_NORMALS, 0, &texturFileName) == AI_SUCCESS)
                material.normalTex = genBindless2DTextureFromFile(m_directory + texturFileName.C_Str());

            // Emission
            if (aimaterial->GetTexture(aiTextureType_EMISSIVE, 0, &texturFileName) == AI_SUCCESS)
                material.emissionTex = genBindless2DTextureFromFile(m_directory + texturFileName.C_Str());
        }
        
        s_materialIDOffset += materialCount;
    }

    Model::~Model()
    {
        batchID = -1;
        isBatched = false;
    }

    void Model::draw()
    {
        mesh.draw();
    }
}