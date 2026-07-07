add_library(PBR_Tool_Core STATIC)

target_sources(PBR_Tool_Core PUBLIC PBR_Tool_Core.hpp
    config.hpp)
target_sources(PBR_Tool_Core PRIVATE PBR_Tool_Core.cpp
    vendor/glad/src/gl.c)

target_include_directories(PBR_Tool_Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include)
target_link_libraries(PBR_Tool_Core PUBLIC glfw)
