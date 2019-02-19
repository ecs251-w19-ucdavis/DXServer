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
  Main.cpp
  SceneManager.cpp
  SceneLoader.cpp
  Communication/MainServer.cpp
  Loader/RegularGridRawBinary.cpp
  Loader/TetraGridRawBinary.cpp)

# Header files
set (vidi3d_dxserver_HEADERS
  SceneLoader.h
  Communication/MainServer.h
  Serializer/SceneDictionary.h
  Loader/Common.h
  Loader/RegularGridRawBinary.h
  Loader/TetraGridRawBinary.h)

get_property(_egl_available GLOBAL PROPERTY V3D_USE_EGL)
if(_egl_available)
  list(APPEND vidi3d_dxserver_SOURCES Graphics/egl/EGLWindow.cpp)
else()
  find_package(glfw3 QUIET)
  if(TARGET glfw)
    list(APPEND vidi3d_dxserver_SOURCES Graphics/glfw/GLFWSetup.cpp)
    list(APPEND vidi3d_dxserver_HEADERS Graphics/glfw/GLFWHeader_Internal.h)
  else()
    list(APPEND vidi3d_dxserver_SOURCES Graphics/qt/QtOpenGLWindow.cpp)
    list(APPEND vidi3d_dxserver_HEADERS Graphics/qt/QtOpenGLWindow.h)
  endif ()
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
  Qt5::Gui
  Qt5::Widgets
  Qt5::WebSockets)
target_link_v3d_opengl(vidi3d_dxserver GL_CONTEXT EGL)
if(TARGET glfw)
  target_link_libraries(vidi3d_dxserver glfw)
endif()
