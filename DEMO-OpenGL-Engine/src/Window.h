#pragma once

#include <string>
#include <functional>
#include <optional>
#include <GLFW/glfw3.h>
#include "Aliases.h"
#include "Util.h"

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
	Window(const Window& source) = delete;
	Window(Window&& source) noexcept;
	~Window() noexcept;

	void Init(Dimension dim, const std::string& title, WindowCallbacks callbacks);
	bool ShouldClose() const;

	Dimension& dim() { return dim_; }
	const Dimension& dim() const { return dim_; }
};

} // namespace HOEngine
