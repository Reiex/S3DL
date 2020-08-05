#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderTarget::RenderTarget(bool hasSurface) :
        _concurrentFrames(2),
        _currentFrame(0),
        _currentImage(0),
        _hasSurface(hasSurface),
        _commandBuffer(VK_NULL_HANDLE),
        _imageAvailableSemaphore(VK_NULL_HANDLE),
        _imageRenderedSemaphore(VK_NULL_HANDLE)
    {
    }

    bool RenderTarget::hasVulkanSurface() const
    {
        return _hasSurface;
    }

    VkSurfaceKHR RenderTarget::getVulkanSurface() const
    {
        return _surface;
    }

    void RenderTarget::setDevice(const Device& device)
    {
        _device = &device;
        createRenderImages();
        createCommandBuffer();
        createSyncTools();
        initClearColors();
    }

    void RenderTarget::bindPipeline(RenderPipeline& pipeline)
    {
        _pipeline = &pipeline;
    }

    void RenderTarget::draw(const Drawable& drawable)
    {
        vkCmdDraw(_commandBuffer, 3, 1, 0, 0);
    }

    void RenderTarget::display()
    {
        // Submit command buffer to GPU

        stopRecordingCommandBuffer();
        
        const DeviceQueues& queues = _device->getVulkanQueues();

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &_imageAvailableSemaphore;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &_commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &_imageRenderedSemaphore;

        VkResult result = vkQueueSubmit(queues.graphicsQueue, 1, &submitInfo, nullptr);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to submit draw command buffer. VkResult: " + std::to_string(result));
        
        presentRenderImage(_imageRenderedSemaphore, _currentImage);

        vkQueueWaitIdle(_device->getVulkanQueues().presentQueue);
        vkQueueWaitIdle(_device->getVulkanQueues().graphicsQueue);

        // Recreate command buffer

        _currentImage = getNextRenderImage(_imageAvailableSemaphore);

        destroyCommandBuffer();
        createCommandBuffer();
        
        startRecordingCommandBuffer();
    }
    
    void RenderTarget::createCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = _device->getVulkanCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkResult result = vkAllocateCommandBuffers(_device->getVulkanDevice(), &allocInfo, &_commandBuffer);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate command buffers. VkResult: " + std::to_string(result));
    }

    void RenderTarget::startRecordingCommandBuffer()
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        VkResult result = vkBeginCommandBuffer(_commandBuffer, &beginInfo);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to begin recording command buffer. VkResult: " + std::to_string(result));
        
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = _pipeline->getVulkanRenderPass(*_device);
        renderPassInfo.framebuffer = _framebuffers[_currentImage];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = _extent;
        renderPassInfo.clearValueCount = _clearValues.size();
        renderPassInfo.pClearValues = _clearValues.data();

        vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->getVulkanPipeline(*_device));
    }

    void RenderTarget::stopRecordingCommandBuffer()
    {
        vkCmdEndRenderPass(_commandBuffer);

        VkResult result = vkEndCommandBuffer(_commandBuffer);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to record command buffer. VkResult: " + std::to_string(result));
    }

    void RenderTarget::destroyCommandBuffer()
    {
        if (_commandBuffer != VK_NULL_HANDLE)
            vkFreeCommandBuffers(_device->getVulkanDevice(), _device->getVulkanCommandPool(), 1, &_commandBuffer);
    }

    void RenderTarget::createSyncTools()
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        vkCreateSemaphore(_device->getVulkanDevice(), &semaphoreInfo, nullptr, &_imageAvailableSemaphore);
        vkCreateSemaphore(_device->getVulkanDevice(), &semaphoreInfo, nullptr, &_imageRenderedSemaphore);
    }

    void RenderTarget::destroySyncTools()
    {
        vkDestroySemaphore(_device->getVulkanDevice(), _imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(_device->getVulkanDevice(), _imageRenderedSemaphore, nullptr);
    }
}
