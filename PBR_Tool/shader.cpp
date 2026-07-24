#include "shader.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <glm/gtc/type_ptr.hpp>

struct ShaderData
{
    std::string Type;
    GLenum GLType;
};

static void logCompileError(GLuint ID, std::string_view type)
{
    int32_t success;
    char infoLog[1024];
    glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(ID, 1024, NULL, infoLog);
        std::cerr << "Compilation error in " << type << " shader\n" << infoLog << '\n';
    }
}

static void logLinkError(GLuint program)
{
    int32_t success;
    char infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cerr << "Linking error in program\n" << infoLog << '\n';
    }
}

static std::string fileToCode(const std::string_view& file)
{
    std::string code;
    std::ifstream inputFile(file.data());
    if (inputFile.is_open())
        code = std::string(std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());
    else
        std::cerr << "Could not open " << file << '\n';
    return code;
}
namespace pt
{
    GLuint Shader::s_activeProgram = 0;

    static std::unordered_map<ShaderType, ShaderData> s_shaderTypeMap = {
    {ShaderType::VERT, {"vertex", GL_VERTEX_SHADER}},
    {ShaderType::FRAG, {"fragment", GL_FRAGMENT_SHADER}},
    {ShaderType::TESSCONT, {"tesselation control", GL_TESS_CONTROL_SHADER}},
    {ShaderType::TESSEVAL, {"tesselation evaluation", GL_TESS_EVALUATION_SHADER}},
    {ShaderType::GEOM, {"geometry", GL_GEOMETRY_SHADER}} };

    void Shader::compileshader(ShaderType shaderType, const char* code)
    {
        GLuint shader = glCreateShader(s_shaderTypeMap[shaderType].GLType);
        glShaderSource(shader, 1, &code, NULL);
        glCompileShader(shader);
        logCompileError(shader, s_shaderTypeMap[shaderType].Type);
        glAttachShader(m_programID, shader);
        glDeleteShader(shader);
    }

    Shader::Shader()
    {
        m_programID = glCreateProgram();
    }

    Shader::Shader(const std::string_view& vert, const std::string_view& frag, SHADERFILEINFO info)
    {
        m_programID = glCreateProgram();
        switch (info)
        {
        case SHADERFILEINFO::STRING:
            compileshader(ShaderType::VERT, vert.data());
            compileshader(ShaderType::FRAG, frag.data());
            break;
        case SHADERFILEINFO::TEXTFILE:
            compileshader(ShaderType::VERT, fileToCode(vert).c_str());
            compileshader(ShaderType::FRAG, fileToCode(frag).c_str());
            break;
        }
        linkProgram();
    }

    void Shader::addShader(ShaderType shaderType, const std::string_view& shader, SHADERFILEINFO info)
    {
        switch (info)
        {
        case SHADERFILEINFO::STRING:
            compileshader(shaderType, shader.data());
            break;
        case SHADERFILEINFO::TEXTFILE:
            compileshader(shaderType, fileToCode(shader).c_str());
            break;
        }
    }

    void Shader::linkProgram() const
    {
        glLinkProgram(m_programID);
        logLinkError(m_programID);
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_programID);
    }

    void Shader::use() const
    {
        if (m_programID != s_activeProgram)
        {
            glUseProgram(m_programID);
            s_activeProgram = m_programID;
        }
    }

    void Shader::setInt(int32_t loc, int32_t v0) const
    {
        glProgramUniform1i(m_programID, loc, v0);
    }

    void Shader::setBool(int32_t loc, bool value) const
    {
        glProgramUniform1ui(m_programID, loc, static_cast<uint32_t>(value));
    }

    void Shader::setUint(int32_t loc, uint32_t v0) const
    {
        glProgramUniform1ui(m_programID, loc, v0);
    }

    void Shader::setFloat(int32_t loc, float v0) const
    {
        glProgramUniform1f(m_programID, loc, v0);
    }

    void Shader::setFloat2(int32_t loc, float v0, float v1) const
    {
        glProgramUniform2f(m_programID, loc, v0, v1);
    }

    void Shader::setFloat3(int32_t loc, float v0, float v1, float v2) const
    {
        glProgramUniform3f(m_programID, loc, v0, v1, v2);
    }

    void Shader::setFloat4(int32_t loc, float v0, float v1, float v2, float v3) const
    {
        glProgramUniform4f(m_programID, loc, v0, v1, v2, v3);
    }

    void Shader::setVec2(int32_t loc, const glm::vec2& value, int32_t count) const
    {
        glProgramUniform2fv(m_programID, loc, count, glm::value_ptr(value));
    }

    void Shader::setVec3(int32_t loc, const glm::vec3& value, int32_t count) const
    {
        glProgramUniform3fv(m_programID, loc, count, glm::value_ptr(value));
    }

    void Shader::setVec4(int32_t loc, const glm::vec4& value, int32_t count) const
    {
        glProgramUniform4fv(m_programID, loc, count, glm::value_ptr(value));
    }

    void Shader::setMat2(int32_t loc, const glm::mat2& value, int32_t count) const
    {
        glProgramUniformMatrix2fv(m_programID, loc, count, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::setMat3(int32_t loc, const glm::mat3& value, int32_t count) const
    {
        glProgramUniformMatrix3fv(m_programID, loc, count, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::setMat4(int32_t loc, const glm::mat4& value, int32_t count) const
    {
        glProgramUniformMatrix4fv(m_programID, loc, count, GL_FALSE, glm::value_ptr(value));
    }

    int32_t Shader::getNamedUniformLocation(const std::string_view& name)
    {
        int32_t uniformLocation = -1;
        if (m_namedUniformCache.find(name.data()) == m_namedUniformCache.end())
        {
            uniformLocation = glGetUniformLocation(m_programID, name.data());
            m_namedUniformCache[name.data()] = uniformLocation;
            if (uniformLocation == -1)
                std::cerr << name << " uniform is not avalible in the shader program.\n";
            return uniformLocation;
        }
        else
            return m_namedUniformCache[name.data()];
    }

    void Shader::setNamedInt(const std::string_view& name, int32_t v0)
    {
        glProgramUniform1i(m_programID, getNamedUniformLocation(name), v0);
    }

    void Shader::setNamedUint(const std::string_view& name, uint32_t v0)
    {
        glProgramUniform1ui(m_programID, getNamedUniformLocation(name), v0);
    }

    void Shader::setNamedFloat(const std::string_view& name, float v0)
    {
        glProgramUniform1f(m_programID, getNamedUniformLocation(name), v0);
    }

    void Shader::setNamedFloat2(const std::string_view& name, float v0, float v1)
    {
        glProgramUniform2f(m_programID, getNamedUniformLocation(name), v0, v1);
    }

    void Shader::setNamedFloat3(const std::string_view& name, float v0, float v1, float v2)
    {
        glProgramUniform3f(m_programID, getNamedUniformLocation(name), v0, v1, v2);
    }

    void Shader::setNamedFloat4(const std::string_view& name, float v0, float v1, float v2, float v3)
    {
        glProgramUniform4f(m_programID, getNamedUniformLocation(name), v0, v1, v2, v3);
    }

    void Shader::SetNamedVec2(const std::string_view& name, const glm::vec2& value, int32_t count)
    {
        glProgramUniform2fv(m_programID, getNamedUniformLocation(name), count, glm::value_ptr(value));
    }

    void Shader::setNamedVec3(const std::string_view& name, const glm::vec3& value, int32_t count)
    {
        glProgramUniform3fv(m_programID, getNamedUniformLocation(name), count, glm::value_ptr(value));
    }

    void Shader::setNamedVec4(const std::string_view& name, const glm::vec4& value, int32_t count)
    {
        glProgramUniform4fv(m_programID, getNamedUniformLocation(name), count, glm::value_ptr(value));
    }

    void Shader::setNamedMat2(const std::string_view& name, const glm::mat2& value, int32_t count)
    {
        glProgramUniformMatrix2fv(m_programID, getNamedUniformLocation(name), count, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::setNamedMat3(const std::string_view& name, const glm::mat3& value, int32_t count)
    {
        glProgramUniformMatrix3fv(m_programID, getNamedUniformLocation(name), count, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::setNamedMat4(const std::string_view& name, const glm::mat4& value, int32_t count)
    {
        glProgramUniformMatrix4fv(m_programID, getNamedUniformLocation(name), count, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::bindTexture(uint32_t unit, uint32_t texture) const
    {
        use();
        glBindTextureUnit(unit, texture);
    }

    void Shader::bindTextures(uint32_t first, int32_t count, const uint32_t* textures) const
    {
        use();
        glBindTextures(first, count, textures);
    }

    void Shader::useBindlessTexture(const std::string_view& name, uint64_t texture)
    {
        use();
        glUniformHandleui64ARB(getNamedUniformLocation(name), texture);
    }
}