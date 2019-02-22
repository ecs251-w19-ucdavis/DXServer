cmake_minimum_required(VERSION 3.9)
project(DXServer)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR})

set(V3D_OPENGL_LOAD_LIBRARY GLAD)
set(V3D_BUILD_TYPE core)
set(V3D_CXX_STANDARD 11)

add_subdirectory(external/libvidi3d)

find_package(Qt5 REQUIRED
  Core OpenGL Gui
  Widgets WebSockets)
if(NOT TARGET Threads::Threads)
  find_package(Threads)
endif()

include(external/libvidi3d/cmake/ConfigureOpenGL.cmake)

add_subdirectory(src)
