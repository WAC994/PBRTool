#include "batch_renderer.hpp"
#include <cstdint>
#include <iostream>

namespace pt
{
    struct Batch
    {
        static constexpr uint32_t mCount = 1000;
        static constexpr size_t dcSize = mCount * sizeof(DEIC);
        static constexpr size_t mmSize = mCount * sizeof(ModelMatricesData);
        uint32_t vCount = 0;
        uint32_t iCount = 0;
        uint32_t vOffset = 0;
        uint32_t iOffset = 0;
        uint32_t vtArray;
        uint32_t vBuffer;
        uint32_t iBuffer;
        uint32_t dcBuffer;
        uint32_t mmBuffer;

        bool reset = true;

        std::vector<ModelMatricesData> mmArray;
        std::vector<DEIC> dcArray;
        std::vector<Model*> mPtrArray;

        size_t vSize()
        {
            return vCount * sizeof(Vertex);
        }

        size_t iSize()
        {
            return iCount * sizeof(uint32_t);
        }

        size_t vOffsetSize()
        {
            return vOffset * sizeof(Vertex);
        }

        size_t iOffsetSize()
        {
            return iOffset * sizeof(uint32_t);
        }

        void addData(Model& model)
        {
            // Return if mesh is over buffers capacity (prevents error)
            if (((model.mesh.vSize + vOffsetSize()) > vSize()) || ((model.mesh.iSize + iOffsetSize()) > iSize()))
                return;
            glNamedBufferSubData(vBuffer, vOffsetSize(), model.mesh.vSize, model.mesh.vertices.data());
            glNamedBufferSubData(iBuffer, iOffsetSize(), model.mesh.iSize, model.mesh.indices.data());

            // Add draw data to  draw command array
            dcArray.emplace_back(static_cast<uint32_t>(model.mesh.iCount), 1, iOffset, vOffset, static_cast<uint32_t>(dcArray.size()));
        }

        void flush()
        {
            for (Model*& model : mPtrArray)
            {
                if (model)
                    model->batched = false;
            }
            vOffset = 0;
            iOffset = 0;
            dcArray.clear();
            mPtrArray.clear();
            reset = true;
        }

        void reallocate(uint32_t newvcount, uint32_t newicount)
        {
            vCount = newvcount;
            iCount = newicount;
            glNamedBufferData(vBuffer, newvcount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
            glNamedBufferData(iBuffer, newicount * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
        }
    };

    static Batch batch;

    void BatchRenderer::init()
    {
        uint32_t buffers[4];
        glCreateBuffers(4, buffers);
        batch.vBuffer = buffers[0];
        batch.iBuffer = buffers[1];
        batch.dcBuffer = buffers[2];
        batch.mmBuffer = buffers[3];

        // Vertex buffer
        glNamedBufferData(batch.vBuffer, batch.vSize(), nullptr, GL_DYNAMIC_DRAW);
        // Index buffer
        glNamedBufferData(batch.iBuffer, batch.iSize(), nullptr, GL_DYNAMIC_DRAW);
        // Draw Command buffer
        glNamedBufferStorage(batch.dcBuffer, batch.dcSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        // Model Matrices buffer
        glNamedBufferStorage(batch.mmBuffer, batch.mmSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

        // Vertex and Index buffer setup
        glCreateVertexArrays(1, &batch.vtArray);
        glVertexArrayVertexBuffer(batch.vtArray, 0, batch.vBuffer, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(batch.vtArray, batch.iBuffer);

        constexpr GLuint positionLoc = 0, normalLoc = 1, tangentLoc = 2, texCoordLoc = 3, materialLoc = 4;

        glEnableVertexArrayAttrib(batch.vtArray, positionLoc);
        glVertexArrayAttribFormat(batch.vtArray, positionLoc, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
        glVertexArrayAttribBinding(batch.vtArray, positionLoc, 0);

        glEnableVertexArrayAttrib(batch.vtArray, normalLoc);
        glVertexArrayAttribFormat(batch.vtArray, normalLoc, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
        glVertexArrayAttribBinding(batch.vtArray, normalLoc, 0);

        glEnableVertexArrayAttrib(batch.vtArray, tangentLoc);
        glVertexArrayAttribFormat(batch.vtArray, tangentLoc, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
        glVertexArrayAttribBinding(batch.vtArray, tangentLoc, 0);

        glEnableVertexArrayAttrib(batch.vtArray, texCoordLoc);
        glVertexArrayAttribFormat(batch.vtArray, texCoordLoc, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord));
        glVertexArrayAttribBinding(batch.vtArray, texCoordLoc, 0);

        glEnableVertexArrayAttrib(batch.vtArray, materialLoc);
        glVertexArrayAttribFormat(batch.vtArray, materialLoc, 1, GL_FLOAT, GL_FALSE, offsetof(Vertex, material));
        glVertexArrayAttribBinding(batch.vtArray, materialLoc, 0);

        // Draw Command array setup
        batch.dcArray.reserve(batch.mCount);

        // Model Matrices array setup
        batch.mmArray.reserve(batch.mCount);

        // Draw Command buffer setup
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, batch.dcBuffer);

        // Model Matrices buffer setup
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TRANSFORM_SSBO_BINDING_POINT, batch.mmBuffer);
    }

    void BatchRenderer::add(Model& model)
    {
        if (!model.batched)
        {
            // Find the capacity of the buffers
            uint32_t vTotal = batch.vOffset + model.mesh.vCount;
            uint32_t iTotal = batch.iOffset + model.mesh.iCount;

            // Draw meshes already in the buffers and reallocate for new meshes
            if (vTotal > batch.vCount || iTotal > batch.iCount || batch.mmArray.size() > batch.mCount)
            {
                BatchRenderer::draw();

                batch.flush();

                batch.reallocate(vTotal, iTotal);
            }

            // Batch mesh data to buffers
            batch.addData(model);

            batch.vOffset += model.mesh.vCount;
            batch.iOffset += model.mesh.iCount;

            model.batched = true;
            batch.mPtrArray.push_back(&model);
        }
        batch.mmArray.emplace_back(model.getMartices());
    }

    void BatchRenderer::draw()
    {
        size_t mCount = batch.dcArray.size();
        size_t dcSize = mCount * sizeof(DEIC);
        size_t mmSize = mCount * sizeof(ModelMatricesData);

        // Update draw command buffer only on new batches
        if (batch.reset)
        {
            glNamedBufferSubData(batch.dcBuffer, 0, dcSize, batch.dcArray.data());
            batch.reset = false;
        }

        glNamedBufferSubData(batch.mmBuffer, 0, mmSize, batch.mmArray.data());
        batch.mmArray.clear();

        glBindVertexArray(batch.vtArray);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, mCount, 0);
    }
}