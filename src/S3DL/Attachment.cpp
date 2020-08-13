#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Attachment::Attachment(const Swapchain& swapchain, VkAttachmentLoadOp loadOp) :
        _format(swapchain._format.format),
        _loadOp(loadOp),
        _storeOp(VK_ATTACHMENT_STORE_OP_STORE),
        _initialLayout(VK_IMAGE_LAYOUT_UNDEFINED),
        _finalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
        _swapchain(&swapchain)
    {
    }

    Attachment::Attachment(VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initialLayout, VkImageLayout finalLayout) :
        _format(format),
        _loadOp(loadOp),
        _storeOp(storeOp),
        _initialLayout(initialLayout),
        _finalLayout(finalLayout),
        _swapchain(nullptr)
    {
    }
}
