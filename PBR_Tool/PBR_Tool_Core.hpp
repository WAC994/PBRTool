#pragma once
#include "scene.hpp"
#include "config.hpp"
#include <memory>

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
        template<typename T>
        void setScene();
        void run();
    private:
        App();
    public:
        Config config;
    private:
        GLFWwindow* m_window;
        std::unique_ptr<Scene> m_scene;
    };

    template<typename T>
    inline void App::setScene()
    {
        static_assert(std::is_base_of<Scene, T>::value, "T must be derived from Scene");
        m_scene = std::make_unique<T>();
    }
}