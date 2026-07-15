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
        static constexpr size_t drawCommandCapacitySize = meshCapacity * sizeof(DEIC);
        static constexpr size_t modelMatricesCapacitySize = meshCapacity * sizeof(ModelMatrices);

        static constexpr size_t vertexCapacity = 1000000;
        static constexpr size_t vertexCapacitySize = vertexCapacity * sizeof(Vertex); // 48 Mega-Bytes

        static constexpr size_t indexCapacity = 6000000;
        static constexpr size_t indexCapacitySize = indexCapacity * sizeof(uint32_t); // 12 Mega-Bytes

        size_t vertexOffset = 0;
        size_t indexOffset = 0;
        uint32_t drawIndex = 0;

        GLuint vertexArray;

        GLuint vertexBuffer;
        GLvoid* vertexBufferPointer;

        GLuint indexBuffer;
        GLvoid* indexBufferPointer;

        GLuint drawCommandBuffer;
        GLvoid* drawCommandBufferPointer;

        GLuint modelMatricesBuffer;
        GLvoid* modelMatricesBufferPointer;

        GLbitfield bufferFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

        inline size_t vertexOffsetSize() { return vertexOffset * sizeof(Vertex); }
        inline size_t indexOffsetSize() { return indexOffset * sizeof(uint32_t); }
        inline size_t drawCommandSize() { return drawCommandArray.size() * sizeof(DEIC); }
        inline size_t modelMatricesSize() { return modelMatricesArray.size() * sizeof(ModelMatrices); }

        void init()
        {
            GLuint buffers[4];
            glCreateBuffers(4, buffers);
            vertexBuffer = buffers[0];
            indexBuffer = buffers[1];
            drawCommandBuffer = buffers[2];
            modelMatricesBuffer = buffers[3];

            if (App::getInstance().config.debug)
            {
                // Vertex buffer
                glNamedBufferStorage(vertexBuffer, vertexCapacitySize, nullptr, GL_DYNAMIC_STORAGE_BIT);

                // Index buffer
                glNamedBufferStorage(indexBuffer, indexCapacitySize, nullptr, GL_DYNAMIC_STORAGE_BIT);

                // Draw Command buffer
                glNamedBufferStorage(drawCommandBuffer, drawCommandCapacitySize, nullptr, GL_DYNAMIC_STORAGE_BIT);

                // Model Matrices buffer
                glNamedBufferStorage(modelMatricesBuffer, modelMatricesCapacitySize, nullptr, GL_DYNAMIC_STORAGE_BIT);
            }
            else
            {
                // Vertex buffer
                glNamedBufferStorage(vertexBuffer, vertexCapacitySize, nullptr, bufferFlags);
                vertexBufferPointer = glMapNamedBufferRange(vertexBuffer, 0, vertexCapacitySize, bufferFlags);

                // Index buffer
                glNamedBufferStorage(indexBuffer, indexCapacitySize, nullptr, bufferFlags);
                indexBufferPointer = glMapNamedBufferRange(indexBuffer, 0, indexCapacitySize, bufferFlags);

                // Draw Command buffer
                glNamedBufferStorage(drawCommandBuffer, drawCommandCapacitySize, nullptr, bufferFlags);
                drawCommandBufferPointer = glMapNamedBufferRange(drawCommandBuffer, 0, drawCommandCapacitySize, bufferFlags);

                // Model Matrices buffer
                glNamedBufferStorage(modelMatricesBuffer, modelMatricesCapacitySize, nullptr, bufferFlags);
                modelMatricesBufferPointer = glMapNamedBufferRange(modelMatricesBuffer, 0, modelMatricesCapacitySize, bufferFlags);
            }

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
            if (App::getInstance().config.debug)
            {
                glNamedBufferSubData(vertexBuffer, vertexOffsetSize(), model.mesh.vertexSize, model.mesh.vertices.data());
                glNamedBufferSubData(indexBuffer, indexOffsetSize(), model.mesh.indexSize, model.mesh.indices.data());
            }
            else
            {
                std::memcpy((Vertex*)vertexBufferPointer + vertexOffset, model.mesh.vertices.data(), model.mesh.vertexSize);
                std::memcpy((uint32_t*)indexBufferPointer + indexOffset, model.mesh.indices.data(), model.mesh.indexSize);
            }

            model.mesh.drawCommand.firstIndex = indexOffset;
            model.mesh.drawCommand.basevertex = vertexOffset;
            model.mesh.drawCommand.baseInstance = drawIndex;

            drawCommandArray.emplace_back(model.mesh.drawCommand);

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
            drawIndex++;
        }

        void draw()
        {
            if (App::getInstance().config.debug)
            {
                glNamedBufferSubData(drawCommandBuffer, 0, drawCommandSize(), drawCommandArray.data());
                glNamedBufferSubData(modelMatricesBuffer, 0, modelMatricesSize(), modelMatricesArray.data());
            }
            else
            {
                memcpy((DEIC*)drawCommandBufferPointer, drawCommandArray.data(), drawCommandSize());
                memcpy((ModelMatrices*)modelMatricesBufferPointer, modelMatricesArray.data(), modelMatricesSize());
            }

            bindVertexArray(vertexArray);
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, drawCommandArray.size(), 0);

            vertexOffset = 0;
            indexOffset = 0;
            drawIndex = 0;

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