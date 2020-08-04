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

        protected:

            virtual void createRenderImages() = 0;
            virtual void destroyRenderImages() = 0;

            virtual void createFramebuffers() = 0;
            virtual void destroyFramebuffers() = 0;

            std::vector<VkImage> _images;
            std::vector<VkImageView> _imageViews;
            VkExtent2D _extent;
            VkFormat _format;

            std::vector<VkFramebuffer> _framebuffers;

            const Device* _device;
            RenderPipeline* _pipeline;

            bool _hasSurface;
            VkSurfaceKHR _surface;

            unsigned int _currentFrame;
    };
}