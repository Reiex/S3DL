#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class RenderTarget
    {
        public:

            RenderTarget() = default;

            const VkSurfaceKHR& getVulkanSurface() const;

        protected:

            VkSurfaceKHR _surface;
    };
}