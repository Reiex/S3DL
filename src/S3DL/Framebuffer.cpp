#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Framebuffer::Framebuffer(const Swapchain& swapchain, const RenderPass& renderPass, const RenderTarget& target, VkImageUsageFlags additionalAttachmentsUsages)
    {
        _swapchain = &swapchain;

        _size = { swapchain._extent.width, swapchain._extent.height };
        _framebuffers.resize(swapchain._imageCount);
        _vulkanFramebuffers.resize(swapchain._imageCount);
        _attachments.resize(renderPass._attachments.size());
        _attachmentsBelonging.resize(renderPass._attachments.size());
        _vulkanAttachments.resize(swapchain._imageCount);
        for (int i(0); i < swapchain._imageCount; i++)
            _vulkanAttachments[i].resize(renderPass._attachments.size());

        // Create texture for each attachment (except swapchain where image views are fetched from it)

        for (int i(0); i < renderPass._attachments.size(); i++)
        {
            if (renderPass._swapchainAttachments[i])
            {
                _attachments[i] = nullptr;
                _attachmentsBelonging[i] = false;

                for (int j(0); j < swapchain._imageCount; j++)
                    _vulkanAttachments[j][i] = swapchain._imageViews[j];
            }
            else
            {
                VkFormat format(renderPass._attachments[i].format);
                VkImageTiling tiling(VK_IMAGE_TILING_OPTIMAL);
                VkImageUsageFlags usage(additionalAttachmentsUsages);
                VkImageAspectFlags imageAspects(0);

                for (int j(0); j < renderPass._inputReferences.size(); j++)
                {
                    for (int k(0); k < renderPass._inputReferences[j].size(); k++)
                    {
                        if (renderPass._inputReferences[j][k].attachment == i)
                        {
                            usage = usage | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
                        }
                    }
                }

                for (int j(0); j < renderPass._colorReferences.size(); j++)
                {
                    for (int k(0); k < renderPass._colorReferences[j].size(); k++)
                    {
                        if (renderPass._colorReferences[j][k].attachment == i)
                        {
                            usage = usage | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                            imageAspects = imageAspects | VK_IMAGE_ASPECT_COLOR_BIT;
                        }
                    }
                }

                for (int j(0); j < renderPass._subpasses.size(); j++)
                {
                    if (renderPass._subpasses[j].pDepthStencilAttachment != nullptr)
                    {
                        if (renderPass._subpasses[j].pDepthStencilAttachment->attachment == i)
                        {
                            usage = usage | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                            imageAspects = imageAspects | VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
                        }
                    }
                }

                _attachments[i] = new Texture(_size, format, tiling, usage, imageAspects);
                _attachmentsBelonging[i] = true;
                for (int j(0); j < swapchain._imageCount; j++)
                    _vulkanAttachments[j][i] = _attachments[i]->getVulkanImageView();
            }
        }

        // Create framebuffer

        for (int i(0); i < swapchain._imageCount; i++)
        {
            _framebuffers[i].sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            _framebuffers[i].pNext = nullptr;
            _framebuffers[i].flags = 0;
            _framebuffers[i].renderPass = renderPass.getVulkanRenderPass();
            _framebuffers[i].attachmentCount = _vulkanAttachments[i].size();
            _framebuffers[i].pAttachments = _vulkanAttachments[i].data();
            _framebuffers[i].width = _size.x;
            _framebuffers[i].height = _size.y;
            _framebuffers[i].layers = 1;

            VkResult result = vkCreateFramebuffer(Device::Active->getVulkanDevice(), &_framebuffers[i], nullptr, &_vulkanFramebuffers[i]);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create framebuffer. VkResult: " + std::to_string(result));
        }

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> " << _vulkanFramebuffers.size() << " VkFramebuffers successfully created." << std::endl;
        #endif
    }
    
    Texture* Framebuffer::getTexture(uint32_t textureIndex)
    {
        return _attachments[textureIndex];
    }

    const std::vector<VkImageView>& Framebuffer::getCurrentImageViews() const
    {
        return _vulkanAttachments[_swapchain->_currentImage];
    }

    VkFramebuffer Framebuffer::getCurrentFramebuffer() const
    {
        return _vulkanFramebuffers[_swapchain->_currentImage];
    }

    Framebuffer::~Framebuffer()
    {
        for (int i(0); i < _vulkanFramebuffers.size(); i++)
            vkDestroyFramebuffer(Device::Active->getVulkanDevice(), _vulkanFramebuffers[i], nullptr);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> " << _vulkanFramebuffers.size() << " VkFramebuffers successfully destroyed." << std::endl;
        #endif

        for (int i(0); i < _attachments.size(); i++)
            if (_attachmentsBelonging[i])
                delete _attachments[i];
    }
}
