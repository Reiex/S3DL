#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class RenderTarget
    {
        public:

            RenderTarget(bool hasVulkanSurface);
            RenderTarget(const RenderTarget& target) = delete;

            RenderTarget& operator=(const RenderTarget& target) = delete;

            void setSwapchain(const Swapchain& swapchain);

            bool hasVulkanSurface() const;
            VkSurfaceKHR getVulkanSurface() const;

        protected:

            uvec2 _targetSize;
            const Swapchain* _swapchain;

            bool _hasVulkanSurface;
            VkSurfaceKHR _surface;
    };
}