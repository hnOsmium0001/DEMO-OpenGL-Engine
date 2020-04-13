#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Engine.hpp"
#include "Entity.hpp"
#include "GLWrapper.hpp"
#include "Model.hpp"
#include "MonadicUtil.hpp"

namespace Ng = HOEngine;

void ResizeCallback(GLFWwindow* handle, int32_t width, int32_t height) {
	auto window = Ng::Window::FromGLFW(handle);
	if (window) {
		window->Resize(width, height);
		glViewport(0, 0, width, height);
	}
}
 
void KeyCallback(GLFWwindow* handle, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
}
 
void CharCallback(GLFWwindow* handle, uint32_t codepoint) {
}
 
void CursorPosCallback(GLFWwindow* handle, double x, double y) {
}
 
void CursorButtonCallback(GLFWwindow* handle, int32_t button, int32_t action, int32_t mods) {
}
 
void ScrollCallback(GLFWwindow* handle, double x, double y) {
}

class Application : public Ng::ApplicationBase {
public:
	void Run() {
		auto window = Ng::Window::New({ 1024, 768 }, "Test window", {
			.resizeCallback = ResizeCallback,
			.keyCallback = KeyCallback,
			.charCallback = CharCallback,
			.cursorPosCallback = CursorPosCallback,
			.cursorButtonCallback = CursorButtonCallback,
			.scrollCallback = ScrollCallback
		});
		if (window == nullptr) {
			std::cerr << "Unable to create GLFW window, aborting\n";
			return;
		}
 
		if (gl3wInit()) {
			std::cerr << "Unable to initialize OpenGL, aborting\n";
			return;
		}

		// ==================== //
		// Actual program below //
		// ==================== //

		auto cube = Ng::Entity::NewObject();
		auto camera = Ng::Entity::New();
		camera.AddComponent(std::make_unique<Ng::TransformComponent>());
		camera.AddComponent(std::make_unique<Ng::CameraComponent>());

		// Cube initialization
		auto& mesh = *cube.GetComponent<Ng::MeshComponent>();
		Ng::ReadOBJAt(mesh, "example/resources/cube.obj");

		// Camera initialization
		auto& camPos = *camera.GetComponent<Ng::TransformComponent>();
		auto& cam = *camera.GetComponent<Ng::CameraComponent>();
		cam.fov = 90.0_deg;
		cam.nearPane = 0.1f;
		cam.farPane = 1000.0f;
		cam.viewRay = glm::vec3{0, 0, 0};

		auto model = glm::mat4{};
		auto view = cam.ViewMat();
		auto perspective = cam.PerspectiveMat(window.get());
		auto mvp = perspective * view * model;

		// ======================================== //
		// Setup OpenGL objects for rendering below //
		// ======================================== //

		auto programOpt = Ng::Bind(
				Ng::ShaderProgram::FromSource,
				Ng::ReadFileAsStr("example/resources/cube3d.vert"),
				Ng::ReadFileAsStr("example/resources/cube3d.frag"));
		if (!programOpt) {
			std::cerr << "Unable to create shader program, aborting\n";
			return;
		}
		auto program = std::move(programOpt.value());
		auto vao = Ng::StateObject{};
		auto vbo = Ng::BufferObject{};
		auto ibo = Ng::BufferObject{};
 
		glBindVertexArray(vao);
		// Bind vbo attribute pointers for VAO
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		Ng::SimpleVertex::SetupPointers();
		// Bind index (element) buffer for VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, mesh.VerticesSize(), &mesh.vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.IndicesSize(), &mesh.indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Camera stuff
		float aspect = static_cast<float>(window->width() / window->height());

		while (!glfwWindowShouldClose(*window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(175.0f / 255.0f, 175.0f / 255.0f, 175.0f / 255.0f, 1.0f);

			glUseProgram(program);
			glUniform4fv(glGetUniformLocation(program, "mvp"), 1, &mvp[0][0]);

			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLE_STRIP, mesh.indices.size() / 3, GL_UNSIGNED_SHORT, 0);

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
