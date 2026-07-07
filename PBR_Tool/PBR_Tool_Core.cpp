#include "pbr_tool_core.hpp"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>
#include <format>
#include <iostream>
#include <cstdint>

namespace pt
{
    App& App::getInstance()
    {
        static App instance;
        return instance;
    }

    App::App()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(config.width, config.height, "PBR Tool", nullptr, nullptr);

        glfwMakeContextCurrent(m_window);

        glfwSwapInterval(config.enableVSync ? 1 : 0);

        gladLoadGL(glfwGetProcAddress);

        config.GPURenderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

        glViewport(0, 0, config.width, config.height);
    }

    void App::run()
    {
        double lastFrameTime = glfwGetTime();
        std::string frameInfo;
        float frameTimeSum = 0.0f;
        uint32_t frameCount = 0;
        while (!glfwWindowShouldClose(m_window))
        {
            double currentFrameTime = glfwGetTime();
            float deltaTime = static_cast<float>(currentFrameTime - lastFrameTime);

            if (m_scene)
                m_scene->update(deltaTime);

            lastFrameTime = currentFrameTime;

            frameTimeSum += deltaTime;
            frameCount++;
            if (frameTimeSum > 0.5f)
            {
                float averageDeltaTime = frameTimeSum / static_cast<float>(frameCount);
                frameInfo = std::format("PBR Tool - GPU: {} - Frame Time: {:.3f} ms - Frame Rate: {:.1f} FPS", config.GPURenderer, averageDeltaTime * 1000.0f, 1.0f / averageDeltaTime);
                glfwSetWindowTitle(m_window, frameInfo.c_str());
                frameTimeSum = 0.0f;
                frameCount = 0;
            }

            glClearColor(1.0f, 0.5f, 0.25f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            if (m_scene)
                m_scene->render();

            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
        glfwTerminate();
    }
}