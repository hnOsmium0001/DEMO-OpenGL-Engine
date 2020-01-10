#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include "Math.h"

namespace HOEngine {

	class Window {
	private:
		HOEngine::Dimension dim;
		GLFWwindow* window;

	public:
		Window()
			: dim{} {
		}

		HOEngine::Dimension& dim() { return this->dim; }
		const HOEngine::Dimension& dim() const { return this->dim; }

	private:
		void Init(HOEngine::Dimension dim, std::string& title);
	};

}
