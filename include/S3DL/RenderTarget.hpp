#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class RenderTarget
    {
        public:

            RenderTarget(bool hasSurface);
            RenderTarget(const RenderTarget& target) = delete;

            RenderTarget& operator=(const RenderTarget& target) = delete;

            bool hasVulkanSurface() const;
            VkSurfaceKHR getVulkanSurface() const;

        protected:

            bool _hasSurface;
            VkSurfaceKHR _surface;
    };
}