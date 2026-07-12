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
    ~Model();
    void draw();
  public:
    Mesh mesh;
    std::string name;
    int32_t batchID = -1;
    bool isBatched = false;
  private:
    std::string m_directory;
    static uint32_t s_materialIDOffset;
  };
}