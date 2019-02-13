//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson), Min Shih                                //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//
// ======================================================================== //
// Copyright SCI Institute, University of Utah, 2018
// ======================================================================== //

#include "QuarkGLHeader.h"
#include "GLFWHeader_Internal.h"

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int v3d::quark::QuarkExecute(int argc, char* argv[], const std::function<void()>& render)
{
    // Initialize GLFW
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    // Provide Window Hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create Window
    GLFWwindow *window = nullptr;
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    window = glfwCreateWindow(v3d::quark::winW, v3d::quark::winH, "", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    // Ready
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
    check_error_gl("Ready");
    // Setup OpenGL
    glEnable(GL_DEPTH_TEST);
    check_error_gl("Setup OpenGL Options");
    // Do the work
    render();
    // Terminate
    glfwDestroyWindow(window);
    glfwTerminate();
}
