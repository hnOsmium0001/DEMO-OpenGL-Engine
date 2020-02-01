#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Engine.hpp"
#include "Shader.hpp"
#include "GLAttribute.hpp"

void resizeCallback(GLFWwindow* handle, int32_t width, int32_t height) {
  HOEngine::Window::HandleResize(handle, width, height);
  glViewport(0, 0, width, height);
}
 
void keyCallback(GLFWwindow* handle, int32_t key, int32_t scancode, int32_t action, int32_t modss) {
}
 
void charCallback(GLFWwindow* handle, uint32_t codepoint) {
}
 
void cursorPosCallback(GLFWwindow* handle, double x, double y) {
}
 
void cursorButtonCallback(GLFWwindow* handle, int32_t button, int32_t action, int32_t mods) {
}
 
void scrollCallback(GLFWwindow* handle, double x, double y) {
}

class Application : HOEngine::ApplicationBase {
public:
  void Run() {
    auto window = HOEngine::Window::New({ 1024, 768 }, "Test window", {
      .resizeCallback = resizeCallback,
      .keyCallback = keyCallback,
      .charCallback = charCallback,
      .cursorPosCallback = cursorPosCallback,
      .cursorButtonCallback = cursorButtonCallback,
      .scrollCallback = scrollCallback
    });
    if (window == nullptr) {
      std::cerr << "Unable to create GLFW window, aborting\n";
      return;
    }
 
    if (gl3wInit()) {
      std::cerr << "Unable to initialize OpenGL, aborting\n";
      return;
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    HOEngine::ScopeGuard vaoRelease([&]() { glDeleteVertexArrays(1, &vao); });
 
    GLuint vbo;
    glGenBuffers(1, &vbo);
    HOEngine::ScopeGuard vboRelease([&]() { glDeleteBuffers(1, &vbo); });

    GLuint ibo;
    glGenBuffers(1, &ibo);
    HOEngine::ScopeGuard iboRelease([&]() { glDeleteBuffers(1, &ibo); });
 
    auto vshSource = HOEngine::ReadFileAsStr("example/resources/cube3d.vsh").value_or("");
    auto fshSource = HOEngine::ReadFileAsStr("example/resources/cube3d.fsh").value_or("");
    auto program = HOEngine::ShaderProgram::New(vshSource, fshSource);
    if (!program) {
      std::cerr << "Unable to create shader program, aborting\n";
      return;
    }

    glBindVertexArray(vao);
    // Bind attribute pointers for VAO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    HOEngine::VertexAttributes<float[3], float[3], float[2]>::SetupPointers();
    // Bind index (element) buffer for VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindVertexArray(0);

    // Setup vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    HOEngine::VertexAttributes<float[3], float[3], float[2]> verts[8];
    constexpr size_t POS = 0;
    constexpr size_t NORMAL = 1;
    constexpr size_t UV = 2;
    verts[0].Attr<POS>() << 0.0f, 0.0f, 0.0f;
    verts[0].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
    verts[0].Attr<UV>() << 0.0f, 0.0f;

    verts[1].Attr<POS>() << 1.0f, 0.0f, 0.0f;
    verts[1].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
    verts[1].Attr<UV>() << 0.0f, 0.0f;

    verts[2].Attr<POS>() << 1.0f, 1.0f, 0.0f;
    verts[2].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
    verts[2].Attr<UV>() << 0.0f, 0.0f;

    verts[3].Attr<POS>() << 0.0f, 1.0f, 0.0f;
    verts[3].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
    verts[3].Attr<UV>() << 0.0f, 0.0f;
    
    verts[4].Attr<POS>() << 0.0f, 0.0f, 1.0f;
    verts[4].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
    verts[4].Attr<UV>() << 0.0f, 0.0f;

    verts[5].Attr<POS>() << 1.0f, 0.0f, 1.0f;
    verts[5].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
    verts[5].Attr<UV>() << 0.0f, 0.0f;

    verts[6].Attr<POS>() << 1.0f, 1.0f, 1.0f;
    verts[6].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
    verts[6].Attr<UV>() << 0.0f, 0.0f;

    verts[7].Attr<POS>() << 0.0f, 1.0f, 1.0f;
    verts[7].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
    verts[7].Attr<UV>() << 0.0f, 0.0f;
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Setup index buffer data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    static const uint16_t indices[] = {
      0, 1, 2,
      0, 2, 3,
      //TODO,
      4, 5, 6,
      4, 6, 7
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    float aspect = static_cast<float>(window->dim().width) / window->dim().height;
    glm::vec3 eye(0, 0, 20);
    glm::vec3 viewDir(0, 0, -1);

    glm::vec3 up(0, 1, 0);

    glm::mat4 model;
    glm::mat4 view = glm::lookAt(eye, glm::vec3(0, 0, 0), up);
    glm::mat4 proj = glm::perspective(90.0_deg, aspect, 0.1f, 1000.0f);
    glm::mat4 mvp = proj * view * model;

    while (!glfwWindowShouldClose(*window)) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(175.0f / 255.0f, 175.0f / 255.0f, 175.0f / 255.0f, 1.0f);

      glUseProgram(*program);
      glUniform4fv(glGetUniformLocation(*program, "mvp"), 1, &mvp[0][0]);

      glBindVertexArray(vao);
      glDrawElements(GL_TRIANGLES, std::extent<decltype(*indices)>::value / 3, GL_UNSIGNED_SHORT, 0);

      glfwSwapBuffers(*window);
      glfwPollEvents();
    }
  }
};

int main(void) {
  Application app;
  app.Run();
  return 0;
}
