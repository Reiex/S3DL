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

            VkSurfaceFormatKHR getFormat() const;
            VkExtent2D getExtent() const;

            VkCommandBuffer getCurrentCommandBuffer() const;

            void waitIdle() const;
            void updateDisplay(const RenderTarget& target) const;

            ~Swapchain();

        private:

            void create(const RenderTarget& target);

            void startRecordingCommandBuffer(unsigned int buffer) const;
            void stopRecordingCommandBuffer(unsigned int buffer) const;
            void recreateCommandBuffer(unsigned int buffer) const;

            void submitCommandBuffer(unsigned int buffer) const;
            void presentSurface(const RenderTarget& target) const;
            unsigned int getNextImage(const RenderTarget& target) const;

            VkSwapchainKHR _swapChain;
            VkSurfaceFormatKHR _format;
            VkExtent2D _extent;

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
