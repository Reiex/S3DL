#include <S3DL/S3DL.hpp>

namespace s3dl
{
    std::set<Window*> Window::_WINDOW_LIST;

    Window::Window(unsigned int width, unsigned int height, const std::string& title) : Window()
    {
        if (width == 0 || height == 0)
            throw std::runtime_error("Cannot open window with size (0, 0).");

        _window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    }

    Window::~Window()
    {
        std::set<Window*>::iterator position = _WINDOW_LIST.find(this);
        if (position != _WINDOW_LIST.end())
            _WINDOW_LIST.erase(position);

        if (_WINDOW_LIST.size() == 0)
            glfwTerminate();
    }

    Window::Window()
    {
        if (_WINDOW_LIST.size() == 0)
        {
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }
        
        _WINDOW_LIST.insert(this);
    }
}