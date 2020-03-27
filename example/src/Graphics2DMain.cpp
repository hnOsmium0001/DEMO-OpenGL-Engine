#include <iostream>
#include <chrono>
#include <cmath>
#include <imgui.h>
#include <../res/bindings/imgui_impl_glfw.h>
#include <../res/bindings/imgui_impl_opengl3.h>
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
			.resizeCallback = ResizeCallback,
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
 
		auto programOpt = bind2(
				HOEngine::ReadFileAsStr("example/resources/triangle.vert"),
				HOEngine::ReadFileAsStr("example/resources/triangle.frag"),
				HOEngine::ShaderProgram::FromSource);
		if (!programOpt) {
			std::cerr << "Unable to create shader program, aborting\n";
			return;
		}
		GLuint program = programOpt.value();
 
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
 
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
 
		// ImGui::StyleColorsClassic();
		// ImGui::StyleColorsLight();
		ImGui::StyleColorsDark();
 
		ImGui_ImplGlfw_InitForOpenGL(*window, true);
		ImGui_ImplOpenGL3_Init(nullptr);
 
		// Data for ImGui application rendering
		bool showDemoWindow = false;
		ImVec2 fboDim(128, 96);
		// Data for triangle rendering
		float time = 0;
 
		GLuint tex;
		glGenTextures(1, &tex);
		HOEngine::ScopeGuard texRelease([&]() { glDeleteTextures(1, &tex); });
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window->width(), window->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
		GLuint fbo;
		glGenFramebuffers(1, &fbo);
		HOEngine::ScopeGuard fboRelease([&]() { glDeleteFramebuffers(1, &fbo); });
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
 
		while (!glfwWindowShouldClose(*window)) {
			glfwPollEvents();
 
			// Render our triangle
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(175.0f / 255.0f, 175.0f / 255.0f, 175.0f / 255.0f, 1.0f);
 
				glUseProgram(program);
				glUniform1f(glGetUniformLocation(program, "coefR"), static_cast<float>(std::sin(time)) * 0.5f + 0.5f);
				glUniform1f(glGetUniformLocation(program, "coefG"), static_cast<float>(std::sin(time + 3.1415926535f / 2)) * 0.5f + 0.5f);
				glUniform1f(glGetUniformLocation(program, "coefB"), static_cast<float>(std::sin(time + 3.1415926535f)) * 0.5f + 0.5f);
 
				glBindVertexArray(vao);
				glDrawArrays(GL_TRIANGLES, 0, 1 * 3);
				glBindVertexArray(0);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
 
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(175.0f / 255.0f, 175.0f / 255.0f, 175.0f / 255.0f, 1.0f);
 
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
 
			if (showDemoWindow) {
				ImGui::ShowDemoWindow();
			}
 
			ImGui::Begin("ImGui Demo Window");
			ImGui::Text("Hello, world!");
			ImGui::Checkbox("Demo window", &showDemoWindow);
			ImGui::SliderFloat("Width", &fboDim.x, 16.0f, 1024.0f);
			ImGui::SliderFloat("Height", &fboDim.y, 12.0f, 768.0f);
			ImGui::End();
 
			ImGui::Begin("Rendering framebuffer");
			ImGui::Image((void*) tex, fboDim);
			ImGui::End();
 
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
 
			glfwSwapBuffers(*window);
			time += 0.01f;
		}
	}
};
 
int main(void) {
	Application app;
	app.Run();
	return 0;
}

#include <../res/bindings/imgui_impl_glfw.cpp>
#include <../res/bindings/imgui_impl_opengl3.cpp>
