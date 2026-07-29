#include "model.hpp"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

namespace pt
{
    Model::Model(const std::filesystem::path& path, bool loadmaterials) : m_directory(path.parent_path().string() + '/')
    {
        Assimp::Importer importer;
        const aiScene* model = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_CalcTangentSpace);
        if (!model || model->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !model->mRootNode)
        {
            std::cerr << "Error: ASSIMP could not import the model\nERROR CODE: " << importer.GetErrorString() << "\n";
            return;
        }

        // Load Mesh
        mesh.loadASSIMPModel(model);

        // Load Material(s)
        if (loadmaterials)
        {
            for (const auto& matPair : mesh.getMaterialPairs())
            {
                aiMaterial* aimaterial = model->mMaterials[matPair.index];
                MaterialManager::loadMaterial(m_directory, aimaterial, matPair.id);
            }
        }
    }

    void Model::draw()
    {
        setMeshTransform();
        mesh.draw();
    }
}