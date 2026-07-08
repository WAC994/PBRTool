#pragma once
#include <scene.hpp>
#include <screen.hpp>
#include <texture.hpp>
#include <shader.hpp>

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