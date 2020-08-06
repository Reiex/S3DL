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

    void RenderWindow::bindPipeline(RenderPipeline& pipeline)
    {
        pipeline.setViewportState({0, 0, _extent.width, _extent.height, 0, 1}, {0, 0, _extent.width, _extent.height});
        RenderTarget::bindPipeline(pipeline);

        if (_framebuffers.size() != 0)
            destroyFramebuffers();

        _currentImage = getNextRenderImage(_imageAvailableSemaphore[_currentFrame]);
        createFramebuffers();
        createCommandBuffer(_commandBuffers[_currentFrame]);
        startRecordingCommandBuffer(_commandBuffers[_currentFrame]);
    }

    void RenderWindow::unbindPipeline()
    {
        for (int i(0); i < _commandBuffers.size(); i++)
            destroyCommandBuffer(_commandBuffers[i]);
        
        destroyFramebuffers();

        RenderTarget::unbindPipeline();
    }

    void RenderWindow::setClearColor(vec4 color)
    {
        _clearValues[0] = {color.x, color.y, color.z, color.w};
    }

    void RenderWindow::destroy()
    {
        RenderTarget::destroy();

        if (_surface != VK_NULL_HANDLE)
            vkDestroySurfaceKHR(Instance::getVulkanInstance(), _surface, nullptr);
        _surface = VK_NULL_HANDLE;
    }

    RenderWindow::~RenderWindow()
    {
        destroy();
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
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
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

            VkExtent2D actualExtent = { width, height };

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
        _format = surfaceFormat.format;
        VkPresentModeKHR presentMode = chooseSwapPresentMode();
        _extent = chooseSwapExtent();

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
        createInfo.imageExtent = _extent;
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
            createInfo.format = _format;
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

    unsigned int RenderWindow::getNextRenderImage(VkSemaphore& imageAvailableSemaphore)
    {
        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(_device->getVulkanDevice(), _swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        while (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreate();
            result = vkAcquireNextImageKHR(_device->getVulkanDevice(), _swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        }

        if (result == VK_SUBOPTIMAL_KHR)
            _needsResize = true;

        return imageIndex;
    }

    void RenderWindow::presentRenderImage(VkSemaphore& imageRenderedSemaphore, unsigned int imageIndex)
    {
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &imageRenderedSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &_swapChain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        VkResult result = vkQueuePresentKHR(_device->getVulkanQueues().presentQueue, &presentInfo);
        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR || _needsResize)
        {
            _needsResize = false;
            recreate();
        }
    }

    void RenderWindow::destroyRenderImages()
    {
        for (int i(0); i < _imageViews.size(); i++)
            vkDestroyImageView(_device->getVulkanDevice(), _imageViews[i], nullptr);

        vkDestroySwapchainKHR(_device->getVulkanDevice(), _swapChain, nullptr);
    }

    void RenderWindow::initClearColors()
    {
        _clearValues.resize(1, {0.f, 0.f, 0.f, 1.f});
    }

    void RenderWindow::createFramebuffers()
    {
        _framebuffers.resize(_imageViews.size());

        for (size_t i = 0; i < _framebuffers.size(); i++)
        {
            VkImageView attachments[] = { _imageViews[i] };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = _pipeline->getVulkanRenderPass(*_device);
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = _extent.width;
            framebufferInfo.height = _extent.height;
            framebufferInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(_device->getVulkanDevice(), &framebufferInfo, nullptr, &_framebuffers[i]);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create framebuffer. VkResult: " + std::to_string(result));
        }

        #ifndef NDEBUG
        std::clog << "VkFramebuffer successfully created for all the window images." << std::endl;
        #endif
    }

    void RenderWindow::destroyFramebuffers()
    {
        for (size_t i = 0; i < _framebuffers.size(); i++)
            vkDestroyFramebuffer(_device->getVulkanDevice(), _framebuffers[i], nullptr);
        
        _framebuffers.resize(0);
    }
}
