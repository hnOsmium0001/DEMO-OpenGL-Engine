#pragma once

#include <string>
#include <functional>
#include <GLFW/glfw3.h>
#include "Aliases.h"
#include "Math.h"

namespace HOEngine {

	class Window {
	private:
		HOEngine::Dimension dim;
		GLFWwindow* window;

		void(*keyCallback)(GLFWwindow*, i32, i32, i32, i32);
		void(*charCallback)(GLFWwindow*, u32);
		void(*cursorPosCallback)(GLFWwindow*, f64, f64);
		void(*cursorButtonCallback)(GLFWwindow*, i32, i32, i32);
		void(*scrollCallback)(GLFWwindow*, f64, f64);

	public:
		Window()
			: dim{} {
		}

		void Init(HOEngine::Dimension dim, std::string& title);

		HOEngine::Dimension& dim() { return dim; }
		const HOEngine::Dimension& dim() const { return dim; }
	};

}
