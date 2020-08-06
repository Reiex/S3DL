#pragma once

#include <vector>
#include <string>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class RenderPass
    {
        public:

            RenderPass();

            void addSubpass(const RenderSubpass& subpass);
            void addAttachment(const VkAttachmentDescription& attachment);
            void addDependency(unsigned int srcSubpass, unsigned int srcStage, unsigned int srcAccess, unsigned int dstSubpass, unsigned int dstStage, unsigned int dstAccess);

            VkRenderPass getVulkanRenderPass(const Device& device) const;

            void destroy(const Device& device) const;

        private:

            std::vector<RenderSubpass> _subpasses;
            std::vector<VkAttachmentDescription> _attachments;
            std::vector<VkSubpassDependency> _dependencies;

            mutable bool _renderPassComputed;
            mutable VkRenderPass _renderPass;
    };
}
