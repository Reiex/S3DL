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

        protected:

            virtual void createRenderImages();
            virtual void destroyRenderImages();

            std::vector<VkImage> _images;
            std::vector<VkImageView> _imageViews;
            VkExtent2D _extent;

            const Device* _device;

            bool _hasSurface;
            VkSurfaceKHR _surface;
    };
}