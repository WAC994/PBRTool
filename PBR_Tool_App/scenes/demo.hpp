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

class PBRTest : public Scene
{
    Shader gBufferShader = Shader("assets/shaders/mesh_batched.vert", "assets/shaders/gbuffer.frag", SHADERFILEINFO::TEXTFILE);
    Shader lightingShader = Shader("assets/shaders/screen.vert", "assets/shaders/lighting_deferred.frag", SHADERFILEINFO::TEXTFILE);
    Shader postProcessShader = Shader("assets/shaders/screen.vert", "assets/shaders/post_process.frag", SHADERFILEINFO::TEXTFILE);
    
    Camera camera;
    
    FrameBuffer gBuffer;
    std::array<uint32_t, MAX_G_BUFFER_TEXTURES> gBufferTextures = genGBufferTextures(App::getInstance().config.width, App::getInstance().config.height);
    uint32_t gBufferDepth = genRenderTexture(App::getInstance().config.width, App::getInstance().config.height, GL_DEPTH_COMPONENT24);
    
    FrameBuffer deferredBuffer;
    uint32_t deferredTarget =  genRenderTexture(App::getInstance().config.width, App::getInstance().config.height, GL_R11F_G11F_B10F);
    uint32_t deferredDepth =  genRenderTexture(App::getInstance().config.width, App::getInstance().config.height, GL_DEPTH_COMPONENT24);

    Screen screen;
    
    Model headphone = Model("assets/models/headphone/headphone.gltf");
    
    Shader enviCubeMapShader = Shader("assets/shaders/cube_map_capture.vert", "assets/shaders/hdri_envi.frag", SHADERFILEINFO::TEXTFILE);
    Shader irraCubeMapShader = Shader("assets/shaders/cube_map_capture.vert", "assets/shaders/irradiance.frag", SHADERFILEINFO::TEXTFILE);
    Shader preFilterCubeMapShader = Shader("assets/shaders/cube_map_capture.vert", "assets/shaders/pre_filter.frag", SHADERFILEINFO::TEXTFILE);
    uint32_t enviTexture = gen2DTextureFromFile("assets/images/autoshop.hdr", GL_RGB16F, GL_CLAMP_TO_EDGE, true);
    uint32_t enviCubeMap;
    uint32_t irraCubeMap;
    uint32_t preFilterCubeMap;

    void onStart() override
    {
        BatchRenderer::init();
        LightManager::init();
        MaterialManager::init();
        
        gBuffer.colorAttachments(gBufferTextures.data(), 0, gBufferTextures.size(), 0);
        gBuffer.depthAttachment(gBufferDepth, 0);
        
        deferredBuffer.colorAttachment(deferredTarget, 0, 0);
        deferredBuffer.depthAttachment(deferredDepth, 0);

        // Generate the environment cube map
        enviCubeMapShader.bindTexture(0, enviTexture);
        enviCubeMap = renderToCubeMap(1024, GL_R11F_G11F_B10F, true);

        // Generate the irradiance cube map
        irraCubeMapShader.bindTexture(0, enviCubeMap);
        irraCubeMap = renderToCubeMap(32, GL_R11F_G11F_B10F);

        // Generate the prefilter environment cube map
        preFilterCubeMapShader.bindTexture(0, enviCubeMap);
        preFilterCubeMap = renderIBLPreFilter(1024, GL_R11F_G11F_B10F, preFilterCubeMapShader);
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
        
        Light bulb1(glm::vec3(200.0f, 200.0f, 180.0f), glm::vec3(2.0f, 2.5f, 2.0f));
        Light bulb2(glm::vec3(50.0f, 10.0f, 0.0f), glm::vec3(-2.0f, 0.0f, -2.0f));
        
        MaterialManager::update();
        LightManager::update();
    }
    
    Shader enviShader = Shader("assets/shaders/sky_box.vert", "assets/shaders/sky_box.frag", SHADERFILEINFO::TEXTFILE);
    
    void onRender() override
    {
        // G Buffer pass
        gBuffer.bind();
        gBufferShader.use();
        BatchRenderer::batch(headphone);
        BatchRenderer::draw();

        // Lighting pass
        deferredBuffer.bind();
        lightingShader.bindTextures(0, 6, gBufferTextures.data());
        lightingShader.bindTexture(6, irraCubeMap);
        screen.draw();

        gBuffer.blit(deferredBuffer.getHandle(), App::getInstance().config.width, App::getInstance().config.height, GL_DEPTH_BUFFER_BIT);

        // Forward render pass
        enviShader.bindTexture(0, preFilterCubeMap);
        glDepthFunc(GL_LEQUAL);
        drawCube(true);
        glDepthFunc(GL_LESS);

        // Post-process pass
        FrameBuffer::bindDefault();
        postProcessShader.bindTexture(0, deferredTarget);
        screen.draw();
    }
};