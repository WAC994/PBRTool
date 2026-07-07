add_library(PBR_Tool_Core STATIC)

target_link_libraries(PBR_Tool_Core PUBLIC glfw)

target_sources(PBR_Tool_Core PUBLIC pbr_tool_core.hpp
    config.hpp
    scene.hpp
    texture.hpp)
target_sources(PBR_Tool_Core PRIVATE pbr_tool_core.cpp
    vendor/glad/src/gl.c
    vendor/stb_image/stb_image.c
    texture.cpp)

target_include_directories(PBR_Tool_Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image)


