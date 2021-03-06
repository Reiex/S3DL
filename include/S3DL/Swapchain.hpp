#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Swapchain
    {
        public:

            Swapchain(const RenderWindow& window);
            Swapchain(const RenderTexture& texture);

            VkSurfaceFormatKHR getFormat() const;
            VkExtent2D getExtent() const;

            uint32_t getCurrentImage() const;
            uint32_t getImageCount() const;

            VkCommandBuffer getCurrentCommandBuffer() const;

            void waitIdle() const;
            void updateDisplay(const RenderTarget& target) const;

            ~Swapchain();

        private:

            void create(const RenderTarget& target);

            void startRecordingCommandBuffer(unsigned int index) const;
            void stopRecordingCommandBuffer(unsigned int index) const;
            void recreateCommandBuffer(unsigned int index) const;

            void submitCommandBuffer(unsigned int index) const;
            void presentSurface(const RenderTarget& target, unsigned int index) const;
            unsigned int getNextImage(const RenderTarget& target) const;

            VkSwapchainKHR _swapChain;
            VkSurfaceFormatKHR _format;
            VkExtent2D _extent;

            uint32_t _imageCount;
            std::vector<VkImage> _images;
            std::vector<VkImageView> _imageViews;

            mutable VkFence _acquireFence;
            mutable std::vector<VkFence> _renderFences;
            mutable std::vector<VkSemaphore> _renderSemaphores;

            mutable std::vector<VkCommandBuffer> _commandBuffers;
            mutable unsigned int _currentImage;

        friend Framebuffer;
    };
}
