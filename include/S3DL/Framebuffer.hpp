#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Framebuffer
    {
        public:

            Framebuffer(const RenderTarget& target, const RenderPass& renderPass);
            Framebuffer(const Framebuffer& framebuffer) = delete;

            Framebuffer& operator=(const Framebuffer& framebuffer) = delete;

            std::vector<VkFramebuffer> getVulkanFramebuffers() const;

            ~Framebuffer();

        private:

            std::vector<VkFramebufferCreateInfo> _framebuffers;
            std::vector<VkFramebuffer> _vulkanFramebuffers;
            std::vector<Texture> _attachments;
    };
}
