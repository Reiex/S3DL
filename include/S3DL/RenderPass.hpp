#pragma once

#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class RenderPass
    {
        public:

            RenderPass(const std::vector<const Attachment*>& attachments, const std::vector<Subpass>& subpasses, const std::vector<Dependency>& dependencies);
            RenderPass(const RenderPass& renderPass) = delete;

            RenderPass& operator=(const RenderPass& renderPass) = delete;

            Pipeline* getNewPipeline(unsigned int subpass, const Shader& shader, const RenderTarget& target);

            VkRenderPass getVulkanRenderPass() const;

            ~RenderPass();

        private:

            void destroyVulkanRenderPass() const;
        
            std::vector<VkAttachmentDescription> _attachments;
            std::vector<bool> _swapchainAttachments;

            std::vector<VkSubpassDescription> _subpasses;
            std::vector<std::vector<VkAttachmentReference>> _inputReferences;
            std::vector<std::vector<VkAttachmentReference>> _colorReferences;
            std::vector<std::vector<uint32_t>> _preserveReferences;
            std::vector<VkAttachmentReference> _depthStencilReferences;

            std::vector<VkSubpassDependency> _dependencies;

            VkRenderPassCreateInfo _renderPass;

            mutable bool _vulkanRenderPassComputed;
            mutable VkRenderPass _vulkanRenderPass;

            std::vector<Pipeline*> _pipelines;
        
        friend Pipeline;
        friend Framebuffer;
    };
}
