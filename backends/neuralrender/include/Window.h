#pragma once

struct GLFWwindow;

class Window {
public:
    Window(int width, int height, const char* title);
    ~Window();

    bool shouldClose() const;
    void pollEvents();
    void display(const float* pixels, int width, int height);

private:
    void createDisplayResources();

    GLFWwindow* window_ = nullptr;

    unsigned int texture_ = 0;
    unsigned int vao_ = 0;
    unsigned int shaderProgram_ = 0;
};
