#include <iostream>
#include <chrono>
#include <cmath>
#include <Engine.hpp>
#include <Shader.hpp>
#include <GLAttribute.hpp>

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

class Application : public HOEngine::ApplicationBase {
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

		// Allocation
		GLuint vao;
		glGenVertexArrays(1, &vao);
		HOEngine::ScopeGuard vaoRelease([&]() { glDeleteVertexArrays(1, &vao); });

		GLuint vbo;
		glGenBuffers(1, &vbo);
		HOEngine::ScopeGuard vboRelease([&]() { glDeleteBuffers(1, &vbo); });

		auto vshSource = HOEngine::ReadFileAsStr("example/resources/triangle.vsh").value_or("");
		auto fshSource = HOEngine::ReadFileAsStr("example/resources/triangle.fsh").value_or("");
		auto program = HOEngine::ShaderProgram::New(vshSource, fshSource);
		if (!program) {
			std::cerr << "Unable to create shader program, aborting\n";
			return;
		}

		// Initialization
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		HOEngine::VertexAttributes<float[3], float[3]>::SetupPointers();
		glBindVertexArray(0);

		// Data
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		HOEngine::VertexAttributes<float[3], float[3]> verts[3];
		verts[0].Attr<0>() << 0.0f, 1.0f, 0.0f;
		verts[0].Attr<1>() << 1.0f, 0.0f, 0.0f;
		verts[1].Attr<0>() << -1.0f, -1.0f, 0.0f;
		verts[1].Attr<1>() << 0.0f, 1.0f, 0.0f;
		verts[2].Attr<0>() << 1.0f, -1.0f, 0.0f;
		verts[2].Attr<1>() << 0.0f, 0.0f, 1.0f;
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		float time = 0;
		while (!glfwWindowShouldClose(*window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(175.0f / 255.0f, 175.0f / 255.0f, 175.0f / 255.0f, 1.0f);

			glUseProgram(*program);
			GLuint coefRUniform = glGetUniformLocation(*program, "coefR");
			GLuint coefGUniform = glGetUniformLocation(*program, "coefG");
			GLuint coefBUniform = glGetUniformLocation(*program, "coefB");
			glUniform1f(coefRUniform, static_cast<float>(std::sin(time)) * 0.5f + 0.5f);
			glUniform1f(coefGUniform, static_cast<float>(std::sin(time + 3.1415926535f / 2)) * 0.5f + 0.5f);
			glUniform1f(coefBUniform, static_cast<float>(std::sin(time + 3.1415926535f)) * 0.5f + 0.5f);

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 1 * 3);
			glBindVertexArray(0);

			glfwSwapBuffers(*window);
			glfwPollEvents();

			time += 0.01f;
		}
	}
};

int main(void) {
	Application app;
	app.Run();
	return 0;
}
