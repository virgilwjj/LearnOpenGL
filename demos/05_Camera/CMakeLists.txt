cmake_minimum_required(VERSION 3.0.0)
project(Camera)

add_executable(${PROJECT_NAME} main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED OFF
)

target_link_libraries(${PROJECT_NAME} glad)

target_link_libraries(${PROJECT_NAME} glfw)

target_link_libraries(${PROJECT_NAME} scope_guard)

target_link_libraries(${PROJECT_NAME} glm)
