#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Subpass::Subpass(const std::vector<const Attachment*>& inputAttachments, const std::vector<const Attachment*>& colorAttachments, const std::vector<const Attachment*>& preserveAttachments, const Attachment* depthAttachment) :
        _inputAttachments(inputAttachments),
        _colorAttachments(colorAttachments),
        _preserveAttachments(preserveAttachments),
        _depthAttachment(depthAttachment)
    {
    }
}
