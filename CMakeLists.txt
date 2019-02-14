cmake_minimum_required(VERSION 3.9)
project(DXServer)

set(V3D_OPENGL_LOAD_LIBRARY GLAD)
set(V3D_BUILD_TYPE core)
set(V3D_CXX_STANDARD 11)

add_subdirectory(external/libvidi3d)

find_package(Qt5 REQUIRED
  Core
  OpenGL
  Gui
  Widgets
  WebSockets)
include(external/libvidi3d/cmake/ConfigureOpenGL.cmake)

add_subdirectory(src)
