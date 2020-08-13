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

            ~Swapchain();

        private:

            void create();

            VkSwapchainKHR _swapChain;
            VkSurfaceFormatKHR _format;
            VkExtent2D _extent;

            std::vector<VkImage> _images;
            std::vector<VkImageView> _imageViews;

            std::vector<VkFence> _acquireFence;
            std::vector<VkSemaphore> _renderSemaphore;

        friend RenderTarget;
        friend Attachment;
    };
}
