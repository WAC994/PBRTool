#pragma once
#include "transform.hpp"
#include <filesystem>
#include <string>
#include "mesh.hpp"
#include <vector>
#include "material.hpp"

namespace pt
{
  class Model : public Transform
  {
  public:
    Model(const std::filesystem::path& path);
    void draw();
  public:
    Mesh mesh;
    std::string name;
  private:
    std::string m_directory;
    static uint32_t s_materialIDOffset;
  };
}