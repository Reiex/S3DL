#pragma once

#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class RenderSubpass
    {
        public:

            RenderSubpass(const std::vector<VkAttachmentReference>& input, const std::vector<VkAttachmentReference>& color, const std::vector<uint32_t>& preserve);
            RenderSubpass(const std::vector<VkAttachmentReference>& input, const std::vector<VkAttachmentReference>& color, const std::vector<uint32_t>& preserve, const VkAttachmentReference& depth);

            const std::vector<VkAttachmentReference>& getVulkanInputReferences() const;
            const std::vector<VkAttachmentReference>& getVulkanColorReferences() const;
            const std::vector<uint32_t>& getVulkanPreserveReferences() const;
            const VkAttachmentReference* getVulkanDepthReference() const;

        private:

            std::vector<VkAttachmentReference> _inputAttachments;
            std::vector<VkAttachmentReference> _colorAttachments;
            std::vector<uint32_t> _preserveAttachments;
            VkAttachmentReference _depthAttachment;
            bool _depthAttachmentSet;
    };
}
