#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <cmath>
#include <glad/glad.h>
#include <iostream>
#include <scope_guard.hpp>
#include <stb_image.h>
#include <string>

static const std::string window_title{"HelloTexture"};
static constexpr int window_width{800};
static constexpr int window_height{600};
static const std::string texture_path = "resources/textures/container.jpg";

static const std::string vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec3 a_position;\n"
    "layout (location = 1) in vec2 a_tex_coord;\n"
    "\n"
    "uniform sampler2D u_texture0;\n"
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
    "uniform sampler2D u_texture0;\n"
    "\n"
    "in vec2 v_tex_coord;\n"
    "\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "  FragColor = texture(u_texture0, v_tex_coord);\n"
    "}";

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
  glBindVertexArray(VAO);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  SCOPE_EXIT { glDeleteBuffers(1, &VBO); };
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  GLuint EBO;
  glGenBuffers(1, &EBO);
  SCOPE_EXIT { glDeleteBuffers(1, &EBO); };
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  GLuint u_texture0;
  glGenTextures(1, &u_texture0);
  SCOPE_EXIT { glDeleteTextures(1, &u_texture0); };
  glBindTexture(GL_TEXTURE_2D, u_texture0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  {
    GLsizei image_width, image_height;
    int image_channels;
    stbi_set_flip_vertically_on_load(true);
    auto image_data{stbi_load(texture_path.c_str(), &image_width, &image_height,
                              &image_channels, 0)};
    if (!image_data) {
      std::cerr << "Failed to load image\n";
      return 1;
    }
    SCOPE_EXIT { stbi_image_free(image_data); };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, u_texture0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}