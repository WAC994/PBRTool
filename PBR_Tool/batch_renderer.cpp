#include "batch_renderer.hpp"
#include <cstdint>
#include <iostream>
#include "pbr_tool_core.hpp"
#include <format>
#include <unordered_map>

namespace pt
{
    struct Batch
    {
        std::vector<ModelMatrices> modelMatricesArray;
        std::vector<DEIC> drawCommandArray;

        static constexpr uint32_t meshCapacity = 1000;
        static constexpr size_t drawCommandSize = meshCapacity * sizeof(DEIC);
        static constexpr size_t modelMatricesSize = meshCapacity * sizeof(ModelMatrices);

        static constexpr size_t vertexCapacity = 1000000;
        static constexpr size_t vertexCapacitySize = vertexCapacity * sizeof(Vertex); // 48 Mega-Bytes

        static constexpr size_t indexCapacity = 6000000;
        static constexpr size_t indexCapacitySize = indexCapacity * sizeof(uint32_t); // 12 Mega-Bytes

        size_t vertexOffset = 0;
        size_t indexOffset = 0;

        GLuint vertexArray;
        GLuint vertexBuffer;
        GLuint indexBuffer;
        GLuint drawCommandBuffer;
        GLuint modelMatricesBuffer;

        inline size_t vertexOffsetSize() { return vertexOffset * sizeof(Vertex); }
        inline size_t indexOffsetSize() { return indexOffset * sizeof(uint32_t); }

        void init()
        {
            GLuint buffers[4];
            glCreateBuffers(4, buffers);
            vertexBuffer = buffers[0];
            indexBuffer = buffers[1];
            drawCommandBuffer = buffers[2];
            modelMatricesBuffer = buffers[3];

            // Vertex buffer
            glNamedBufferStorage(vertexBuffer, vertexCapacitySize, nullptr, GL_DYNAMIC_STORAGE_BIT);

            // Index buffer
            glNamedBufferStorage(indexBuffer, indexCapacitySize, nullptr, GL_DYNAMIC_STORAGE_BIT);

            // Draw Command buffer
            glNamedBufferStorage(drawCommandBuffer, drawCommandSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

            // Model Matrices buffer
            glNamedBufferStorage(modelMatricesBuffer, modelMatricesSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

            // Vertex and Index buffer setup
            glCreateVertexArrays(1, &vertexArray);
            glVertexArrayVertexBuffer(vertexArray, 0, vertexBuffer, 0, sizeof(Vertex));
            glVertexArrayElementBuffer(vertexArray, indexBuffer);

            glEnableVertexArrayAttrib(vertexArray, POSITION_LOCATION);
            glVertexArrayAttribFormat(vertexArray, POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
            glVertexArrayAttribBinding(vertexArray, POSITION_LOCATION, 0);

            glEnableVertexArrayAttrib(vertexArray, NORMAL_LOCATION);
            glVertexArrayAttribFormat(vertexArray, NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
            glVertexArrayAttribBinding(vertexArray, NORMAL_LOCATION, 0);

            glEnableVertexArrayAttrib(vertexArray, TANGENT_LOCATION);
            glVertexArrayAttribFormat(vertexArray, TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
            glVertexArrayAttribBinding(vertexArray, TANGENT_LOCATION, 0);

            glEnableVertexArrayAttrib(vertexArray, TEXTURE_COORDINATE_LOCATION);
            glVertexArrayAttribFormat(vertexArray, TEXTURE_COORDINATE_LOCATION, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord));
            glVertexArrayAttribBinding(vertexArray, TEXTURE_COORDINATE_LOCATION, 0);

            glEnableVertexArrayAttrib(vertexArray, MATERIAL_LOCATION);
            glVertexArrayAttribFormat(vertexArray, MATERIAL_LOCATION, 1, GL_FLOAT, GL_FALSE, offsetof(Vertex, material));
            glVertexArrayAttribBinding(vertexArray, MATERIAL_LOCATION, 0);

            // Draw Command array setup
            drawCommandArray.reserve(meshCapacity);

            // Model Matrices array setup
            modelMatricesArray.reserve(meshCapacity);

            // Draw Command buffer setup
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

            // Model Matrices buffer setup
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TRANSFORM_SSBO_BINDING_POINT, modelMatricesBuffer);
        }

        void getData(Model& model)
        {
            glNamedBufferSubData(vertexBuffer, vertexOffsetSize(), model.mesh.vertexSize, model.mesh.vertices.data());
            glNamedBufferSubData(indexBuffer, indexOffsetSize(), model.mesh.indexSize, model.mesh.indices.data());

            drawCommandArray.emplace_back(
                static_cast<uint32_t>(model.mesh.indexCount),
                1,
                static_cast<uint32_t>(indexOffset),
                static_cast<int32_t>(vertexOffset),
                static_cast<uint32_t>(drawCommandArray.size())
            );

            modelMatricesArray.emplace_back(model.getMartices());
        }

        void batch(Model& model)
        {
            size_t vertexTotal = vertexOffset + model.mesh.vertexCount;
            size_t indexTotal = indexOffset + model.mesh.indexCount;

            if ((vertexTotal > vertexCapacity) || (indexTotal > indexCapacity))
            {
                draw();
                batch(model);
                return;
            }

            getData(model);

            vertexOffset += model.mesh.vertexCount;
            indexOffset += model.mesh.indexCount;
        }

        void draw()
        {
            glNamedBufferSubData(drawCommandBuffer, 0, sizeof(DEIC) * drawCommandArray.size(), drawCommandArray.data());
            glNamedBufferSubData(modelMatricesBuffer, 0, sizeof(ModelMatrices) * modelMatricesArray.size(), modelMatricesArray.data());
            bindVertexArray(vertexArray);
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, drawCommandArray.size(), 0);

            vertexOffset = 0;
            indexOffset = 0;

            drawCommandArray.clear();
            modelMatricesArray.clear();
        }
    };

    static Batch s_instance;

    void BatchRenderer::init()
    {
        s_instance.init();
    }

    void BatchRenderer::batch(Model& model)
    {
        s_instance.batch(model);
    }

    void BatchRenderer::draw()
    {
        s_instance.draw();
    }
}