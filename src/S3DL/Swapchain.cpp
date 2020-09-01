#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Swapchain::Swapchain(const RenderWindow& window)
    {
        // Compute swap chain settings
        _extent = window.getBestSwapExtent();
        VkPresentModeKHR presentMode = window.getBestSwapPresentMode();
        _format = window.getBestSwapSurfaceFormat();

        PhysicalDevice::SwapChainSupportDetails swapChainSupport = Device::Active->getPhysicalDevice().swapSupport;

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            imageCount = swapChainSupport.capabilities.maxImageCount;
        
        // Create the swap chain

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = window.getVulkanSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = _format.format;
        createInfo.imageColorSpace = _format.colorSpace;
        createInfo.imageExtent = _extent;
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
        _images.resize(imageCount);
        vkGetSwapchainImagesKHR(Device::Active->getVulkanDevice(), _swapChain, &imageCount, _images.data());

        // Compute swap chain image views
        
        _imageViews.resize(_images.size());
        for (size_t i = 0; i < _images.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};

            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = _images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = _format.format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(Device::Active->getVulkanDevice(), &createInfo, nullptr, &_imageViews[i]);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create swap chain image view " + std::to_string(i) + ".");
        }

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkSwapChainKHR successfully created with " + std::to_string(imageCount) << " swap images." << std::endl;
        #endif

        create(window);
    }

    VkSurfaceFormatKHR Swapchain::getFormat() const
    {
        return _format;
    }

    VkExtent2D Swapchain::getExtent() const
    {
        return _extent;
    }

    uint32_t Swapchain::getImageCount() const
    {
        return _images.size();
    }

    VkCommandBuffer Swapchain::getCurrentCommandBuffer() const
    {
        return _commandBuffers[_currentImage];
    }

    void Swapchain::updateDisplay(const RenderTarget& target) const
    {
        stopRecordingCommandBuffer(_currentImage);

        vkWaitForFences(Device::Active->getVulkanDevice(), 1, &_acquireFence, VK_TRUE, UINT64_MAX);

        vkResetFences(Device::Active->getVulkanDevice(), 1, &_renderFences[_currentImage]);
        submitCommandBuffer(_currentImage);
        presentSurface(target, _currentImage);

        vkResetFences(Device::Active->getVulkanDevice(), 1, &_acquireFence);
        _currentImage = getNextImage(target);

        vkWaitForFences(Device::Active->getVulkanDevice(), 1, &_renderFences[_currentImage], VK_TRUE, UINT64_MAX);

        recreateCommandBuffer(_currentImage);
        startRecordingCommandBuffer(_currentImage);
    }

    void Swapchain::waitIdle() const
    {
        vkWaitForFences(Device::Active->getVulkanDevice(), 1, &_acquireFence, VK_TRUE, UINT64_MAX);
        for (int i(0); i < _renderFences.size(); i++)
            vkWaitForFences(Device::Active->getVulkanDevice(), 1, &_renderFences[i], VK_TRUE, UINT64_MAX);
    }

    Swapchain::~Swapchain()
    {
        vkDestroyFence(Device::Active->getVulkanDevice(), _acquireFence, nullptr);

        for (int i(0); i < _renderFences.size(); i++)
            vkDestroyFence(Device::Active->getVulkanDevice(), _renderFences[i], nullptr);

        for (int i(0); i < _renderSemaphores.size(); i++)
            vkDestroySemaphore(Device::Active->getVulkanDevice(), _renderSemaphores[i], nullptr);

        for (int i(0); i < _commandBuffers.size(); i++)
            vkFreeCommandBuffers(Device::Active->getVulkanDevice(), Device::Active->getVulkanCommandPool(), 1, &_commandBuffers[i]);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Swapchain update tools successfully destroyed." << std::endl;
        #endif

        for (int i(0); i < _imageViews.size(); i++)
            vkDestroyImageView(Device::Active->getVulkanDevice(), _imageViews[i], nullptr);

        vkDestroySwapchainKHR(Device::Active->getVulkanDevice(), _swapChain, nullptr);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkSwapChainKHR successfully destroyed." << std::endl;
        #endif
    }

    void Swapchain::create(const RenderTarget& target)
    {
        unsigned int imageCount = _images.size();

        _renderFences.resize(imageCount);
        _renderSemaphores.resize(imageCount);
        _commandBuffers.resize(imageCount);

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkCommandBufferAllocateInfo commandBufferInfo{};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = Device::Active->getVulkanCommandPool();
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = 1;

        vkCreateFence(Device::Active->getVulkanDevice(), &fenceInfo, nullptr, &_acquireFence);
        for (int i(0); i < imageCount; i++)
        {
            vkCreateFence(Device::Active->getVulkanDevice(), &fenceInfo, nullptr, &_renderFences[i]);
            vkCreateSemaphore(Device::Active->getVulkanDevice(), &semaphoreInfo, nullptr, &_renderSemaphores[i]);
            vkAllocateCommandBuffers(Device::Active->getVulkanDevice(), &commandBufferInfo, &_commandBuffers[i]);
        }

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Swapchain update tools successfully created." << std::endl;
        #endif

        vkResetFences(Device::Active->getVulkanDevice(), 1, &_acquireFence);
        _currentImage = getNextImage(target);
        startRecordingCommandBuffer(_currentImage);
    }

    void Swapchain::startRecordingCommandBuffer(unsigned int index) const
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        VkResult result = vkBeginCommandBuffer(_commandBuffers[index], &beginInfo);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to begin recording command buffer. VkResult: " + std::to_string(result));
    }

    void Swapchain::stopRecordingCommandBuffer(unsigned int index) const
    {
        VkResult result = vkEndCommandBuffer(_commandBuffers[index]);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to record command buffer. VkResult: " + std::to_string(result));
    }

    void Swapchain::recreateCommandBuffer(unsigned int index) const
    {
        vkFreeCommandBuffers(Device::Active->getVulkanDevice(), Device::Active->getVulkanCommandPool(), 1, &_commandBuffers[index]);

        VkCommandBufferAllocateInfo commandBufferInfo{};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = Device::Active->getVulkanCommandPool();
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = 1;

        vkAllocateCommandBuffers(Device::Active->getVulkanDevice(), &commandBufferInfo, &_commandBuffers[index]);
    }

    void Swapchain::submitCommandBuffer(unsigned int index) const
    {
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &_commandBuffers[index];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &_renderSemaphores[index];

        VkResult result = vkQueueSubmit(Device::Active->getVulkanGraphicsQueue(), 1, &submitInfo, _renderFences[index]);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to submit draw command buffer. VkResult: " + std::to_string(result));
    }

    void Swapchain::presentSurface(const RenderTarget& target, unsigned int index) const
    {
        if (target.hasVulkanSurface())
        {
            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &_renderSemaphores[index];
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = &_swapChain;
            presentInfo.pImageIndices = &_currentImage;
            presentInfo.pResults = nullptr;

            vkQueuePresentKHR(Device::Active->getVulkanPresentQueue(), &presentInfo);
        }
    }

    unsigned int Swapchain::getNextImage(const RenderTarget& target) const
    {
        if (target.hasVulkanSurface())
        {
            uint32_t index;
            vkAcquireNextImageKHR(Device::Active->getVulkanDevice(), _swapChain, UINT64_MAX, VK_NULL_HANDLE, _acquireFence, &index);
            return index;
        }

        return (_currentImage + 1) % _images.size();
    }
}
