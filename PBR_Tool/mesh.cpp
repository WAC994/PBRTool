#include "mesh.hpp"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

namespace pt
{
    Mesh::Mesh(const std::filesystem::path& path)
    {
        Assimp::Importer importer;
        const aiScene* model = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_CalcTangentSpace);
        if (!model || model->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !model->mRootNode)
        {
            std::cerr << "Error: ASSIMP could not import the model\nERROR CODE: " << importer.GetErrorString() << "\n";
            return;
        }

        uint32_t iOffset = 0;
        for (uint32_t i = 0; i < model->mNumMeshes; ++i)
        {
            aiMesh* mesh = model->mMeshes[i];
            for (uint32_t j = 0; j < mesh->mNumVertices; ++j)
            {
                Vertex vertex{};
                vertex.position = { mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z };
                vertex.normal = { mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z };
                vertex.tangent = { mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z };
                vertex.texCoord = { mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y };
                vertex.material = static_cast<float>(mesh->mMaterialIndex);
                vertices.push_back(vertex);
            }
            uint32_t iCount = 0;
            for (uint32_t j = 0; j < mesh->mNumFaces; ++j)
            {
                aiFace face = mesh->mFaces[j];
                for (uint32_t k = 0; k < face.mNumIndices; ++k)
                {
                    indices.push_back(face.mIndices[k] + iOffset);
                    ++iCount;
                }
            }
            iOffset += iCount;
        }

        vertices.shrink_to_fit();
        indices.shrink_to_fit();

        vCount = vertices.size();
        vSize = vCount * sizeof(Vertex);

        iCount = indices.size();
        iSize = iCount * sizeof(uint32_t);

        glCreateVertexArrays(1, &m_vArray);
        GLuint buffers[2];
        glCreateBuffers(2, buffers);
        m_vBuffer = buffers[0];
        m_iBuffer = buffers[1];

        glNamedBufferData(m_vBuffer, vSize, vertices.data(), GL_STATIC_DRAW);
        glNamedBufferData(m_iBuffer, iSize, indices.data(), GL_STATIC_DRAW);

        glVertexArrayVertexBuffer(m_vArray, 0, m_vBuffer, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(m_vArray, m_iBuffer);

        constexpr GLuint positionLoc = 0, normalLoc = 1, tangentLoc = 2, texCoordLoc = 3, materialLoc = 4;

        glEnableVertexArrayAttrib(m_vArray, positionLoc);
        glVertexArrayAttribFormat(m_vArray, positionLoc, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
        glVertexArrayAttribBinding(m_vArray, positionLoc, 0);

        glEnableVertexArrayAttrib(m_vArray, normalLoc);
        glVertexArrayAttribFormat(m_vArray, normalLoc, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
        glVertexArrayAttribBinding(m_vArray, normalLoc, 0);

        glEnableVertexArrayAttrib(m_vArray, tangentLoc);
        glVertexArrayAttribFormat(m_vArray, tangentLoc, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
        glVertexArrayAttribBinding(m_vArray, tangentLoc, 0);

        glEnableVertexArrayAttrib(m_vArray, texCoordLoc);
        glVertexArrayAttribFormat(m_vArray, texCoordLoc, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord));
        glVertexArrayAttribBinding(m_vArray, texCoordLoc, 0);

        glEnableVertexArrayAttrib(m_vArray, materialLoc);
        glVertexArrayAttribFormat(m_vArray, materialLoc, 1, GL_FLOAT, GL_FALSE, offsetof(Vertex, material));
        glVertexArrayAttribBinding(m_vArray, materialLoc, 0);
    }

    Mesh::~Mesh()
    {
        glDeleteVertexArrays(1, &m_vArray);
        glDeleteBuffers(1, &m_vBuffer);
        glDeleteBuffers(1, &m_iBuffer);
    }

    void Mesh::draw()
    {
        bindVertexArray(m_vArray);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, reinterpret_cast<const void*>(0));
    }
}