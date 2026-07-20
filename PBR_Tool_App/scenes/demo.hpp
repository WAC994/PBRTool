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

using namespace pt;

class PBRTest : public Scene
{
    Shader gBufferShader = Shader("assets/shaders/mesh_batched.vert", "assets/shaders/gbuffer.frag", SHADERFILEINFO::TEXTFILE);

    Camera camera;
    MaterialManager materials;

    FrameBuffer gBuffer;
    std::array<uint32_t, MAX_G_BUFFER_TEXTURES> gBufferTextures = genGBufferTextures(App::getInstance().config.width, App::getInstance().config.height);
    uint32_t gBufferDepth = genRenderTexture(App::getInstance().config.width, App::getInstance().config.height, GL_DEPTH_COMPONENT24);

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
        gBuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gBufferShader.use();
        BatchRenderer::batch(gravity_generator);
        BatchRenderer::batch(M870);
        BatchRenderer::batch(headphone);
        BatchRenderer::draw();

        FrameBuffer::bindDefault();
        screen.draw(gBufferTextures.at(0));

        materials.update();

        Light red(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f));
        Light green(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f));
        Light blue(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f));
        Light yellow(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f));
        LightManager::update();
    }
};