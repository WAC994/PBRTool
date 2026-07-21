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
#include <framebuffer.hpp>
#include <lights.hpp>
#include <render_tools.hpp>

using namespace pt;

class PBRTest : public Scene
{
    Shader gBufferShader = Shader("assets/shaders/mesh_batched.vert", "assets/shaders/gbuffer.frag", SHADERFILEINFO::TEXTFILE);
    Shader lightingShader = Shader("assets/shaders/screen.vert", "assets/shaders/lighting_deferred.frag", SHADERFILEINFO::TEXTFILE);
    Shader colorShader = Shader("assets/shaders/screen.vert", "assets/shaders/post_process.frag", SHADERFILEINFO::TEXTFILE);

    Camera camera;
    MaterialManager materials;

    FrameBuffer gBuffer;
    std::array<uint32_t, MAX_G_BUFFER_TEXTURES> gBufferTextures = genGBufferTextures(App::getInstance().config.width, App::getInstance().config.height);
    uint32_t gBufferDepth = genRenderTexture(App::getInstance().config.width, App::getInstance().config.height, GL_DEPTH_COMPONENT24);

    FrameBuffer deferredBuffer;
    uint32_t deferredTarget =  genRenderTexture(App::getInstance().config.width, App::getInstance().config.height, GL_R11F_G11F_B10F);

    Screen screen;

    Model headphone = Model("assets/models/headphone/headphone.gltf");
    Model M870 = Model("assets/models/M870/M870.gltf");
    Model gravity_generator = Model("assets/models/gravity-generator/gravity-generator.gltf");

    void onStart() override
    {
        BatchRenderer::init();
        LightManager::init();
        gBuffer.colorAttachments(gBufferTextures.data(), 0, gBufferTextures.size(), 0);
        gBuffer.depthAttachment(gBufferDepth, 0);
        deferredBuffer.colorAttachment(deferredTarget, 0, 0);
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
        /* Deferred Rendering */
        // G Buffer pass
        gBuffer.bind();
        gBufferShader.use();
        // BatchRenderer::batch(gravity_generator);
        // BatchRenderer::batch(M870);
        BatchRenderer::batch(headphone);
        BatchRenderer::draw();

        // Lighting pass
        deferredBuffer.bind();
        lightingShader.bindTextures(0, 6, gBufferTextures.data());
        screen.draw();

        // Blit Deferred Renders depth with Forward Depth

        /* Forward Rendering */

        // Post-process pass
        FrameBuffer::bindDefault();
        colorShader.bindTexture(0, deferredTarget);
        screen.draw();

        materials.update();

        Light bulb1(glm::vec3(200.0f, 200.0f, 180.0f), glm::vec3(2.0f, 2.5f, 2.0f));
        Light bulb2(glm::vec3(50.0f, 10.0f, 0.0f), glm::vec3(-2.0f, 0.0f, -2.0f));
        LightManager::update();
    }
};

class CubeTest : public Scene
{
    Shader colorShader = Shader("assets/shaders/mesh.vert", "assets/shaders/color.frag", SHADERFILEINFO::TEXTFILE);
    Camera camera;

    void onStart() override
    {
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
    }

    void onRender() override
    {
        colorShader.use();
        drawCube();
    }
};