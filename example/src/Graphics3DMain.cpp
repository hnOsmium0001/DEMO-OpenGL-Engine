#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Engine.hpp"
#include "MonadicUtil.hpp"
#include "GLWrapper.hpp"

void ResizeCallback(GLFWwindow* handle, int32_t width, int32_t height) {
	auto window = HOEngine::Window::FromGLFW(handle);
	if (window) {
		window->Resize(width, height);
		glViewport(0, 0, width, height);
	}
}
 
void KeyCallback(GLFWwindow* handle, int32_t key, int32_t scancode, int32_t action, int32_t modss) {
}
 
void CharCallback(GLFWwindow* handle, uint32_t codepoint) {
}
 
void CursorPosCallback(GLFWwindow* handle, double x, double y) {
}
 
void CursorButtonCallback(GLFWwindow* handle, int32_t button, int32_t action, int32_t mods) {
}
 
void ScrollCallback(GLFWwindow* handle, double x, double y) {
}

class Application : public HOEngine::ApplicationBase {
public:
	void Run() {
		auto window = HOEngine::Window::New({ 1024, 768 }, "Test window", {
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

		HOEngine::StateObject vao;
		HOEngine::BufferObject vbo, ibo;
 
		auto programOpt = bind2(
				HOEngine::ReadFileAsStr("example/resources/cube3d.vert"),
				HOEngine::ReadFileAsStr("example/resources/cube3d.frag"),
				HOEngine::ShaderProgram::FromSource);
		if (!programOpt) {
			std::cerr << "Unable to create shader program, aborting\n";
			return;
		}
		GLuint program = programOpt.value();

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
		verts[0].Attr<POS>() << -1.0f, -1.0f, 1.0f;
		verts[0].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
		verts[0].Attr<UV>() << 0.0f, 0.0f;

		verts[1].Attr<POS>() << 1.0f, -1.0f, 1.0f;
		verts[1].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
		verts[1].Attr<UV>() << 0.0f, 0.0f;

		verts[2].Attr<POS>() << -1.0f, 1.0f, 1.0f;
		verts[2].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
		verts[2].Attr<UV>() << 0.0f, 0.0f;

		verts[3].Attr<POS>() << 1.0f, 1.0f, 1.0f;
		verts[3].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
		verts[3].Attr<UV>() << 0.0f, 0.0f;
		
		verts[4].Attr<POS>() << -1.0f, -1.0f, -1.0f;
		verts[4].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
		verts[4].Attr<UV>() << 0.0f, 0.0f;

		verts[5].Attr<POS>() << 1.0f, -1.0f, -1.0f;
		verts[5].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
		verts[5].Attr<UV>() << 0.0f, 0.0f;

		verts[6].Attr<POS>() << -1.0f, 1.0f, -1.0f;
		verts[6].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
		verts[6].Attr<UV>() << 0.0f, 0.0f;

		verts[7].Attr<POS>() << 1.0f, 1.0f, -1.0f;
		verts[7].Attr<NORMAL>() << 0.0f, 0.0f, 0.0f;
		verts[7].Attr<UV>() << 0.0f, 0.0f;
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Setup index buffer data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		static const uint16_t indices[] = {
			0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		float aspect = static_cast<float>(window->width() / window->height());
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

			glUseProgram(program);
			glUniform4fv(glGetUniformLocation(program, "mvp"), 1, &mvp[0][0]);

			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLE_STRIP, std::extent<decltype(*indices)>::value / 3, GL_UNSIGNED_SHORT, 0);

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
