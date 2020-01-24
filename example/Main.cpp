#include <iostream>
#include "../src/Engine.hpp"

void keyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 modss) {
}

void charCallback(GLFWwindow* window, u32 codepoint) {
}

void cursorPosCallback(GLFWwindow* window, f64 x, f64 y) {
}

void cursorButtonCallback(GLFWwindow* window, i32 button, i32 action, i32 mods) {
}

void scrollCallback(GLFWwindow* window, f64 x, f64 y) {
}

class Application : public HOEngine::ApplicationBase {
public:
	void Run() {
		HOEngine::Window window = {};
		HOEngine::WindowCallbacks callbacks = {
			std::optional(keyCallback),
			std::optional(charCallback),
			std::optional(cursorPosCallback),
			std::optional(cursorButtonCallback),
			std::optional(scrollCallback)
		};
		window.Init({ 1024, 768 }, "Test window", callbacks);

		if (gl3wInit()) {
			throw std::runtime_error("Unable to initialize OpenGL");
		}

		// Allocation
		GLuint vao;
		glGenVertexArrays(1, &vao);
		HOEngine::ScopeGuard vaoRelease([&]() { glDeleteVertexArrays(1, &vao); });

		GLuint vbo;
		glGenBuffers(1, &vbo);
		HOEngine::ScopeGuard vboRelease([&]() { glDeleteBuffers(1, &vbo); });

		// Initialization
		glBindVertexArray(vao);
		{
			constexpr GLuint stride = sizeof(float) * 3 + sizeof(float) * 3;
			// Position attribute
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);

			// Color attribute
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
		}
		glBindVertexArray(0);

		// Data
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		static f32 vertices[] = {
			0.0f, 1.0f, 0.0f,		1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,		1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f,		1.0f, 1.0f, 1.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		while (!glfwWindowShouldClose(window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(175.0f / 255.0f, 175.0f / 255.0f, 175.0f / 255.0f, 1.0f);

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 1 * 3);
			glBindVertexArray(0);

			glfwSwapBuffers(window);

			glfwPollEvents();
		}
	}
};

int main(void) {
	Application app;
	app.Run();
	return 0;
}
