cmake_minimum_required(VERSION 3.20)

project(PBRTool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add main library
add_subdirectory(PBR_Tool)

# add vendor dependencies
include(vendor.cmake)

# add application
add_subdirectory(PBR_Tool_App)