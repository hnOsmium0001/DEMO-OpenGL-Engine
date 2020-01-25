#include <iostream>
#include <Engine.hpp>
#include <Shader.hpp>
#include <GLAttribute.hpp>

void keyCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t modss) {
}

void charCallback(GLFWwindow* window, uint32_t codepoint) {
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
}

void cursorButtonCallback(GLFWwindow* window, int32_t button, int32_t action, int32_t mods) {
}

void scrollCallback(GLFWwindow* window, double x, double y) {
}

class Application : public HOEngine::ApplicationBase {
public:
	void Run() {
		auto window = HOEngine::Window::New({ 1024, 768 }, "Test window", {
			.resizeCallback = {},
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

		// Allocation
		GLuint vao;
		glGenVertexArrays(1, &vao);
		HOEngine::ScopeGuard vaoRelease([&]() { glDeleteVertexArrays(1, &vao); });

		GLuint vbo;
		glGenBuffers(1, &vbo);
		HOEngine::ScopeGuard vboRelease([&]() { glDeleteBuffers(1, &vbo); });

		auto vshSource = HOEngine::ReadFileAsStr("example/resources/triangle.vsh").value_or("");
		auto fshSource = HOEngine::ReadFileAsStr("example/resources/triangle.fsh").value_or("");
		auto programOpt = HOEngine::ShaderProgram::New(vshSource, fshSource);
		if (!programOpt) {
			std::cerr << "Unable to create shader program, aborting\n";
			return;
		}
		auto program = std::move(*programOpt);

		// Initialization
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		HOEngine::AttributeLayout<float[3], float[3]>::SetupGL();
		glBindVertexArray(0);

		// Data
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		static GLfloat vertices[] = {
			0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,		0.0f, 0.0f, 1.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		while (!glfwWindowShouldClose(*window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(175.0f / 255.0f, 175.0f / 255.0f, 175.0f / 255.0f, 1.0f);

			glUseProgram(program);

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 1 * 3);
			glBindVertexArray(0);

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
