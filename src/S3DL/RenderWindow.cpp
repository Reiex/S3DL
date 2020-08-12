#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderWindow::RenderWindow(const uvec2& size, const std::string& title) :
        Window(size, title),
        RenderTarget(true)
    {
        VkResult result = glfwCreateWindowSurface(Instance::Active->getVulkanInstance(), _window, nullptr, &_surface);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create render window surface. VkResult: " + std::to_string(result));

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Render window surface successfully created." << std::endl;
        #endif
    }

    RenderWindow::~RenderWindow()
    {
        vkDestroySurfaceKHR(Instance::Active->getVulkanInstance(), _surface, nullptr);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Render window surface successfully destroyed." << std::endl;
        #endif
    }
}
