#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Window::Window(unsigned int width, unsigned int height, const std::string& title)
    {
        if (width == 0 || height == 0)
            throw std::runtime_error("Cannot open window with size (0, 0).");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        _window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        #ifndef NDEBUG
        std::clog << "Window successfully opened." << std::endl;
        #endif
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(_window);
    }

    Window::~Window()
    {
        glfwDestroyWindow(_window);
    }
}