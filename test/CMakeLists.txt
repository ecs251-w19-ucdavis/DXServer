# ┌──────────────────────────────────────────────────────────────────┐
# │  Configure Source Files                                          │
# └──────────────────────────────────────────────────────────────────┘
# Source files
set (vidi3d_test_SOURCES
  Main.cpp
)

# Header files
set (vidi3d_test_HEADERS )

# Create target
add_executable(vidi3d_test ${vidi3d_test_SOURCES} ${vidi3d_test_HEADERS})

# ┌──────────────────────────────────────────────────────────────────┐
# │  Configure Targets                                               │
# └──────────────────────────────────────────────────────────────────┘
target_v3d_configure(vidi3d_test
  LINK_PLAIN
  v3d::vidi3d_core
  v3d::vidi3d_module_opengl
  Threads::Threads)
target_link_v3d_opengl(vidi3d_test GL_CONTEXT EGL)