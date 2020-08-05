#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class RenderTarget
    {
        public:

            RenderTarget(bool hasSurface);

            bool hasVulkanSurface() const;
            VkSurfaceKHR getVulkanSurface() const;

            void setDevice(const Device& device);
            void bindPipeline(RenderPipeline& pipeline);

            void draw(const Drawable& drawable);
            void display();

        protected:

            virtual void createRenderImages() = 0;
            virtual unsigned int getNextRenderImage(VkSemaphore& imageAvailableSemaphore) const = 0;
            virtual void presentRenderImage(VkSemaphore& imageRenderedSemaphore, unsigned int imageIndex) = 0;
            virtual void destroyRenderImages() = 0;

            virtual void initClearColors() = 0;

            virtual void createFramebuffers() = 0;
            virtual void destroyFramebuffers() = 0;

            void createCommandBuffer(VkCommandBuffer& commandBuffer);
            void startRecordingCommandBuffer(VkCommandBuffer& commandBuffer);
            void stopRecordingCommandBuffer(VkCommandBuffer& commandBuffer);
            void destroyCommandBuffer(VkCommandBuffer& commandBuffer);

            void createSyncTools();
            void destroySyncTools();

            std::vector<VkImage> _images;
            std::vector<VkImageView> _imageViews;
            VkExtent2D _extent;
            VkFormat _format;

            std::vector<VkFramebuffer> _framebuffers;

            const Device* _device;
            RenderPipeline* _pipeline;

            bool _hasSurface;
            VkSurfaceKHR _surface;

            unsigned int _concurrentFrames;
            unsigned int _currentImage;
            unsigned int _currentFrame;
            std::vector<VkCommandBuffer> _commandBuffers;
            std::vector<VkClearValue> _clearValues;
            std::vector<VkSemaphore> _imageAvailableSemaphore;
            std::vector<VkSemaphore> _imageRenderedSemaphore;
            std::vector<VkFence> _imageRenderedFence;
            std::vector<VkFence> _frameRenderedFence;
    };
}