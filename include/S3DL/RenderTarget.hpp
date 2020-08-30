#pragma once

#include <vector>

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

            void beginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer, const std::vector<VkClearValue>& clearValues);
            void bindPipeline(const Pipeline* pipeline);
            void draw(const Drawable& drawable);
            void beginNextSubpass();
            void display();

            const uvec2& getTargetSize() const;
            bool hasVulkanSurface() const;

            VkSurfaceKHR getVulkanSurface() const;

        protected:

            void endRenderPass();

            uvec2 _targetSize;

            const Swapchain* _swapchain;

            bool _hasVulkanSurface;
            VkSurfaceKHR _surface;

            const RenderPass* _currentRenderPass;
    };
}