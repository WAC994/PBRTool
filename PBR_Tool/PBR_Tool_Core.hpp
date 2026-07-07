#pragma once
#include "config.hpp"

// Forward declare GLFWwindow to avoid including the GLFW header in this header file
struct GLFWwindow;

namespace pt
{
    class App
    {
    public:
        App();
        void run();
    private:
    public:
        Config config;
    private:
        GLFWwindow* m_window;
    };
}