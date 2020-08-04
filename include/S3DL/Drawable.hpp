#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Drawable
    {
        public:

            VkCommandBuffer getVulkanCommandBuffer(const Device& device, const RenderPipeline& pipeline, VkFramebuffer framebuffer, VkExtent2D extent) const;

            ~Drawable();

        private:

            mutable bool _commandCreated;
            mutable VkCommandBuffer _commandBuffer;
    };
}
