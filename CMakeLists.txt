# ┌──────────────────────────────────────────────────────────────────┐
# │ Daxian Server                                                    │
# │  Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                     │
# │  University of California, Davis                                 │
# │  MIT Licensed                                                    │
# └──────────────────────────────────────────────────────────────────┘

# ┌──────────────────────────────────────────────────────────────────┐
# │  Project Specification                                           │
# └──────────────────────────────────────────────────────────────────┘
cmake_minimum_required(VERSION 3.9)
project(DXServer)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR})

# ┌──────────────────────────────────────────────────────────────────┐
# │  Configure LibViDi3d                                             │
# └──────────────────────────────────────────────────────────────────┘
set(V3D_OPENGL_LOAD_LIBRARY GLAD)
set(V3D_BUILD_TYPE core)
set(V3D_CXX_STANDARD 11)
add_subdirectory(external/libvidi3d)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Configure DX Server                                             │
# └──────────────────────────────────────────────────────────────────┘
find_package(Qt5 REQUIRED
  Core OpenGL Gui
  Widgets WebSockets)
if(NOT TARGET Threads::Threads)
  find_package(Threads)
endif()
include(external/libvidi3d/cmake/ConfigureOpenGL.cmake)
add_subdirectory(src)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Others                                                          │
# └──────────────────────────────────────────────────────────────────┘
file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/data/" DX_DATA_NATIVE_PATH)
configure_file(${PROJECT_SOURCE_DIR}/data/database.json.in
  database.json @ONLY)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Build Documentation                                             │
# └──────────────────────────────────────────────────────────────────┘
find_package(Doxygen)
find_package(Perl)
if(DOXYGEN_FOUND AND PERL_FOUND)
  doxygen_add_docs(doc ${PROJECT_SOURCE_DIR}/src COMMENT "Generate man pages")
else()
  message("Doxygen is not found")
endif()
