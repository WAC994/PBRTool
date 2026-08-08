#pragma once
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glad/gl.h>

using FilePath = std::filesystem::path;
namespace pt
{
  enum class ShaderType
  {
    VERT,
    FRAG,
    TESSEVAL,
    TESSCONT,
    GEOM
  };

  enum class SHADERFILEINFO
  {
    STRING, // String literal
    TEXTFILE, // Plane text
  };

  class Shader
  {
  public:
    Shader();
    Shader(const std::string_view& vert, const std::string_view& frag, SHADERFILEINFO info);

    void addShader(ShaderType shaderType, const std::string_view& shader, SHADERFILEINFO info);

    void linkProgram() const;

    ~Shader();
    void use() const;

    void setInt(int32_t loc, int32_t v0) const;
    void setBool(int32_t loc, bool value) const;
    void setUint(int32_t loc, uint32_t v0) const;
    void setFloat(int32_t loc, float v0) const;
    void setFloat2(int32_t loc, float v0, float v1) const;
    void setFloat3(int32_t loc, float v0, float v1, float v2) const;
    void setFloat4(int32_t loc, float v0, float v1, float v2, float v3) const;
    void setVec2(int32_t loc, const glm::vec2& value, int32_t count = 1) const;
    void setVec3(int32_t loc, const glm::vec3& value, int32_t count = 1) const;
    void setVec4(int32_t loc, const glm::vec4& value, int32_t count = 1) const;
    void setMat2(int32_t loc, const glm::mat2& value, int32_t count = 1) const;
    void setMat3(int32_t loc, const glm::mat3& value, int32_t count = 1) const;
    void setMat4(int32_t loc, const glm::mat4& value, int32_t count = 1) const;


    void setNamedInt(const std::string_view& name, int32_t v0);
    void setNamedBool(const std::string_view& name, bool value);
    void setNamedUint(const std::string_view& name, uint32_t v0);
    void setNamedFloat(const std::string_view& name, float v0);
    void setNamedFloat2(const std::string_view& name, float v0, float v1);
    void setNamedFloat3(const std::string_view& name, float v0, float v1, float v2);
    void setNamedFloat4(const std::string_view& name, float v0, float v1, float v2, float v3);
    void setNamedVec2(const std::string_view& name, const glm::vec2& value, int32_t count = 1);
    void setNamedVec3(const std::string_view& name, const glm::vec3& value, int32_t count = 1);
    void setNamedVec4(const std::string_view& name, const glm::vec4& value, int32_t count = 1);
    void setNamedMat2(const std::string_view& name, const glm::mat2& value, int32_t count = 1);
    void setNamedMat3(const std::string_view& name, const glm::mat3& value, int32_t count = 1);
    void setNamedMat4(const std::string_view& name, const glm::mat4& value, int32_t count = 1);

    void bindTexture(uint32_t unit, uint32_t texture) const;
    void bindTextures(uint32_t first, int32_t count, const uint32_t* textures) const;
    void useBindlessTexture(const std::string_view& name, uint64_t texture);

  private:
    void compileshader(ShaderType shaderType, const char* code);
    int32_t getNamedUniformLocation(const std::string_view& name);

  private:
    GLuint m_programID;
    std::unordered_map<std::string, uint32_t> m_namedUniformCache;
    static GLuint s_activeProgram;
  };
}