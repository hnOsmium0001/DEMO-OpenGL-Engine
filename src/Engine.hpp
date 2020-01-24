#pragma once

#include <string>
#include <functional>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Aliases.hpp"
#include "Util.hpp"

namespace HOEngine {

struct WindowCallbacks {
	std::optional<void(*)(GLFWwindow*, i32, i32, i32, i32)> keyCallback;
	std::optional<void(*)(GLFWwindow*, u32)> charCallback;
	std::optional<void(*)(GLFWwindow*, f64, f64)> cursorPosCallback;
	std::optional<void(*)(GLFWwindow*, i32, i32, i32)> cursorButtonCallback;
	std::optional<void(*)(GLFWwindow*, f64, f64)> scrollCallback;
};

class Window {
private:
	Dimension dim_;
	GLFWwindow* window_;

public:
	Window() = default;
	Window(const Window&) = delete;
	Window(Window&& source) noexcept;
	~Window() noexcept;

	void Init(Dimension dim, const std::string& title, WindowCallbacks callbacks);

	Dimension& dim() { return dim_; }
	const Dimension& dim() const { return dim_; }
	operator GLFWwindow*() const { return window_; }
};

class ApplicationBase {
public:
	ApplicationBase();
	~ApplicationBase() noexcept;
};

} // namespace HOEngine
