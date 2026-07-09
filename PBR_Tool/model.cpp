#include "model.hpp"

namespace pt
{
    uint32_t Model::s_modelOffset = 0;
    Model::Model(const std::filesystem::path& path) : mesh(path), name(path.stem().string()), m_index(s_modelOffset++)
    {
        
    }
    void Model::draw()
    {
        mesh.draw();
    }
}