#include "pbr_tool_core.hpp"
#include <glad/gl.h>
#include "debug.hpp"
#include <GLFW/glfw3.h>
#include <string>
#include <format>
#include <iostream>
#include "utilities.hpp"

namespace pt
{
    App& App::getInstance()
    {
        static App instance;
        return instance;
    }

    App::App() : m_firstMouse(true)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

        m_window = glfwCreateWindow(config.width, config.height, "PBR Tool", config.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

        glfwMakeContextCurrent(m_window);

        glfwSwapInterval(config.vSync ? 1 : 0);

        glfwSetInputMode(m_window, GLFW_CURSOR, config.hideCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

        glfwSetWindowUserPointer(m_window, this);

        gladLoadGL(glfwGetProcAddress);

        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (config.debug)
        {
            if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
            {
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                glDebugMessageCallback(openGLDebugCallback, nullptr);
                glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
            }
        }

        config.GPURenderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

        eventCallbacks();
    }

    void App::runEvent(Event& event)
    {
        if (m_scene)
            m_scene->onEvent(event);
    }

    void App::eventCallbacks()
    {
        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mode)
            {
                App& thisWindow = *((App*)glfwGetWindowUserPointer(window));
                KeyEvent event;
                event.key = key;
                event.action = action;
                thisWindow.runEvent(event);

                // Reserved close window event
                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                    thisWindow.close();
            });

        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
            {
                App& thisWindow = *((App*)glfwGetWindowUserPointer(window));
                MouseButtonEvent event;
                event.button = button;
                event.action = action;
                thisWindow.runEvent(event);
            });

        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
            {
                App& thisWindow = *((App*)glfwGetWindowUserPointer(window));
                MousePositionEvent event;
                event.x = xpos;
                event.y = ypos;

                if (thisWindow.m_firstMouse)
                {
                    thisWindow.m_mousePosX = xpos;
                    thisWindow.m_mousePosY = ypos;
                    thisWindow.m_firstMouse = false;
                }

                event.dx = xpos - thisWindow.m_mousePosX;
                event.dy = thisWindow.m_mousePosY - ypos;

                thisWindow.m_mousePosX = xpos;
                thisWindow.m_mousePosY = ypos;
                thisWindow.runEvent(event);
            });

        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
            {
                App& thisWindow = *((App*)glfwGetWindowUserPointer(window));
                FramebufferResizeEvent event;
                event.width = width;
                event.height = height;
                thisWindow.runEvent(event);
            });
    }

    void App::run()
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        if (m_scene)
            m_scene->onStart();

        double lastFrameTime = glfwGetTime();
        std::string frameInfo;
        float frameTimeSum = 0.0f;
        uint32_t frameCount = 0;
        while (!glfwWindowShouldClose(m_window))
        {
            double currentFrameTime = glfwGetTime();
            float deltaTime = static_cast<float>(currentFrameTime - lastFrameTime);

            if (m_scene)
                m_scene->onUpdate(deltaTime);

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
                m_scene->onRender();

            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
        glfwTerminate();
    }

    void App::close()
    {
        glfwSetWindowShouldClose(m_window, true);
    }
}