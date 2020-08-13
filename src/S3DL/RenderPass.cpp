#include <S3DL/S3DL.hpp>

namespace s3dl
{
    namespace
    {
        unsigned int getAttachmentIndex(const std::vector<Attachment>& attachments, const Attachment* attachment)
        {
            for (int i(0); i < attachments.size(); i++)
                if (&attachments[i] == attachment)
                    return i;
            
            throw std::runtime_error("Attachment used in subpass not found in render pass attachments.");
        }
    }

    RenderPass::RenderPass(const std::vector<Attachment>& attachments, const std::vector<Subpass>& subpasses, const std::vector<Dependency>& dependencies)
    {
        _vulkanAttachments.resize(attachments.size());

        _vulkanSubpasses.resize(subpasses.size());
        _vulkanInputReferences.resize(subpasses.size());
        _vulkanColorReferences.resize(subpasses.size());
        _vulkanPreserveReferences.resize(subpasses.size());
        _vulkanDepthReferences.resize(subpasses.size());

        _vulkanDependencies.resize(dependencies.size());

        // Render pass attachments

        for (int i(0); i < attachments.size(); i++)
        {
            VkAttachmentDescription attachment{};
            attachment.format = attachments[i]._format;
            attachment.samples = VK_SAMPLE_COUNT_1_BIT;
            attachment.loadOp = attachments[i]._loadOp;
            attachment.storeOp = attachments[i]._storeOp;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.initialLayout = attachments[i]._initialLayout;
            attachment.finalLayout = attachments[i]._finalLayout;

            _vulkanAttachments[i] = attachment;
        }

        for (int i(0); i < subpasses.size(); i++)
        {
            // Subpass input references

            _vulkanInputReferences[i].resize(subpasses[i]._inputAttachments.size());
            for (int j(0); j < subpasses[i]._inputAttachments.size(); j++)
            {
                VkAttachmentReference reference{};
                reference.attachment = getAttachmentIndex(attachments, subpasses[i]._inputAttachments[j]);
                reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                _vulkanInputReferences[i][j] = reference;
            }

            // Subpass output references

            _vulkanColorReferences[i].resize(subpasses[i]._colorAttachments.size());
            for (int j(0); j < subpasses[i]._colorAttachments.size(); j++)
            {
                VkAttachmentReference reference{};
                reference.attachment = getAttachmentIndex(attachments, subpasses[i]._colorAttachments[j]);
                reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                _vulkanColorReferences[i][j] = reference;
            }

            // Subpass preserve references
            
            _vulkanPreserveReferences[i].resize(subpasses[i]._preserveAttachments.size());
            for (int j(0); j < subpasses[i]._preserveAttachments.size(); j++)
                _vulkanPreserveReferences[i][j] = getAttachmentIndex(attachments, subpasses[i]._preserveAttachments[j]);

            // Subpass depth references

            if (subpasses[i]._depthAttachment != nullptr)
            {
                VkAttachmentReference reference{};
                reference.attachment = getAttachmentIndex(attachments, subpasses[i]._depthAttachment);
                reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                _vulkanDepthReferences[i] = reference;
            }

            // Subpass itself

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.inputAttachmentCount = _vulkanInputReferences[i].size();
            subpass.pInputAttachments = _vulkanInputReferences[i].data();
            subpass.colorAttachmentCount =_vulkanColorReferences[i].size();
            subpass.pColorAttachments = _vulkanColorReferences[i].data();
            subpass.pResolveAttachments = nullptr;

            if (subpasses[i]._depthAttachment != nullptr)
                subpass.pDepthStencilAttachment = &_vulkanDepthReferences[i];
            else
                subpass.pDepthStencilAttachment = nullptr;
            
            subpass.preserveAttachmentCount = _vulkanPreserveReferences[i].size();
            subpass.pPreserveAttachments = _vulkanPreserveReferences[i].data();
        }

        // Dependencies

        for (int i(0); i < _vulkanDependencies.size(); i++)
        {
            VkSubpassDependency dependency{};
            dependency.srcSubpass = dependencies[i]._srcSubpass;
            dependency.srcStageMask = dependencies[i]._srcStage;
            dependency.srcAccessMask = dependencies[i]._srcMask;
            dependency.dstSubpass = dependencies[i]._dstSubpass;
            dependency.dstStageMask = dependencies[i]._dstStage;
            dependency.dstAccessMask = dependencies[i]._dstMask;

            _vulkanDependencies[i] = dependency;
        }

        // Render pass itself

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pNext = nullptr;
        renderPassInfo.attachmentCount = _vulkanAttachments.size();
        renderPassInfo.pAttachments = _vulkanAttachments.data();
        renderPassInfo.subpassCount = _vulkanSubpasses.size();
        renderPassInfo.pSubpasses = _vulkanSubpasses.data();
        renderPassInfo.dependencyCount = _vulkanDependencies.size();
        renderPassInfo.pDependencies = _vulkanDependencies.data();
    }
}
