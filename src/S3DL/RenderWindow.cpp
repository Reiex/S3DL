#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderWindow::RenderWindow(const Device& device, unsigned int width, unsigned int height, const std::string& title) :
        Window(width, height, title),
        RenderTarget()
    {
        VkResult result = glfwCreateWindowSurface(Device::getVulkanInstance(), _window, nullptr, &_surface);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create render window surface. VkResult: " + std::to_string(result));

        #ifndef NDEBUG
        std::clog << "Render window surface successfully created." << std::endl;
        #endif
    }

    RenderWindow::~RenderWindow()
    {
        vkDestroySurfaceKHR(Device::getVulkanInstance(), _surface, nullptr);
    }
}
