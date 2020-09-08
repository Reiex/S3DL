#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Attachment Attachment::ScreenAttachment(const Swapchain& swapchain, VkAttachmentLoadOp loadOp)
    {
        Attachment attachment(swapchain.getFormat().format, loadOp, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        attachment._swapchain = &swapchain;

        return attachment;
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
