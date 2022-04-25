#include <GLFW/glfw3.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <cmath>
#include <iostream>
#include <scope_guard.hpp>
#include <stb_image.h>
#include <string>

static const std::string window_title{"HelloTexture"};
static constexpr int window_width{800};
static constexpr int window_height{600};

static void error_callback(int error_code, const char *description) {
  std::cerr << "error_code: " << error_code << " description: " << description
            << '\n';
}

static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
  glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

static const std::string vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec3 a_position;\n"
    "layout (location = 1) in vec2 a_tex_coord;\n"
    "\n"
    "out vec2 v_tex_coord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "  v_tex_coord = a_tex_coord;\n"
    "  gl_Position = vec4(a_position, 1.0);\n"
    "}";

static const std::string fragment_shader_source =
    "#version 330 core\n"
    "uniform sampler2D texture1;\n"
    "\n"
    "in vec2 v_tex_coord;\n"
    "\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "  FragColor = texture(texture1, v_tex_coord);\n"
    "}";

static const std::string texture_path = "resources/textures/container.jpg";

int main() {
  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
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
    return 1;
  }
  SCOPE_EXIT { glfwDestroyWindow(window); };

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize OpenGL context\n";
    return 1;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, key_callback);

  GLint success;
  constexpr GLsizei infobuffer_size{512};
  GLchar infobuffer[infobuffer_size];

  auto shader_program{glCreateProgram()};
  SCOPE_EXIT { glDeleteProgram(shader_program); };

  {
    auto vertex_shader{glCreateShader(GL_VERTEX_SHADER)};
    SCOPE_EXIT { glDeleteShader(vertex_shader); };
    auto vertex_shader_code{vertex_shader_source.c_str()};
    glShaderSource(vertex_shader, 1, &vertex_shader_code, nullptr);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vertex_shader, infobuffer_size, nullptr, infobuffer);
      std::cerr << infobuffer << '\n';
    }

    auto fragment_shader{glCreateShader(GL_FRAGMENT_SHADER)};
    SCOPE_EXIT { glDeleteShader(fragment_shader); };
    auto fragment_shader_code{fragment_shader_source.c_str()};
    glShaderSource(fragment_shader, 1, &fragment_shader_code, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fragment_shader, infobuffer_size, nullptr, infobuffer);
      std::cerr << infobuffer << '\n';
    }

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader_program, infobuffer_size, nullptr, infobuffer);
      std::cerr << infobuffer << '\n';
    }
  }

  float vertices[] = {
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,
  };

  unsigned int indices[] = {
      0, 1, 3, 1, 2, 3,
  };

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  SCOPE_EXIT { glDeleteVertexArrays(1, &VAO); };

  GLuint VBO;
  glGenBuffers(1, &VBO);
  SCOPE_EXIT { glDeleteBuffers(1, &VBO); };

  GLuint EBO;
  glGenBuffers(1, &EBO);
  SCOPE_EXIT { glDeleteBuffers(1, &EBO); };

  GLuint texture;
  glGenTextures(1, &texture);
  SCOPE_EXIT { glDeleteTextures(1, &texture); };

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  GLsizei width, height;
  int nrChannels;
  auto data{stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0)};
  if (!data) {
    std::cerr << "Failed to load image\n";
  }
  SCOPE_EXIT { stbi_image_free(data); };
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}