#pragma once

#include <string>
#include <functional>
#include <optional>
#include <GLFW/glfw3.h>
#include "Aliases.h"
#include "Util.h"

namespace HOEngine {

class WindowCallbacks {
public:
	std::optional<FuncPtr<auto(GLFWwindow*, i32, i32, i32, i32) -> void>> keyCallback;
	std::optional<FuncPtr<auto(GLFWwindow*, u32) -> void>> charCallback;
	std::optional<FuncPtr<auto(GLFWwindow*, f64, f64) -> void>> cursorPosCallback;
	std::optional<FuncPtr<auto(GLFWwindow*, i32, i32, i32) -> void>> cursorButtonCallback;
	std::optional<FuncPtr<auto(GLFWwindow*, f64, f64) -> void>> scrollCallback;
};

class Window {
private:
	Dimension _dim;
	GLFWwindow* _window;

public:
	Window() = default;
	Window(const Window& source) = delete;
	Window(Window&& source);
	~Window();

	void Init(Dimension dim, const std::string& title, WindowCallbacks callbacks);
	bool ShouldClose() const;

	Dimension& dim() { return _dim; }
	const Dimension& dim() const { return _dim; }
};

} // namespace HOEngine
