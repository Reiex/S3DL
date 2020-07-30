#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderWindow::RenderWindow(unsigned int width, unsigned int height, const std::string& title) :
        Window(width, height, title),
        RenderTarget()
    {
        // VkResult result = glfwCreateWindowSurface(Device::_VK_INSTANCE, _window, nullptr, &_surface);
        // if (result != VK_SUCCESS)
        //     throw std::runtime_error("Failed to create render window surface. Error code: " + std::to_string(result));
    }

    RenderWindow::~RenderWindow()
    {

    }
}
