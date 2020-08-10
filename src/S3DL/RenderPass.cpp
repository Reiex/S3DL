#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderPass::RenderPass() :
        _renderPassComputed(false),
        _renderPass(VK_NULL_HANDLE)
    {
    }

    void RenderPass::addSubpass(const RenderSubpass& subpass)
    {
        _subpasses.push_back(subpass);
        _renderPassComputed = false;
    }

    void RenderPass::addAttachment(const VkAttachmentDescription& attachment)
    {
        _attachments.push_back(attachment);
        _renderPassComputed = false;
    }

    void RenderPass::addDependency(unsigned int srcSubpass, unsigned int srcStage, unsigned int srcAccess, unsigned int dstSubpass, unsigned int dstStage, unsigned int dstAccess)
    {
        VkSubpassDependency dependency{};
        dependency.srcSubpass = srcSubpass;
        dependency.srcStageMask = srcStage;
        dependency.srcAccessMask = srcAccess;
        dependency.dstSubpass = dstSubpass;
        dependency.dstStageMask = dstStage;
        dependency.dstAccessMask = dstAccess;

        _dependencies.push_back(dependency);
        _renderPassComputed = false;
    }

    VkRenderPass RenderPass::getVulkanRenderPass(const Device& device) const
    {
        if (!_renderPassComputed)
        {
            destroy(device);

            std::vector<VkSubpassDescription> subpasses(_subpasses.size());
            for (int i(0); i < subpasses.size(); i++)
            {
                VkSubpassDescription subpass{};
                subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subpass.inputAttachmentCount = _subpasses[i].getVulkanInputReferences().size();
                subpass.pInputAttachments = _subpasses[i].getVulkanInputReferences().data();
                subpass.colorAttachmentCount = _subpasses[i].getVulkanColorReferences().size();
                subpass.pColorAttachments = _subpasses[i].getVulkanColorReferences().data();
                subpass.pResolveAttachments = nullptr;
                subpass.pDepthStencilAttachment = &_subpasses[i].getVulkanDepthReference();
                subpass.preserveAttachmentCount = _subpasses[i].getVulkanPreserveReferences().size();
                subpass.pPreserveAttachments = _subpasses[i].getVulkanPreserveReferences().data();

                subpasses[i] = subpass;
            }

            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.pNext = nullptr;
            renderPassInfo.attachmentCount = _attachments.size();
            renderPassInfo.pAttachments = _attachments.data();
            renderPassInfo.subpassCount = _subpasses.size();
            renderPassInfo.pSubpasses = subpasses.data();
            renderPassInfo.dependencyCount = _dependencies.size();
            renderPassInfo.pDependencies = _dependencies.data();

            VkResult result = vkCreateRenderPass(device.getVulkanDevice(), &renderPassInfo, nullptr, &_renderPass);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create render pass. VkResult: " + std::to_string(result));
            
            #ifndef NDEBUG
            std::clog << "VkRenderPass successfully created." << std::endl;
            #endif

            _renderPassComputed = true;
        }

        return _renderPass;
    }

    void RenderPass::destroy(const Device& device) const
    {
        if (_renderPass != VK_NULL_HANDLE)
            vkDestroyRenderPass(device.getVulkanDevice(), _renderPass, nullptr);
        
        _renderPass = VK_NULL_HANDLE;
        _renderPassComputed = false;
    }
}
