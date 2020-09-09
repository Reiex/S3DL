#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Framebuffer
    {
        public:

            Framebuffer(const Swapchain& swapchain, const RenderPass& renderPass, const RenderTarget& target, VkImageUsageFlags additionalAttachmentsUsages = 0);
            Framebuffer(const Framebuffer& framebuffer) = delete;

            Framebuffer& operator=(const Framebuffer& framebuffer) = delete;

            Texture* getTexture(uint32_t textureIndex);
            const std::vector<VkImageView>& getCurrentImageViews() const;
            VkFramebuffer getCurrentFramebuffer() const;

            ~Framebuffer();

        private:

            const Swapchain* _swapchain;

            uvec2 _size;
            std::vector<VkFramebufferCreateInfo> _framebuffers;
            std::vector<VkFramebuffer> _vulkanFramebuffers;
            std::vector<Texture*> _attachments;
            std::vector<bool> _attachmentsBelonging;
            std::vector<std::vector<VkImageView>> _vulkanAttachments;
    };
}
