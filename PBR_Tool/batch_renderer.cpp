#include "batch_renderer.hpp"
#include <cstdint>
#include <iostream>
#include "pbr_tool_core.hpp"
#include <format>
#include <unordered_map>

namespace pt
{
    BatchRender::BatchRender()
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
        glNamedBufferStorage(drawCommandBuffer, drawCommandCapacitySize, nullptr, GL_DYNAMIC_STORAGE_BIT);

        // Model Matrices buffer
        glNamedBufferStorage(modelMatricesBuffer, modelMatricesCapacitySize, nullptr, GL_DYNAMIC_STORAGE_BIT);

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

    void BatchRender::batch(Model& model)
    {
        size_t vertexTotal = vertexOffset + model.mesh.vertexCount;
        size_t indexTotal = indexOffset + model.mesh.indexCount;

        if ((vertexTotal > vertexCapacity) || (indexTotal > indexCapacity) || drawIndex >= meshCapacity)
        {
            draw();
            batch(model);
            return;
        }

        getData(model);

        model.mesh.batched = true;
        batchTracker[&model] = model.mesh.batched;

        drawIndex++;
    }

    bool BatchRender::drawCommandChanged(Model& model)
    {
        return (
            model.mesh.drawCommand.firstIndex != indexOffset ||
            model.mesh.drawCommand.basevertex != vertexOffset ||
            model.mesh.drawCommand.baseInstance != drawIndex
            );
    }

    void BatchRender::getData(Model& model)
    {
        if (drawCommandChanged(model) || !model.mesh.batched)
        {
            glNamedBufferSubData(vertexBuffer, vertexOffsetSize(), model.mesh.vertexSize, model.mesh.vertices.data());
            glNamedBufferSubData(indexBuffer, indexOffsetSize(), model.mesh.indexSize, model.mesh.indices.data());
            model.mesh.drawCommand.firstIndex = indexOffset;
            model.mesh.drawCommand.basevertex = vertexOffset;
            model.mesh.drawCommand.baseInstance = drawIndex;
        }

        drawCommandArray.emplace_back(model.mesh.drawCommand);

        modelMatricesArray.emplace_back(model.getMartices());

        vertexOffset += model.mesh.vertexCount;
        indexOffset += model.mesh.indexCount;
    }

    void BatchRender::draw()
    {
        for (auto& model : batchTracker)
        {
            if (model.first)
                model.first->mesh.batched = model.first->mesh.batched && model.second;
        }

        glNamedBufferSubData(drawCommandBuffer, 0, drawCommandSize(), drawCommandArray.data());
        glNamedBufferSubData(modelMatricesBuffer, 0, modelMatricesSize(), modelMatricesArray.data());
        bindVertexArray(vertexArray);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, drawCommandArray.size(), 0);

        vertexOffset = 0;
        indexOffset = 0;
        drawIndex = 0;

        for (auto& model : batchTracker)
        {
            if (model.first)
                model.second = false;
        }

        drawCommandArray.clear();
        modelMatricesArray.clear();
    }
}