#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderSubpass::RenderSubpass(const std::vector<VkAttachmentReference>& input, const std::vector<VkAttachmentReference>& color, const std::vector<uint32_t>& preserve)
    {
        _inputAttachments = input;
        _colorAttachments = color;
        _preserveAttachments = preserve;
    }

    const std::vector<VkAttachmentReference>& RenderSubpass::getVulkanInputReferences() const
    {
        return _inputAttachments;
    }

    const std::vector<VkAttachmentReference>& RenderSubpass::getVulkanColorReferences() const
    {
        return _colorAttachments;
    }
    
    const std::vector<uint32_t>& RenderSubpass::getVulkanPreserveReferences() const
    {
        return _preserveAttachments;
    }
}