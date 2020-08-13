#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Subpass
    {
        public:

            Subpass(const std::vector<const Attachment*>& inputAttachments, const std::vector<const Attachment*>& colorAttachments, const std::vector<const Attachment*>& preserveAttachments, const Attachment* depthAttachment = nullptr);

        private:

            std::vector<const Attachment*> _inputAttachments;
            std::vector<const Attachment*> _colorAttachments;
            std::vector<const Attachment*> _preserveAttachments;
            const Attachment* _depthAttachment;
        
        friend RenderPass;
    };
}
