#include "mesh.hpp"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

namespace pt
{
    Mesh::Mesh(const std::filesystem::path& path, uint32_t materialid)
    {
        Assimp::Importer importer;
        const aiScene* model = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_CalcTangentSpace);
        if (!model || model->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !model->mRootNode)
        {
            std::cerr << "Error: ASSIMP could not import the model\nERROR CODE: " << importer.GetErrorString() << "\n";
            return;
        }

        loadASSIMPModel(model, materialid);
    }

    Mesh::~Mesh()
    {
        if (m_vertexArrayIsSet)
        {
            glDeleteVertexArrays(1, &m_vArray);
            glDeleteBuffers(1, &m_vBuffer);
            glDeleteBuffers(1, &m_iBuffer);
        }
    }

    void Mesh::loadASSIMPModel(const aiScene* model, uint32_t materialid)
    {
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
                vertex.material = static_cast<float>(mesh->mMaterialIndex + materialid);
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

        setupMeshData();

        setupVertexArray();
    }

    void Mesh::loadASSIMPMesh(const aiMesh* mesh, uint32_t materialid)
    {
        for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex vertex{};
            vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
            vertex.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
            vertex.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            vertex.material = static_cast<float>(mesh->mMaterialIndex + materialid);
            vertices.push_back(vertex);
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; ++j)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        setupMeshData();

        setupVertexArray();
    }

    void Mesh::draw()
    {
        if (m_vertexArrayIsSet)
        {
            bindVertexArray(m_vArray);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, reinterpret_cast<const void*>(0));
        }
    }

    void Mesh::setupMeshData()
    {
        vertices.shrink_to_fit();
        indices.shrink_to_fit();

        vertexCount = vertices.size();
        vertexSize = vertexCount * sizeof(Vertex);

        indexCount = indices.size();
        indexSize = indexCount * sizeof(uint32_t);

        drawCommand.count = indexCount;
    }

void Mesh::setupVertexArray()
    {
        if (!m_vertexArrayIsSet)
        {
            glCreateVertexArrays(1, &m_vArray);
            GLuint buffers[2];
            glCreateBuffers(2, buffers);
            m_vBuffer = buffers[0];
            m_iBuffer = buffers[1];

            glNamedBufferData(m_vBuffer, vertexSize, vertices.data(), GL_STATIC_DRAW);
            glNamedBufferData(m_iBuffer, indexSize, indices.data(), GL_STATIC_DRAW);

            glVertexArrayVertexBuffer(m_vArray, 0, m_vBuffer, 0, sizeof(Vertex));
            glVertexArrayElementBuffer(m_vArray, m_iBuffer);

            glEnableVertexArrayAttrib(m_vArray, POSITION_LOCATION);
            glVertexArrayAttribFormat(m_vArray, POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
            glVertexArrayAttribBinding(m_vArray, POSITION_LOCATION, 0);

            glEnableVertexArrayAttrib(m_vArray, NORMAL_LOCATION);
            glVertexArrayAttribFormat(m_vArray, NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
            glVertexArrayAttribBinding(m_vArray, NORMAL_LOCATION, 0);

            glEnableVertexArrayAttrib(m_vArray, TANGENT_LOCATION);
            glVertexArrayAttribFormat(m_vArray, TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
            glVertexArrayAttribBinding(m_vArray, TANGENT_LOCATION, 0);

            glEnableVertexArrayAttrib(m_vArray, TEXTURE_COORDINATE_LOCATION);
            glVertexArrayAttribFormat(m_vArray, TEXTURE_COORDINATE_LOCATION, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord));
            glVertexArrayAttribBinding(m_vArray, TEXTURE_COORDINATE_LOCATION, 0);

            glEnableVertexArrayAttrib(m_vArray, MATERIAL_LOCATION);
            glVertexArrayAttribFormat(m_vArray, MATERIAL_LOCATION, 1, GL_FLOAT, GL_FALSE, offsetof(Vertex, material));
            glVertexArrayAttribBinding(m_vArray, MATERIAL_LOCATION, 0);

            m_vertexArrayIsSet = true;
        }
    }
}