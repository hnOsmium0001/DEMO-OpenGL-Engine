cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(OpenGLEngine)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(
  lib/gl3w/include
)

add_library(gl3w
  lib/gl3w/gl3w.c
)

add_library(opengl_engine
  engine/src/Engine.hpp
  engine/src/Engine.cpp
  engine/src/Shader.hpp
  engine/src/Shader.cpp
  engine/src/Util.hpp
  engine/src/Util.cpp
)
target_link_libraries(opengl_engine
  gl3w
  ${CONAN_LIBS}
)

include_directories(engine/src)
add_executable(opengl_example
  example/src/Main.cpp
)
target_link_libraries(opengl_example
  # Includes link targets in `opengl_engine`
  opengl_engine
)
