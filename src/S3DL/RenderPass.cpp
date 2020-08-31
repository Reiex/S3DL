#include <S3DL/S3DL.hpp>

namespace s3dl
{
    namespace
    {
        unsigned int getAttachmentIndex(const std::vector<const Attachment*>& attachments, const Attachment* attachment)
        {
            for (int i(0); i < attachments.size(); i++)
                if (attachments[i] == attachment)
                    return i;
            
            throw std::runtime_error("Attachment used in subpass not found in render pass attachments.");
        }
    }

    RenderPass::RenderPass(const std::vector<const Attachment*>& attachments, const std::vector<Subpass>& subpasses, const std::vector<Dependency>& dependencies)
    {
        _attachments.resize(attachments.size());
        _swapchainAttachments.resize(attachments.size());

        _subpasses.resize(subpasses.size());
        _inputReferences.resize(subpasses.size());
        _colorReferences.resize(subpasses.size());
        _preserveReferences.resize(subpasses.size());
        _depthStencilReferences.resize(subpasses.size());

        _dependencies.resize(dependencies.size());

        // Render pass attachments

        for (int i(0); i < attachments.size(); i++)
        {
            VkAttachmentDescription attachment{};
            attachment.format = attachments[i]->_format;
            attachment.samples = VK_SAMPLE_COUNT_1_BIT;
            attachment.loadOp = attachments[i]->_loadOp;
            attachment.storeOp = attachments[i]->_storeOp;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.initialLayout = attachments[i]->_initialLayout;
            attachment.finalLayout = attachments[i]->_finalLayout;

            _attachments[i] = attachment;

            _swapchainAttachments[i] = (attachments[i]->_swapchain != nullptr);
        }

        for (int i(0); i < subpasses.size(); i++)
        {
            // Subpass input references

            _inputReferences[i].resize(subpasses[i]._inputAttachments.size());
            for (int j(0); j < subpasses[i]._inputAttachments.size(); j++)
            {
                VkAttachmentReference reference{};
                reference.attachment = getAttachmentIndex(attachments, subpasses[i]._inputAttachments[j]);
                reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                _inputReferences[i][j] = reference;
            }

            // Subpass output references

            _colorReferences[i].resize(subpasses[i]._colorAttachments.size());
            for (int j(0); j < subpasses[i]._colorAttachments.size(); j++)
            {
                VkAttachmentReference reference{};
                reference.attachment = getAttachmentIndex(attachments, subpasses[i]._colorAttachments[j]);
                reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                _colorReferences[i][j] = reference;
            }

            // Subpass preserve references
            
            _preserveReferences[i].resize(subpasses[i]._preserveAttachments.size());
            for (int j(0); j < subpasses[i]._preserveAttachments.size(); j++)
                _preserveReferences[i][j] = getAttachmentIndex(attachments, subpasses[i]._preserveAttachments[j]);

            // Subpass depth references

            if (subpasses[i]._depthStencilAttachment != nullptr)
            {
                VkAttachmentReference reference{};
                reference.attachment = getAttachmentIndex(attachments, subpasses[i]._depthStencilAttachment);
                reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                _depthStencilReferences[i] = reference;
            }

            // Subpass itself

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

            subpass.inputAttachmentCount = _inputReferences[i].size();
            subpass.pInputAttachments = (_inputReferences[i].size() != 0) ? _inputReferences[i].data(): nullptr;
            subpass.colorAttachmentCount = _colorReferences[i].size();
            subpass.pColorAttachments = (_colorReferences[i].size() != 0) ? _colorReferences[i].data(): nullptr;
            subpass.pResolveAttachments = nullptr;
            subpass.pDepthStencilAttachment = (subpasses[i]._depthStencilAttachment != nullptr) ? &_depthStencilReferences[i]: nullptr;
            subpass.preserveAttachmentCount = _preserveReferences[i].size();
            subpass.pPreserveAttachments = (_preserveReferences[i].size() != 0) ? _preserveReferences[i].data(): nullptr;

            _subpasses[i] = subpass;
        }

        // Dependencies

        for (int i(0); i < _dependencies.size(); i++)
        {
            VkSubpassDependency dependency{};
            dependency.srcSubpass = dependencies[i]._srcSubpass;
            dependency.srcStageMask = dependencies[i]._srcStage;
            dependency.srcAccessMask = dependencies[i]._srcMask;
            dependency.dstSubpass = dependencies[i]._dstSubpass;
            dependency.dstStageMask = dependencies[i]._dstStage;
            dependency.dstAccessMask = dependencies[i]._dstMask;

            _dependencies[i] = dependency;
        }

        // Render pass itself

        _renderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        _renderPass.pNext = nullptr;
        _renderPass.flags = 0;
        _renderPass.attachmentCount = _attachments.size();
        _renderPass.pAttachments = _attachments.data();
        _renderPass.subpassCount = _subpasses.size();
        _renderPass.pSubpasses = _subpasses.data();
        _renderPass.dependencyCount = _dependencies.size();
        _renderPass.pDependencies = _dependencies.data();

        _vulkanRenderPassComputed = false;
        _vulkanRenderPass = VK_NULL_HANDLE;
    }

    Pipeline* RenderPass::getNewPipeline(unsigned int subpass, const Shader& shader, const RenderTarget& target)
    {
        _pipelines.push_back(new Pipeline(*this, subpass, shader, target));

        return _pipelines[_pipelines.size() - 1];
    }

    VkRenderPass RenderPass::getVulkanRenderPass() const
    {
        if (!_vulkanRenderPassComputed)
        {
            destroyVulkanRenderPass();

            VkResult result = vkCreateRenderPass(Device::Active->getVulkanDevice(), &_renderPass, nullptr, &_vulkanRenderPass);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create render pass. VkResult: " + std::to_string(result));
            
            #ifndef NDEBUG
            std::clog << "<S3DL Debug> VkRenderPass successfully created." << std::endl;
            #endif

            _vulkanRenderPassComputed = true;

            for (int i(0); i < _pipelines.size(); i++)
            {
                _pipelines[i]->_pipeline.renderPass = _vulkanRenderPass;
                _pipelines[i]->_vulkanPipelineComputed = false;
            }
        }

        return _vulkanRenderPass;
    }

    RenderPass::~RenderPass()
    {
        for (int i(0); i < _pipelines.size(); i++)
            delete _pipelines[i];

        destroyVulkanRenderPass();
    }

    void RenderPass::destroyVulkanRenderPass() const
    {
        if (_vulkanRenderPass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(Device::Active->getVulkanDevice(), _vulkanRenderPass, nullptr);

            #ifndef NDEBUG
            std::clog << "<S3DL Debug> VkRenderPass successfully destroyed." << std::endl;
            #endif
        }
    }
}
