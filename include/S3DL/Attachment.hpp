#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Attachment
    {
        public:

            Attachment(VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initialLayout, VkImageLayout finalLayout);

        private:

            VkFormat _format;
            VkAttachmentLoadOp _loadOp;
            VkAttachmentStoreOp _storeOp;
            VkImageLayout _initialLayout;
            VkImageLayout _finalLayout;

            bool _targetAttachment;
            const RenderTarget* _target;
    };
}
