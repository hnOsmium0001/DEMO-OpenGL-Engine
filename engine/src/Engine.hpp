#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Util.hpp"

namespace HOEngine {

struct WindowCallbacks {
	std::optional<void(*)(GLFWwindow*, int32_t, int32_t)> resizeCallback;
	std::optional<void(*)(GLFWwindow*, int32_t, int32_t, int32_t, int32_t)> keyCallback;
	std::optional<void(*)(GLFWwindow*, uint32_t)> charCallback;
	std::optional<void(*)(GLFWwindow*, double, double)> cursorPosCallback;
	std::optional<void(*)(GLFWwindow*, int32_t, int32_t, int32_t)> cursorButtonCallback;
	std::optional<void(*)(GLFWwindow*, double, double)> scrollCallback;
};

class Window {
private:
	Dimension dim_;
	GLFWwindow* handle_;

	Window(Dimension, GLFWwindow*) noexcept;

public:
	inline static Window* FromGLFW(GLFWwindow* handle);
	static std::unique_ptr<Window> New(const Dimension& dim, const std::string& title, const WindowCallbacks& callbacks);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&& source) noexcept;
	Window& operator=(Window&& source) noexcept;
	~Window() noexcept;

	void Resize(int32_t width, int32_t height);

	Dimension& dim() { return dim_; }
	const Dimension& dim() const { return dim_; }
	operator GLFWwindow*() const { return handle_; }

	// Update window size fields of the attached window
	static void HandleResize(GLFWwindow* handle, int32_t width, int32_t height);
};

class ApplicationBase {
public:
	ApplicationBase();
	~ApplicationBase() noexcept;
};

} // namespace HOEngine
