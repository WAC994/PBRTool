#pragma once
#include <iostream>
#include <glad/gl.h>

#ifdef __linux__
    #define STDCL
#else
    #define STDCL __stdcall
#endif

inline void STDCL openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    std::clog << "Log: Debug message id (" << id << ")\n";
    std::cerr << "Error: " << message << "\n";

    switch (source)
    {
    case GL_DEBUG_SOURCE_API: std::clog << "Source: API\n";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::clog << "Source: Window System\n";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::clog << "Source: Shader Compiler\n";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: std::clog << "Source: Third Party\n";
        break;
    case GL_DEBUG_SOURCE_APPLICATION: std::clog << "Source: Application\n";
        break;
    case GL_DEBUG_SOURCE_OTHER: std::clog << "Source: Other\n";
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR: std::cerr << "Type: Error\n";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Type: Deprecated Behaviour\n";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cerr << "Type: Undefined Behaviour\n";
        break;
    case GL_DEBUG_TYPE_PORTABILITY: std::cerr << "Type: Portability\n";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE: std::cerr << "Type: Performance\n";
        break;
    case GL_DEBUG_TYPE_MARKER: std::cerr << "Type: Marker\n";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP: std::cerr << "Type: Push Group\n";
        break;
    case GL_DEBUG_TYPE_POP_GROUP: std::cerr << "Type: Pop Group\n";
        break;
    case GL_DEBUG_TYPE_OTHER: std::cerr << "Type: Other\n";
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH: std::clog << "Severity: high\n";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM: std::clog << "Severity: medium\n";
        break;
    case GL_DEBUG_SEVERITY_LOW: std::clog << "Severity: low\n";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::clog << "Severity: notification\n";
        break;
    }

    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        std::cerr << "Aborting due to OpenGL error\n";
        {
#ifdef _MSC_VER
            __debugbreak(); // Visual Studio breakpoint
#else
            __builtin_trap(); // GCC/Clang breakpoint
#endif
        }
    }
}
