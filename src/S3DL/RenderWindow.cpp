#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderWindow::RenderWindow(unsigned int width, unsigned int height, const std::string& title) :
        Window(width, height, title),
        RenderTarget(true)
    {
        VkResult result = glfwCreateWindowSurface(Instance::getVulkanInstance(), _window, nullptr, &_surface);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create render window surface. VkResult: " + std::to_string(result));

        #ifndef NDEBUG
        std::clog << "Render window surface successfully created." << std::endl;
        #endif
    }

    RenderWindow::~RenderWindow()
    {
        destroyRenderImages();
        vkDestroySurfaceKHR(Instance::getVulkanInstance(), _surface, nullptr);
    }

    VkSurfaceFormatKHR RenderWindow::chooseSwapSurfaceFormat()
    {
        for (int i(0); i < _device->getPhysicalDeviceProperties().swapSupport.formats.size(); i++)
        {
            VkSurfaceFormatKHR availableFormat = _device->getPhysicalDeviceProperties().swapSupport.formats[i];
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
        }

        return _device->getPhysicalDeviceProperties().swapSupport.formats[0];
    }

    VkPresentModeKHR RenderWindow::chooseSwapPresentMode()
    {
        for (int i(0); i < _device->getPhysicalDeviceProperties().swapSupport.presentModes.size(); i++)
        {
            VkPresentModeKHR availablePresentMode = _device->getPhysicalDeviceProperties().swapSupport.presentModes[i];
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                return availablePresentMode;
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D RenderWindow::chooseSwapExtent()
    {
        VkSurfaceCapabilitiesKHR capabilities = _device->getPhysicalDeviceProperties().swapSupport.capabilities;
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(_window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    void RenderWindow::createRenderImages()
    {
        // Compute swap chain settings
        SwapChainSupportDetails swapChainSupport = _device->getPhysicalDeviceProperties().swapSupport;
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat();
        _swapChainImageFormat = surfaceFormat.format;
        VkPresentModeKHR presentMode = chooseSwapPresentMode();
        VkExtent2D _swapChainImageextent = chooseSwapExtent();

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
        createInfo.imageExtent = _swapChainImageextent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(_device->getVulkanDevice(), &createInfo, nullptr, &_swapChain);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create swap chain. VkResult: " + std::to_string(result));

        // Extract the swap chain images

        vkGetSwapchainImagesKHR(_device->getVulkanDevice(), _swapChain, &imageCount, nullptr);
        _images.resize(imageCount);
        vkGetSwapchainImagesKHR(_device->getVulkanDevice(), _swapChain, &imageCount, _images.data());

        // Compute swap chain image views
        
        _imageViews.resize(_images.size());
        for (size_t i = 0; i < _images.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};

            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = _images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = _swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(_device->getVulkanDevice(), &createInfo, nullptr, &_imageViews[i]);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create swap chain image view " + std::to_string(i) + ".");
        }

        #ifndef NDEBUG
        std::clog << "VkSwapChainKHR successfully created with " + std::to_string(imageCount) << " swap images." << std::endl;
        #endif
    }

    void RenderWindow::destroyRenderImages()
    {
        vkDestroySwapchainKHR(_device->getVulkanDevice(), _swapChain, nullptr);
    }
}
