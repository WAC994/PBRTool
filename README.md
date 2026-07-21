# **PBR Tool**
## An OpenGL Physically Based Rendering (PBR) Sandbox
## Contents
1. [Features](#features)
2. [Build Instructions](#build-instructions) 
3. [Guide](#guide)
***
## Features
1. C++ 20 support
2. Cross-Platform (*tested on Windows and Ubuntu*) 
   > Uses GLFW as windowing library
4. Uses OpenGL 4.6 core and ARB_BindlessTextures extension
5. Core-App system
6. PBR material system based loosely on GLTF specification
7. Added functionality for tools and effects such as:
   - point lights
   - image-based lighting (IBL)
   - simple transformations (translate, rotate and scale)
   - tone mapping
   - physically based bloom
***
## Build Instructions
Normal build instructions for a CMake project
  - ```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release   # Or Debug, RelWithDebInfo
    ```
***
## Guide
### 1. Run the demo application  
   In your IDE, select **"PBR_TOOL_App"** as the defualt target
