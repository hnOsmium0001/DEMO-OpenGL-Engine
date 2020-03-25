cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(OpenGLEngine)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(opengl_engine
	engine/src/Engine.hpp
	engine/src/Engine.cpp
	engine/src/Entity.hpp
	engine/src/Entity.cpp
	engine/src/GLWrapper.hpp
	engine/src/GLWrapper.cpp
)
target_link_libraries(opengl_engine ${CONAN_LIBS})

# Examples should be able to #include engine headers
include_directories(engine/src)

add_executable(opengl_example
	example/src/Graphics2DMain.cpp
)
target_link_libraries(opengl_example
	# Includes link targets in `opengl_engine`
	opengl_engine
)

add_executable(opengl_3d_example
	example/src/Graphics3DMain.cpp
)
target_link_libraries(opengl_3d_example
	# Includes link targets in `opengl_engine`
	opengl_engine
)
