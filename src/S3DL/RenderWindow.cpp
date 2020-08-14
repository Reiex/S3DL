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

    VkExtent2D RenderWindow::getBestSwapExtent() const
    {
        VkSurfaceCapabilitiesKHR capabilities = Device::Active->getPhysicalDevice().swapSupport.capabilities;
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(_window, &width, &height);

            VkExtent2D actualExtent = { (uint32_t) width, (uint32_t) height };

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    VkPresentModeKHR RenderWindow::getBestSwapPresentMode() const
    {
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    VkSurfaceFormatKHR RenderWindow::getBestSwapSurfaceFormat() const
    {
        for (int i(0); i < Device::Active->getPhysicalDevice().swapSupport.formats.size(); i++)
        {
            VkSurfaceFormatKHR availableFormat = Device::Active->getPhysicalDevice().swapSupport.formats[i];
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
        }

        return Device::Active->getPhysicalDevice().swapSupport.formats[0];
    }

    RenderWindow::~RenderWindow()
    {
        vkDestroySurfaceKHR(Instance::Active->getVulkanInstance(), _surface, nullptr);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Render window surface successfully destroyed." << std::endl;
        #endif
    }
}
