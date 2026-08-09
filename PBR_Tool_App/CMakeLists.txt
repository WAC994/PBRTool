add_executable(PBR_Tool_App main.cpp
    scenes/learn.hpp)

target_link_libraries(PBR_Tool_App PRIVATE PBR_Tool_Core)

target_include_directories(PBR_Tool_App PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/scenes)