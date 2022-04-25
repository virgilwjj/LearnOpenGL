cmake_minimum_required(VERSION 3.0.0)
project(LearnOpenGL)

add_subdirectory(third-party/glad)
add_subdirectory(third-party/glfw)
add_subdirectory(third-party/scope_guard)
include_directories(third-party/stb)

add_subdirectory(demos/00_HelloWindow)
add_subdirectory(demos/01_HelloTriangle)
add_subdirectory(demos/02_HelloShader)
add_subdirectory(demos/03_HelloTexture)