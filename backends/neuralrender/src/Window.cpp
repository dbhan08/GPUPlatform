#include "Window.h"

#include <cstdlib>
#include <iostream>

#include <glad/gl.h>
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

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "OpenGL error: failed to initialize GLAD" << std::endl;
        std::exit(1);
    }

    createDisplayResources();
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

void Window::display(const float* pixels, int width, int height) {
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA32F,
        width,
        height,
        0,
        GL_RGBA,
        GL_FLOAT,
        pixels
    );

    glUseProgram(shaderProgram_);
    glBindVertexArray(vao_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glUniform1i(glGetUniformLocation(shaderProgram_, "imageTexture"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window_);
}

void Window::createDisplayResources() {
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenVertexArrays(1, &vao_);

    const char* vertexShaderSource = R"(
        #version 460 core

        out vec2 uv;

        void main() {
            vec2 positions[3] = vec2[](
                vec2(-1.0, -1.0),
                vec2( 3.0, -1.0),
                vec2(-1.0,  3.0)
            );

            vec2 texCoords[3] = vec2[](
                vec2(0.0, 0.0),
                vec2(2.0, 0.0),
                vec2(0.0, 2.0)
            );

            gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
            uv = texCoords[gl_VertexID];
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 460 core

        in vec2 uv;
        out vec4 fragColor;

        uniform sampler2D imageTexture;

        void main() {
            fragColor = texture(imageTexture, uv);
        }
    )";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram_ = glCreateProgram();
    glAttachShader(shaderProgram_, vertexShader);
    glAttachShader(shaderProgram_, fragmentShader);
    glLinkProgram(shaderProgram_);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
