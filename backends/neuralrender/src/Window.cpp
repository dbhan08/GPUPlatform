#include "Window.h"

#include <cstdlib>
#include <iostream>

#include <GLFW/glfw3.h>


Window::Window(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "GLFW error: glfwInit failed" << std::endl;
        std::exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window_) {
        std::cerr << "GLFW error: glfwCreateWindow failed" << std::endl;
        glfwTerminate();
        std::exit(1);
    }

    glfwMakeContextCurrent(window_);
}

Window::~Window() {
    if (window_) {
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window_);
}

void Window::pollEvents() {
    glfwPollEvents();

    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
}
