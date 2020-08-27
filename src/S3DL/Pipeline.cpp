#include <S3DL/S3DL.hpp>

namespace s3dl
{
    void Pipeline::setVertexInput(const std::vector<VkVertexInputBindingDescription>& bindings, const std::vector<VkVertexInputAttributeDescription>& attributes)
    {
        _inputBindings = bindings;
        _vertexInput.vertexBindingDescriptionCount = _inputBindings.size();
        _vertexInput.pVertexBindingDescriptions = (_inputBindings.size() != 0) ? _inputBindings.data(): nullptr;

        _inputAttributes = attributes;
        _vertexInput.vertexAttributeDescriptionCount = _inputAttributes.size();
        _vertexInput.pVertexAttributeDescriptions = (_inputAttributes.size() != 0) ? _inputAttributes.data(): nullptr;

        _vulkanPipelineComputed = false;
        _vulkanPipelineLayoutComputed = false;
    }

    VkPipelineLayout Pipeline::getVulkanPipelineLayout() const
    {
        if (!_vulkanPipelineLayoutComputed)
        {
            destroyVulkanPipelineLayout();

            VkResult result = vkCreatePipelineLayout(Device::Active->getVulkanDevice(), &_pipelineLayout, nullptr, &_vulkanPipelineLayout);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create pipeline layout. VkResult: " + std::to_string(result));

            #ifndef NDEBUG
            std::clog << "<S3DL Debug> VkPipelineLayout successfully created." << std::endl;
            #endif
            
            _vulkanPipelineLayoutComputed = true;
        }

        return _vulkanPipelineLayout;
    }

    VkPipeline Pipeline::getVulkanPipeline() const
    {
        if (!_vulkanPipelineComputed | !_vulkanPipelineLayoutComputed)
        {
            destroyVulkanPipeline();
            
            _pipeline.layout = getVulkanPipelineLayout();

            VkResult result = vkCreateGraphicsPipelines(Device::Active->getVulkanDevice(), VK_NULL_HANDLE, 1, &_pipeline, nullptr, &_vulkanPipeline);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create graphics pipeline. VkResult: " + std::to_string(result));

            #ifndef NDEBUG
            std::clog << "<S3DL Debug> VkPipeline successfully created." << std::endl;
            #endif

            _vulkanPipelineComputed = true;
        }

        return _vulkanPipeline;
    }

    Pipeline::~Pipeline()
    {
        destroyVulkanPipeline();
        destroyVulkanPipelineLayout();
    }

    Pipeline::Pipeline(const RenderPass& renderPass, unsigned int subpass, const Shader& shader, const RenderTarget& target)
    {
        // Shader

        _shader = &shader;

        // Vertex input

        _vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        _vertexInput.pNext = nullptr;
        _vertexInput.flags = 0;
        _vertexInput.vertexBindingDescriptionCount = 0;
        _vertexInput.pVertexBindingDescriptions = nullptr;
        _vertexInput.vertexAttributeDescriptionCount = 0;
        _vertexInput.pVertexAttributeDescriptions = nullptr;

        // Input assembly

        _inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        _inputAssembly.pNext = nullptr;
        _inputAssembly.flags = 0;
        _inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        _inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Viewport state

        _viewport.x = 0.f;
        _viewport.y = 0.f;
        _viewport.width = target.getTargetSize().x;
        _viewport.height = target.getTargetSize().y;
        _viewport.minDepth = 0.f;
        _viewport.maxDepth = 1.f;

        _scissor.offset.x = 0;
        _scissor.offset.y = 0;
        _scissor.extent.width = target.getTargetSize().x;
        _scissor.extent.height = target.getTargetSize().y;

        _viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        _viewportState.pNext = nullptr;
        _viewportState.flags = 0;
        _viewportState.viewportCount = 1;
        _viewportState.pViewports = &_viewport;
        _viewportState.scissorCount = 1;
        _viewportState.pScissors = &_scissor;

        // Rasterization

        _rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        _rasterizer.pNext = nullptr;
        _rasterizer.flags = 0;
        _rasterizer.depthClampEnable = VK_FALSE;
        _rasterizer.rasterizerDiscardEnable = VK_FALSE;
        _rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        _rasterizer.cullMode = VK_CULL_MODE_NONE;
        _rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        _rasterizer.depthBiasEnable = VK_FALSE;
        _rasterizer.depthBiasConstantFactor = 0.0f;
        _rasterizer.depthBiasClamp = 0.0f;
        _rasterizer.depthBiasSlopeFactor = 0.0f;
        _rasterizer.lineWidth = 1.0f;

        // Multisampling

        _multisampler.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        _multisampler.pNext = nullptr;
        _multisampler.flags = 0;
        _multisampler.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        _multisampler.sampleShadingEnable = VK_FALSE;
        _multisampler.minSampleShading = 1.0f;
        _multisampler.pSampleMask = nullptr;
        _multisampler.alphaToCoverageEnable = VK_FALSE;
        _multisampler.alphaToOneEnable = VK_FALSE;

        // Depth testing

        _depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        _depthStencil.pNext = nullptr;
        _depthStencil.flags = 0;
        _depthStencil.depthTestEnable = VK_FALSE;
        _depthStencil.depthWriteEnable = VK_FALSE;
        _depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        _depthStencil.depthBoundsTestEnable = VK_FALSE;
        _depthStencil.stencilTestEnable = VK_FALSE;
        _depthStencil.front = VkStencilOpState{};
        _depthStencil.back = VkStencilOpState{};
        _depthStencil.minDepthBounds = 0.0f;
        _depthStencil.maxDepthBounds = 1.0f;

        // Color blending

        VkPipelineColorBlendAttachmentState blendAttachment{};
        blendAttachment.blendEnable = VK_FALSE;
        blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        _blendAttachments.resize(renderPass._colorReferences[subpass].size(), blendAttachment);

        _blendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        _blendState.pNext = nullptr;
        _blendState.flags = 0;
        _blendState.logicOpEnable = VK_FALSE;
        _blendState.logicOp = VK_LOGIC_OP_COPY;
        _blendState.attachmentCount = _blendAttachments.size();
        _blendState.pAttachments = _blendAttachments.data();
        _blendState.blendConstants[0] = 0.f;
        _blendState.blendConstants[1] = 0.f;
        _blendState.blendConstants[2] = 0.f;
        _blendState.blendConstants[3] = 0.f;

        // Pipeline layout

        _pipelineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        _pipelineLayout.pNext = nullptr;
        _pipelineLayout.flags = 0;
        _pipelineLayout.setLayoutCount = 0;
        _pipelineLayout.pSetLayouts = nullptr;
        _pipelineLayout.pushConstantRangeCount = 0;
        _pipelineLayout.pPushConstantRanges = nullptr;

        // Pipeline creation
        
        _pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        _pipeline.pNext = nullptr;
        _pipeline.flags = 0;
        _pipeline.stageCount = _shader->getVulkanShaderStages().size();
        _pipeline.pStages = _shader->getVulkanShaderStages().data();
        _pipeline.pVertexInputState = &_vertexInput;
        _pipeline.pInputAssemblyState = &_inputAssembly;
        _pipeline.pTessellationState = nullptr;
        _pipeline.pViewportState = &_viewportState;
        _pipeline.pRasterizationState = &_rasterizer;
        _pipeline.pMultisampleState = &_multisampler;
        _pipeline.pDepthStencilState = renderPass._subpasses[subpass].pDepthStencilAttachment != nullptr ? &_depthStencil: nullptr;
        _pipeline.pColorBlendState = &_blendState;
        _pipeline.pDynamicState = nullptr;
        _pipeline.layout = VK_NULL_HANDLE;
        _pipeline.renderPass = renderPass.getVulkanRenderPass();
        _pipeline.subpass = subpass;
        _pipeline.basePipelineHandle = VK_NULL_HANDLE;
        _pipeline.basePipelineIndex = -1;

        _vulkanPipelineLayoutComputed = false;
        _vulkanPipelineComputed = false;
        _vulkanPipelineLayout = VK_NULL_HANDLE;
        _vulkanPipeline = VK_NULL_HANDLE;
    }

    void Pipeline::destroyVulkanPipelineLayout() const
    {
        if (_vulkanPipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(Device::Active->getVulkanDevice(), _vulkanPipelineLayout, nullptr);

            #ifndef NDEBUG
            std::clog << "<S3DL Debug> VkPipelineLayout successfully destroyed." << std::endl;
            #endif
        }

        _vulkanPipelineLayout = VK_NULL_HANDLE;
        _vulkanPipelineLayoutComputed = false;
    }

    void Pipeline::destroyVulkanPipeline() const
    {
        if (_vulkanPipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(Device::Active->getVulkanDevice(), _vulkanPipeline, nullptr);

            #ifndef NDEBUG
            std::clog << "<S3DL Debug> VkPipeline successfully destroyed." << std::endl;
            #endif
        }

        _vulkanPipeline = VK_NULL_HANDLE;
        _vulkanPipelineComputed = false;
    }
}
