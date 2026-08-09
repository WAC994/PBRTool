#pragma once
#include <scene.hpp>
#include <screen.hpp>
#include <texture.hpp>
#include <shader.hpp>
#include <camera.hpp>
#include <model.hpp>
#include <batch_renderer.hpp>
#include <inputs.hpp>
#include <framebuffer.hpp>
#include <lights.hpp>
#include <render_tools.hpp>
#include <iostream>

using namespace pt;

class Learn : public Scene
{
    Shader textureShader = Shader("assets/shaders/mesh.vert", "assets/shaders/texture.frag", SHADERFILEINFO::TEXTFILE);
    uint32_t redTexture = gen2DTextureFromFile("assets/images/circle_red.png", GL_SRGB8_ALPHA8);
    uint32_t greenTexture = gen2DTextureFromFile("assets/images/circle_green.png", GL_SRGB8_ALPHA8);
    uint32_t blueTexture = gen2DTextureFromFile("assets/images/circle_blue.png", GL_SRGB8_ALPHA8);
    Camera camera;
    Model plane_1 = Model("assets/models/plane_1.obj", false);
    Model plane_2 = Model("assets/models/plane_1.obj", false);
    Model plane_3 = Model("assets/models/plane_1.obj", false);

    void onStart() override
    {
        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        plane_3.translate(glm::vec3(0.0f, 0.0f, -2.0f));
        plane_2.translate(glm::vec3(0.0f, 0.0f, -1.0f));
    }

    void onEvent(Event& event) override
    {
        EventManager eventManager(event);

        eventManager.dispatch<MousePositionEvent>([this](MousePositionEvent& e)
            {
                camera.rotate(e.dx, e.dy);
            });
    }

    void onUpdate(float deltaTime) override
    {
        camera.move(Input::axis2D(), deltaTime);
        plane_1.translate(glm::vec3(Input::axis2D(AxisInput::ARROWS).x, 0.0f, 0.0f) * deltaTime);
    }

    void onRender() override
    {
        textureShader.use();

        glBindTextureUnit(0, blueTexture);
        plane_3.draw();

        glBindTextureUnit(0, greenTexture);
        plane_2.draw();

        glBindTextureUnit(0, redTexture);
        plane_1.draw();
    }
};