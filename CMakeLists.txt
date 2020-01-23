cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(OpenGLEngine)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
  E:/Library/GLFW3.3.1/include
  E:/Library/glm0.9.9.7
  lib/gl3w/include
)

add_executable(OpenGLEngine
  src/HOEngine.hpp
  src/Aliases.hpp
  src/Engine.hpp
  src/Engine.cpp
  src/Util.hpp
  src/Util.cpp

  example/Main.cpp

  lib/gl3w/gl3w.c
)
target_link_libraries(OpenGLEngine
  E:/Library/GLFW3.3.1/lib-mingw/libglfw3.a
)
