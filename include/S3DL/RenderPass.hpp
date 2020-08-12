#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class RenderPass
    {
        public:

            RenderPass(const std::vector<Attachment>& attachments, const std::vector<Subpass>& subpasses, const std::vector<Dependency>& dependencies);

        private:
        
            std::vector<VkAttachmentDescription> _vulkanAttachments;
            std::vector<VkSubpassDescription> _vulkanSubpasses;
            std::vector<std::vector<VkAttachmentReference>> _vulkanInputReferences;
            std::vector<std::vector<VkAttachmentReference>> _vulkanColorReferences;
            std::vector<std::vector<VkAttachmentReference>> _vulkanPreserveReferences;
            std::vector<VkSubpassDependency> _vulkanDependencies;

            std::vector<Pipeline> _pipelines;
    };
}
