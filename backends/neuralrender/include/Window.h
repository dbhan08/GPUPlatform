#pragma once

struct GLFWwindow;

class Window {
public:
    Window(int width, int height, const char* title);
    ~Window();

    bool shouldClose() const;
    void pollEvents();

private:
    GLFWwindow* window_ = nullptr;
};
