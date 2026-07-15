#pragma once
#include <scene.hpp>
#include <screen.hpp>
#include <texture.hpp>
#include <shader.hpp>
#include <camera.hpp>
#include <mesh.hpp>
#include <model.hpp>
#include <batch_renderer.hpp>
#include <inputs.hpp>
#include <material.hpp>

using namespace pt;

class ScreenTexture : public Scene
{
    Screen demo;
    uint32_t logo = gen2DTextureFromFile("assets/images/w.png", GL_SRGB8);
    void onRender() override
    {
        demo.draw(logo);
    }
};

class ScreenColor : public Scene
{
    Screen demo;
    Shader color = Shader("assets/shaders/screen.vert", "assets/shaders/color.frag", SHADERFILEINFO::TEXTFILE);
    void onRender() override
    {
        color.use();
        demo.draw();
    }
};

class MeshColor : public Scene
{
    Camera camera;
    Mesh demo = Mesh("assets/models/headphone/headphone.gltf", 0);
    Shader color = Shader("assets/shaders/mesh.vert", "assets/shaders/color.frag", SHADERFILEINFO::TEXTFILE);
    void onStart() override
    {
        camera.update();
    }

    void onRender() override
    {
        color.use();
        demo.draw();
    }
};

class MeshTexture : public Scene
{
    Camera camera;
    Mesh demo = Mesh("assets/models/headphone/headphone.gltf", 0);
    Shader shader = Shader("assets/shaders/mesh.vert", "assets/shaders/texture.frag", SHADERFILEINFO::TEXTFILE);
    uint32_t texture = gen2DTextureFromFile("assets/models/headphone/Headphones_1001_baseColor.jpg");
    void onStart() override
    {
        camera.update();
    }

    void onRender() override
    {
        shader.bindTexture(0, texture);
        demo.draw();
    }
};

class MeshBatchedTexture : public Scene
{
    Camera camera;
    Model headphone = Model("assets/models/headphone/headphone.gltf");
    Model M870 = Model("assets/models/M870/M870.gltf");
    Model gravity_generator = Model("assets/models/gravity-generator/gravity-generator.gltf");
    Shader shader = Shader("assets/shaders/mesh_batched.vert", "assets/shaders/material.frag", SHADERFILEINFO::TEXTFILE);
    uint64_t texture = genBindless2DTextureFromFile("assets/models/headphone/Headphones_1001_baseColor.jpg");
    MaterialManager materials;

    void onStart() override
    {
        camera.update();
        BatchRenderer::init();
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
        headphone.translate(glm::vec3(Input::axis2D(AxisInput::ARROWS) * deltaTime, 0.0f));
    }

    void onRender() override
    {
        shader.use();
        if (Input::keyPressed(GLFW_KEY_U))
        {
            BatchRenderer::batch(gravity_generator);
        }

        BatchRenderer::batch(M870);

        if (Input::keyReleased(GLFW_KEY_O))
        {
            BatchRenderer::batch(headphone);
        }
        
        BatchRenderer::draw();

        materials.update();
    }
};