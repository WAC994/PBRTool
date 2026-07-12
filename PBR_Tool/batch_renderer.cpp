#include "batch_renderer.hpp"
#include <cstdint>
#include <iostream>
#include "pbr_tool_core.hpp"
#include <format>

namespace pt

{
    struct Batch
    {
        std::vector<ModelMatricesData> modelMatricesArray;
        std::vector<DEIC> drawCommandArray;

        static constexpr uint32_t meshCapacity = 1000;
        static constexpr size_t drawCommandSize = meshCapacity * sizeof(DEIC);
        static constexpr size_t modelMatricesSize = meshCapacity * sizeof(ModelMatricesData);

        size_t vertexCapacity = 0;
        size_t indexCapacity = 0;
        size_t vertexOffset = 0;
        size_t indexOffset = 0;

        GLuint vertexArray;
        GLuint vertexBuffer;
        GLuint indexBuffer;
        GLuint drawCommandBuffer;
        GLuint modelMatricesBuffer;

        inline size_t vertexCapacitySize() { return vertexCapacity * sizeof(Vertex); }

        inline size_t indexCapacitySize() { return indexCapacity * sizeof(uint32_t); }

        inline size_t vertexOffsetSize() { return vertexOffset * sizeof(Vertex); }

        inline size_t indexOffsetSize() { return indexOffset * sizeof(uint32_t); }

        GLint getVertexBufferSize()
        {
            GLint vertexBufferSize = 0;
            glGetNamedBufferParameteriv(vertexBuffer, GL_BUFFER_SIZE, &vertexBufferSize);
            return vertexBufferSize;
        }
        size_t getVertexBufferCount() { return getVertexBufferSize() / sizeof(Vertex); }

        GLint getIndexBufferSize()
        {
            GLint indexBufferSize = 0;
            glGetNamedBufferParameteriv(indexBuffer, GL_BUFFER_SIZE, &indexBufferSize);
            return indexBufferSize;
        }
        size_t getIndexBufferCount() { return getIndexBufferSize() / sizeof(uint32_t); }

        void init()
        {
            GLuint buffers[4];
            glCreateBuffers(4, buffers);
            vertexBuffer = buffers[0];
            indexBuffer = buffers[1];
            drawCommandBuffer = buffers[2];
            modelMatricesBuffer = buffers[3];

            // Vertex buffer
            glNamedBufferData(vertexBuffer, vertexCapacitySize(), nullptr, GL_DYNAMIC_DRAW);
            // Index buffer
            glNamedBufferData(indexBuffer, indexCapacitySize(), nullptr, GL_DYNAMIC_DRAW);

            GLbitfield storageFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
            // Draw Command buffer
            glNamedBufferStorage(drawCommandBuffer, drawCommandSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
            // Model Matrices buffer
            glNamedBufferStorage(modelMatricesBuffer, modelMatricesSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

            // Vertex and Index buffer setup
            glCreateVertexArrays(1, &vertexArray);
            glVertexArrayVertexBuffer(vertexArray, 0, vertexBuffer, 0, sizeof(Vertex));
            glVertexArrayElementBuffer(vertexArray, indexBuffer);

            constexpr GLuint positionLoc = 0, normalLoc = 1, tangentLoc = 2, texCoordLoc = 3, materialLoc = 4;

            glEnableVertexArrayAttrib(vertexArray, positionLoc);
            glVertexArrayAttribFormat(vertexArray, positionLoc, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
            glVertexArrayAttribBinding(vertexArray, positionLoc, 0);

            glEnableVertexArrayAttrib(vertexArray, normalLoc);
            glVertexArrayAttribFormat(vertexArray, normalLoc, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
            glVertexArrayAttribBinding(vertexArray, normalLoc, 0);

            glEnableVertexArrayAttrib(vertexArray, tangentLoc);
            glVertexArrayAttribFormat(vertexArray, tangentLoc, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
            glVertexArrayAttribBinding(vertexArray, tangentLoc, 0);

            glEnableVertexArrayAttrib(vertexArray, texCoordLoc);
            glVertexArrayAttribFormat(vertexArray, texCoordLoc, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord));
            glVertexArrayAttribBinding(vertexArray, texCoordLoc, 0);

            glEnableVertexArrayAttrib(vertexArray, materialLoc);
            glVertexArrayAttribFormat(vertexArray, materialLoc, 1, GL_FLOAT, GL_FALSE, offsetof(Vertex, material));
            glVertexArrayAttribBinding(vertexArray, materialLoc, 0);

            // Draw Command array setup
            drawCommandArray.reserve(meshCapacity);

            // Model Matrices array setup
            modelMatricesArray.reserve(meshCapacity);

            // Draw Command buffer setup
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

            // Model Matrices buffer setup
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TRANSFORM_SSBO_BINDING_POINT, modelMatricesBuffer);
        }

        void flush()
        {
            vertexOffset = 0;
            indexOffset = 0;
            drawCommandArray.clear();
            modelMatricesArray.clear();
        }

        void getData(Model& model)
        {
            if ((model.mesh.vertexCount + vertexOffset > getVertexBufferCount()) || (model.mesh.indexCount + indexOffset > getIndexBufferCount()))
            {
                std::cout << std::format("Error in vertex: offset({}) + data({}) > capacity({})\n", vertexOffset, model.mesh.vertexCount, getVertexBufferCount());
                std::cout << std::format("Error in index: offset({}) + data({}) > capacity({})\n", indexOffset, model.mesh.indexCount, getIndexBufferCount());
                return;
            }

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

        void reallocateBuffers()
        {
            glNamedBufferData(vertexBuffer, vertexCapacity * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
            glNamedBufferData(indexBuffer, indexCapacity * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
            std::cout << std::format("New buffer sizes: {} vertices, {} indices\n", getVertexBufferCount(), getIndexBufferCount());
        }

        void add(Model& model)
        {
            size_t vertexTotal = vertexOffset + model.mesh.vertexCount;
            size_t indexTotal = indexOffset + model.mesh.indexCount;

            if ((vertexTotal > vertexCapacity) || (indexTotal > indexCapacity))
            {
                draw();

                vertexCapacity = vertexTotal;
                indexCapacity = indexTotal;
                reallocateBuffers();
            }

            getData(model);

            vertexOffset += model.mesh.vertexCount;
            indexOffset += model.mesh.indexCount;
        }

        void draw()
        {
            glNamedBufferSubData(drawCommandBuffer, 0, sizeof(DEIC)*drawCommandArray.size(), drawCommandArray.data());
            glNamedBufferSubData(modelMatricesBuffer, 0, sizeof(ModelMatricesData)*modelMatricesArray.size(), modelMatricesArray.data());
            bindVertexArray(vertexArray);
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, drawCommandArray.size(), 0);
            flush();
        }
    };

    static Batch batch;

    void BatchRenderer::init()
    {
        batch.init();
    }

    void BatchRenderer::add(Model& model)
    {
        batch.add(model);
    }

    void BatchRenderer::draw()
    {
        batch.draw();
    }
}