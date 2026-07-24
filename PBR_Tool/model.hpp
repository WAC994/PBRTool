#pragma once
#include "transform.hpp"
#include <filesystem>
#include "mesh.hpp"
#include "material.hpp"

namespace pt
{
  class Model : public Transform
  {
  public:
    Model(const std::filesystem::path& path, bool loadmaterials = true);
    void draw();
  public:
    Mesh mesh;
    std::vector<uint32_t> materialIDs;
  private:
    std::string m_directory;
    uint32_t m_materialID;
  };
}