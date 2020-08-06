#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderTarget::RenderTarget(bool hasSurface) :
        _extent({0, 0}),
        _format(VK_FORMAT_UNDEFINED),

        _device(nullptr),
        _pipeline(nullptr),

        _hasSurface(hasSurface),
        _surface(VK_NULL_HANDLE),

        _concurrentFrames(2),
        _currentFrame(0),
        _currentImage(0),
        _commandBuffers(_concurrentFrames, VK_NULL_HANDLE),
        _imageAvailableSemaphore(_concurrentFrames, VK_NULL_HANDLE),
        _imageRenderedSemaphore(_concurrentFrames, VK_NULL_HANDLE),
        _frameRenderedFence(_concurrentFrames, VK_NULL_HANDLE),

        _needsResize(false)
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

    void RenderTarget::bindDevice(const Device& device)
    {
        unbindDevice();

        _device = &device;
        createRenderImages();
        createSyncTools();
        initClearColors();
    }

    void RenderTarget::unbindDevice()
    {
        if (_device != nullptr)
        {
            destroySyncTools();
            destroyRenderImages();
        }
        _device = nullptr;
    }

    void RenderTarget::bindPipeline(RenderPipeline& pipeline)
    {
        unbindPipeline();

        _pipeline = &pipeline;
    }

    void RenderTarget::unbindPipeline()
    {
        _pipeline = nullptr;
    }

    void RenderTarget::draw(const Drawable& drawable)
    {
        vkCmdDraw(_commandBuffers[_currentFrame], 3, 1, 0, 0);
    }

    void RenderTarget::display()
    {
        // Submit command buffer to GPU

        stopRecordingCommandBuffer(_commandBuffers[_currentFrame]);
        
        const DeviceQueues& queues = _device->getVulkanQueues();

        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &_imageAvailableSemaphore[_currentFrame];
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &_commandBuffers[_currentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &_imageRenderedSemaphore[_currentFrame];

        if (_imageRenderedFence[_currentImage] != VK_NULL_HANDLE)
            vkWaitForFences(_device->getVulkanDevice(), 1, &_imageRenderedFence[_currentImage], VK_TRUE, UINT64_MAX);
        _imageRenderedFence[_currentImage] = _frameRenderedFence[_currentFrame];

        vkResetFences(_device->getVulkanDevice(), 1, &_frameRenderedFence[_currentFrame]);
        VkResult result = vkQueueSubmit(queues.graphicsQueue, 1, &submitInfo, _frameRenderedFence[_currentFrame]);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to submit draw command buffer. VkResult: " + std::to_string(result));
        
        presentRenderImage(_imageRenderedSemaphore[_currentFrame], _currentImage);

        _currentFrame = (_currentFrame + 1) % _concurrentFrames;

        // Recreate command buffer
        
        vkWaitForFences(_device->getVulkanDevice(), 1, &_frameRenderedFence[_currentFrame], VK_TRUE, UINT64_MAX);
        _currentImage = getNextRenderImage(_imageAvailableSemaphore[_currentFrame]);

        destroyCommandBuffer(_commandBuffers[_currentFrame]);
        createCommandBuffer(_commandBuffers[_currentFrame]);
        
        startRecordingCommandBuffer(_commandBuffers[_currentFrame]);
    }
    
    void RenderTarget::destroy()
    {
        unbindPipeline();
        unbindDevice();
    }

    RenderTarget::~RenderTarget()
    {
        destroy();
    }

    void RenderTarget::createCommandBuffer(VkCommandBuffer& commandBuffer)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = _device->getVulkanCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkResult result = vkAllocateCommandBuffers(_device->getVulkanDevice(), &allocInfo, &commandBuffer);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate command buffers. VkResult: " + std::to_string(result));
    }

    void RenderTarget::startRecordingCommandBuffer(VkCommandBuffer& commandBuffer)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
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

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->getVulkanPipeline(*_device));
    }

    void RenderTarget::stopRecordingCommandBuffer(VkCommandBuffer& commandBuffer)
    {
        vkCmdEndRenderPass(commandBuffer);

        VkResult result = vkEndCommandBuffer(commandBuffer);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to record command buffer. VkResult: " + std::to_string(result));
    }

    void RenderTarget::destroyCommandBuffer(VkCommandBuffer& commandBuffer)
    {
        if (commandBuffer != VK_NULL_HANDLE)
            vkFreeCommandBuffers(_device->getVulkanDevice(), _device->getVulkanCommandPool(), 1, &commandBuffer);
        commandBuffer = VK_NULL_HANDLE;
    }

    void RenderTarget::createSyncTools()
    {
        _imageAvailableSemaphore.resize(_concurrentFrames);
        _imageRenderedSemaphore.resize(_concurrentFrames);
        _imageRenderedFence.resize(_images.size(), VK_NULL_HANDLE);
        _frameRenderedFence.resize(_concurrentFrames);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (int i(0); i < _concurrentFrames; i++)
        {
            vkCreateSemaphore(_device->getVulkanDevice(), &semaphoreInfo, nullptr, &_imageAvailableSemaphore[i]);
            vkCreateSemaphore(_device->getVulkanDevice(), &semaphoreInfo, nullptr, &_imageRenderedSemaphore[i]);
            vkCreateFence(_device->getVulkanDevice(), &fenceInfo, nullptr, &_frameRenderedFence[i]);
        }
    }

    void RenderTarget::destroySyncTools()
    {
        for (int i(0); i < _concurrentFrames; i++)
        {
            vkDestroySemaphore(_device->getVulkanDevice(), _imageAvailableSemaphore[i], nullptr);
            vkDestroySemaphore(_device->getVulkanDevice(), _imageRenderedSemaphore[i], nullptr);
            vkDestroyFence(_device->getVulkanDevice(), _frameRenderedFence[i], nullptr);
        }

        for (int i(0); i < _images.size(); i++)
            _imageRenderedFence[i] = VK_NULL_HANDLE;
    }

    void RenderTarget::recreate()
    {
        vkDeviceWaitIdle(_device->getVulkanDevice());

        RenderPipeline* pipeline(_pipeline);
        const Device* device(_device);

        unbindPipeline();
        unbindDevice();

        device->updateProperties(*this);

        bindDevice(*device);
        bindPipeline(*pipeline);
    }
}
