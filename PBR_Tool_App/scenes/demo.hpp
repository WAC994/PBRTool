#pragma once
#include <scene.hpp>
#include <screen.hpp>
#include <texture.hpp>
#include <shader.hpp>
#include <camera.hpp>
#include <mesh.hpp>

using namespace pt;

class ScreenTexture : public Scene
{
    Screen demo;
    uint32_t logo = gen2DTextureFromFile("assets/images/w.png", GL_SRGB8);
    void render() override
    {
        demo.draw(logo);
    }
};

class ScreenColor : public Scene
{
    Screen demo;
    Shader color = Shader("assets/shaders/screen.vert", "assets/shaders/color.frag", SHADERFILEINFO::TEXTFILE);
    void render() override
    {
        color.use();
        demo.draw();
    }
};

class MeshColor : public Scene
{
    Camera camera;
    Mesh demo = Mesh("assets/models/headphone/headphone.gltf");
    Shader color = Shader("assets/shaders/mesh.vert", "assets/shaders/color.frag", SHADERFILEINFO::TEXTFILE);
    void init() override
    {
        camera.update();
    }

    void render() override
    {
        color.use();
        demo.draw();
    }
};

class MeshTexture : public Scene
{
    Camera camera;
    Mesh demo = Mesh("assets/models/headphone/headphone.gltf");
    Shader shader = Shader("assets/shaders/mesh.vert", "assets/shaders/texture.frag", SHADERFILEINFO::TEXTFILE);
    uint32_t texture = gen2DTextureFromFile("assets/models/headphone/Headphones_1001_baseColor.jpg");
    void init() override
    {
        camera.update();
    }

    void render() override
    {
        shader.bindTexture(0, texture);
        demo.draw();
    }
};