# ┌──────────────────────────────────────────────────────────────────┐
# │ Daxian Server                                                    │
# │  Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                     │
# │  University of California, Davis                                 │
# │  MIT Licensed                                                    │
# └──────────────────────────────────────────────────────────────────┘

# ┌──────────────────────────────────────────────────────────────────┐
# │  Configure Qt                                                    │
# └──────────────────────────────────────────────────────────────────┘
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Manage Qt resources
set(CMAKE_AUTORCC ON)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Configure Source Files                                          │
# └──────────────────────────────────────────────────────────────────┘
# Source files
set (vidi3d_dxserver_SOURCES

  Graphics/DXGL.cpp
  Graphics/Engine.cpp

  Loader/RegularGridRawBinary.cpp
  Loader/TetraGridRawBinary.cpp

  Util/Client.cpp
  Util/Resolve.cpp
  Util/Request.cpp

  Communicator.cpp
  GPUTaskHandler.cpp
  CPUTaskHandler.cpp
  RequestQueue.cpp

  Workspace.cpp
  Main.cpp
  )

# Header files
set (vidi3d_dxserver_HEADERS

  Graphics/DXGL.h
  Graphics/Engine.h

  Loader/Common.h
  Loader/RegularGridRawBinary.h
  Loader/TetraGridRawBinary.h

  Serializer/SceneDictionary.h

  Util/Client.h
  Util/Request.h
  Util/Resolve.h

  Communicator.h
  GPUTaskHandler.h
  CPUTaskHandler.h
  RequestQueue.h

  Workspace.h
  )

get_property(_egl_available GLOBAL PROPERTY V3D_USE_EGL)
set(_extra_libs )
set(_extra_defs )
if(_egl_available)
  list(APPEND vidi3d_dxserver_SOURCES Graphics/egl/EGLWindow.cpp)
else()
  list(APPEND vidi3d_dxserver_SOURCES Graphics/qt/QtOpenGLWindow.cpp)
  list(APPEND vidi3d_dxserver_HEADERS Graphics/qt/QtOpenGLWindow.h)
  list(APPEND _extra_libs Qt5::OpenGL Qt5::Widgets)
  list(APPEND _extra_defs USE_QT_OPENGL)
endif()

# Create target
add_executable(vidi3d_dxserver ${vidi3d_dxserver_SOURCES} ${vidi3d_dxserver_HEADERS})

# ┌──────────────────────────────────────────────────────────────────┐
# │  Configure Targets                                               │
# └──────────────────────────────────────────────────────────────────┘
target_v3d_configure(vidi3d_dxserver
  LINK_PLAIN
  v3d::vidi3d_core
  v3d::vidi3d_module_opengl
  Qt5::Core
  Qt5::Gui # needed by QImage
  Qt5::WebSockets
  Threads::Threads
  ${_extra_libs})
target_link_v3d_opengl(vidi3d_dxserver GL_CONTEXT EGL)
target_compile_definitions(vidi3d_dxserver PRIVATE ${_extra_defs})
