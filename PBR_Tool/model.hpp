#pragma once
#include "transform.hpp"
#include <filesystem>
#include <string>
#include "mesh.hpp"

namespace pt
{
  class Model : public Transform
  {
  public:
    Model(const std::filesystem::path& path);
    void draw();
  private:
  public:
    std::string name;
    Mesh mesh;
    bool batched = false;
  private:
    uint32_t m_index;
    static uint32_t s_modelOffset;
  };
}