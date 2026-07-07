#pragma once
#include "config.hpp"

// Forward declare GLFWwindow to avoid including the GLFW header in this header file
struct GLFWwindow;

namespace pt
{
    class App
    {
    public:
        static App& getInstance();
        App(const App&) = delete;
        App& operator=(const App&) = delete;
        void run();
    private:
        App();
    public:
        Config config;
    private:
        GLFWwindow* m_window;
    };
}