#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <scope_guard.hpp>
#include <string>

static const std::string window_title{"HelloWindow"};
static constexpr int window_width{800};
static constexpr int window_height{600};

int main() {
  glfwSetErrorCallback([](int error_code, const char *description) {
    std::cerr << "error_code: " << error_code << " description: " << description
              << '\n';
  });

  if (!glfwInit()) {
    std::cerr << "Failed to initialize glfw\n";
    return 1;
  }
  SCOPE_EXIT { glfwTerminate(); };

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  auto window{glfwCreateWindow(window_width, window_height,
                               window_title.c_str(), nullptr, nullptr)};
  if (!window) {
    std::cerr << "Failed to create window\n";
    return 1;
  }
  SCOPE_EXIT { glfwDestroyWindow(window); };

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize OpenGL context\n";
    return 1;
  }

  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow *window, int width, int height) {
                                   glViewport(0, 0, width, height);
                                 });

  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode,
                                int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  });

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}
