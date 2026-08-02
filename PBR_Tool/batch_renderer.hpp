#pragma once
#include "model.hpp"
#include "utilities.hpp"

namespace pt
{
    class BatchRender
    {
    public:
        BatchRender();
        void batch(Model& model);
        void draw();
    private:
        bool drawCommandChanged(Model& model);
        void getData(Model& model);
    private:
        std::vector<ModelMatrices> modelMatricesArray;
        std::vector<DEIC> drawCommandArray;
        std::unordered_map <Model*, bool> batchTracker;

        static constexpr uint32_t meshCapacity = 1000;
        static constexpr size_t drawCommandCapacitySize = meshCapacity * sizeof(DEIC);
        static constexpr size_t modelMatricesCapacitySize = meshCapacity * sizeof(ModelMatrices);

        static constexpr size_t vertexCapacity = 1000000;
        static constexpr size_t vertexCapacitySize = vertexCapacity * sizeof(Vertex); // 48 Mega-Bytes

        static constexpr size_t indexCapacity = 6000000;
        static constexpr size_t indexCapacitySize = indexCapacity * sizeof(uint32_t); // 24 Mega-Bytes

        size_t vertexOffset = 0;
        size_t indexOffset = 0;
        uint32_t drawIndex = 0;

        GLuint vertexArray;
        GLuint vertexBuffer;
        GLuint indexBuffer;
        GLuint drawCommandBuffer;
        GLuint modelMatricesBuffer;

        inline size_t vertexOffsetSize() { return vertexOffset * sizeof(Vertex); }
        inline size_t indexOffsetSize() { return indexOffset * sizeof(uint32_t); }
        inline size_t drawCommandSize() { return drawCommandArray.size() * sizeof(DEIC); }
        inline size_t modelMatricesSize() { return modelMatricesArray.size() * sizeof(ModelMatrices); }
    };
}