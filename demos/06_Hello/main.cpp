#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cmath>
#include <glad/glad.h>
#include <iostream>
#include <scope_guard.hpp>
#include <string>

static const std::string window_title{"HelloShader"};
static constexpr int window_width{800};
static constexpr int window_height{600};

static const std::string vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec3 a_position;\n"
    "\n"
    "uniform sampler1DArray framePosition;\n"
    "uniform float u_t;\n"
    "\n"
    "out vec3 cc;\n"
    "vec3 computePosition()\n"
    "{\n"
    "  int prevFrameId = int(floor(u_t / 1.0));\n"
    "  int nextFrameId = prevFrameId;\n"
    "  float dt = u_t - 1.0 * prevFrameId;\n"
    "  vec3 prevPosition;\n"
    "  if (prevFrameId < 1)\n"
    "  {\n"
    "    prevPosition = a_position;\n"
    "  }\n" 
    "  else if (prevFrameId < 2)\n"
    "  {\n"
    "    prevPosition = texture(framePosition, vec2(gl_VertexID / 4.0, floor(prevFrameId - 1))).rgb;\n"
    "  }\n" 
    "  else\n"
    "  {\n"
    "    prevPosition = texture(framePosition, vec2(gl_VertexID / 4.0, 1)).rgb;\n"
    "  }\n" 
    "  vec3 nextPosition;\n"
    "  if (nextFrameId < 1)\n"
    "  {\n"
    "    nextPosition = a_position;\n"
    "  }\n" 
    "  else if (prevFrameId < 2)\n"
    "  {\n"
    "    nextPosition = texture(framePosition, vec2(gl_VertexID / 4.0, nextFrameId - 1)).rgb;\n"
    "  }\n" 
    "  else\n"
    "  {\n"
    "    nextPosition = texture(framePosition, vec2(gl_VertexID / 4.0, 1)).rgb;\n"
    "  }\n" 
    "  return mix(prevPosition, nextPosition, dt);\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "  cc = computePosition();\n"
    "  gl_Position = vec4(cc, 1.0);\n"
    "}";

static const std::string fragment_shader_source =
    "#version 330 core\n"
    "\n"
    "in vec3 cc;\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "  FragColor = vec4(cc, 1.0);\n"
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
      0.9f,  0.9f,  0.0f, 
      0.9f,  -0.9f, 0.0f,
      -0.9f, -0.9f, 0.0f, 
      -0.9f, 0.9f,  0.0f,
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (void *)(0 * sizeof(float)));
  glEnableVertexAttribArray(0);

  GLuint EBO;
  glGenBuffers(1, &EBO);
  SCOPE_EXIT { glDeleteBuffers(1, &EBO); };
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  GLuint u_texture0;
  glGenTextures(1, &u_texture0);
  SCOPE_EXIT { glDeleteTextures(1, &u_texture0); };
  glBindTexture(GL_TEXTURE_1D_ARRAY, u_texture0);
  glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  float texels[24] = 
  {
     // Texels for first image.
      0.5f,  0.5f,  0.0f, 
      0.5f,  -0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f, 
      -0.5f, 0.5f,  0.0f,
     // Texels for second image.
      0.1f,  0.1f,  0.0f, 
      0.1f,  -0.1f, 0.0f,
      -0.1f, -0.1f, 0.0f, 
      -0.1f, 0.1f,  0.0f,
  };
  glTexImage2D(GL_TEXTURE_1D_ARRAY, 0, GL_RGB, 4, 2, 0, GL_RGB, GL_FLOAT, texels);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D_ARRAY, u_texture0);

    auto time_value{glfwGetTime()};
    auto t_value{static_cast<float>((std::sin(time_value) + 1.0) * 1.5)};
    std::cout << "t_value: " << t_value << '\n';
    auto t_location{glGetUniformLocation(shader_program, "u_t")};
    glUniform1f(t_location, t_value);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}
