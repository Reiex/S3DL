#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Drawable
    {
        public:

        protected:

            virtual void draw(const Device& device, VkCommandBuffer& commandBuffer) const = 0;

        friend RenderTarget;
    };
}
