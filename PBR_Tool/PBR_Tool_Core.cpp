#include "pbr_tool_core.hpp"
#include <glad/gl.h>
#include "debug.hpp"
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

        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(openGLDebugCallback, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }

        config.GPURenderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

        glViewport(0, 0, config.width, config.height);
    }

    void App::run()
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        if (m_scene)
            m_scene->init();

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

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (m_scene)
                m_scene->render();

            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
        glfwTerminate();
    }
}