#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Window::Window(const uvec2& size, const std::string& title) :
        _windowSize(size)
    {
        if (size.x == 0 || size.y == 0)
            throw std::runtime_error("Cannot open window with size (0, 0).");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        _window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Window successfully opened." << std::endl;
        #endif
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(_window);
    }

    const uvec2& Window::getWindowSize() const
    {
        return _windowSize;
    }

    GLFWwindow* Window::getWindowHandle()
    {
        return _window;
    }

    Window::~Window()
    {
        glfwDestroyWindow(_window);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Window successfully closed." << std::endl;
        #endif
    }
}