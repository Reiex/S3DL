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

    void RenderWindow::createSwapChain()
    {
        createSwapChain(chooseSwapExtent(), chooseSwapPresentMode(), chooseSwapSurfaceFormat());
    }

    void RenderWindow::createSwapChain(VkExtent2D extent, VkPresentModeKHR presentMode, VkSurfaceFormatKHR surfaceFormat)
    {
        _targetSize = {extent.width, extent.height};
        _swapFormat = surfaceFormat;

        // Compute swap chain settings
        PhysicalDevice::SwapChainSupportDetails swapChainSupport = Device::Active->getPhysicalDevice().swapSupport;

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            imageCount = swapChainSupport.capabilities.maxImageCount;
        
        // Create the swap chain

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = _surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(Device::Active->getVulkanDevice(), &createInfo, nullptr, &_swapChain);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create swap chain. VkResult: " + std::to_string(result));

        // Extract the swap chain images

        vkGetSwapchainImagesKHR(Device::Active->getVulkanDevice(), _swapChain, &imageCount, nullptr);
        _swapImages.resize(imageCount);
        vkGetSwapchainImagesKHR(Device::Active->getVulkanDevice(), _swapChain, &imageCount, _swapImages.data());

        // Compute swap chain image views
        
        _swapImageViews.resize(_swapImages.size());
        for (size_t i = 0; i < _swapImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};

            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = _swapImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = surfaceFormat.format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(Device::Active->getVulkanDevice(), &createInfo, nullptr, &_swapImageViews[i]);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create swap chain image view " + std::to_string(i) + ".");
        }

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkSwapChainKHR successfully created with " + std::to_string(imageCount) << " swap images." << std::endl;
        #endif
    }

    RenderWindow::~RenderWindow()
    {
        vkDestroySurfaceKHR(Instance::Active->getVulkanInstance(), _surface, nullptr);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Render window surface successfully destroyed." << std::endl;
        #endif
    }

    VkSurfaceFormatKHR RenderWindow::chooseSwapSurfaceFormat()
    {
        for (int i(0); i < Device::Active->getPhysicalDevice().swapSupport.formats.size(); i++)
        {
            VkSurfaceFormatKHR availableFormat = Device::Active->getPhysicalDevice().swapSupport.formats[i];
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
        }

        return Device::Active->getPhysicalDevice().swapSupport.formats[0];
    }

    VkPresentModeKHR RenderWindow::chooseSwapPresentMode()
    {
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    VkExtent2D RenderWindow::chooseSwapExtent()
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

            VkExtent2D actualExtent = { width, height };

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }
}
