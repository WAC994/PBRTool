add_library(PBR_Tool_Core STATIC)

target_link_libraries(PBR_Tool_Core PUBLIC glfw glm assimp)

target_sources(PBR_Tool_Core PUBLIC pbr_tool_core.hpp
    config.hpp
    scene.hpp
    texture.hpp
    screen.hpp
    shader.hpp
    camera.hpp
    mesh.hpp
    framebuffer.hpp
    batch_renderer.hpp
    transform.hpp
    model.hpp
    event.hpp
    inputs.hpp
    material.hpp
    lights.hpp
    render_tools.hpp)

target_sources(PBR_Tool_Core PRIVATE pbr_tool_core.cpp
    config.cpp
    vendor/glad/src/gl.c
    vendor/stb_image/stb_image.c
    texture.cpp
    shader.cpp
    screen.cpp
    camera.cpp
    mesh.cpp
    utilities.hpp 
    utilities.cpp
    framebuffer.cpp
    debug.hpp
    batch_renderer.cpp
    transform.cpp
    model.cpp
    inputs.cpp
    material.cpp
    lights.cpp
    render_tools.cpp)

target_include_directories(PBR_Tool_Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image)
