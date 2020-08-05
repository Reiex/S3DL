#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderPipeline::RenderPipeline() :
        _pipelineComputed(false),
        _blendAttachment{
            VK_FALSE,                                                     // .blendEnable
            VK_BLEND_FACTOR_ONE,                                          // .srcColorBlendFactor
            VK_BLEND_FACTOR_ZERO,                                         // .dstColorBlendFactor
            VK_BLEND_OP_ADD,                                              // .colorBlendOp
            VK_BLEND_FACTOR_ONE,                                          // .srcAlphaBlendFactor
            VK_BLEND_FACTOR_ZERO,                                         // .dstAlphaBlendFactor
            VK_BLEND_OP_ADD,                                              // .alphaBlendOp
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT  // .colorWriteMask
        },
        _blending{
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,     // .sType
            nullptr,                                                      // .pNext
            0,                                                            // .flags
            VK_FALSE,                                                     // .logicOpEnable
            VK_LOGIC_OP_COPY,                                             // .logicOp
            1,                                                            // .attachmentCount
            &_blendAttachment,                                            // .pAttachments
            {0.f, 0.f, 0.f, 0.f}                                          // .blendConstants
        },
        _inputAssembly{
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // .sType
            nullptr,                                                      // .pNext
            0,                                                            // .flags
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                          // .topology
            VK_FALSE                                                      // .primitiveRestartEnable
        },
        _viewport{
            0.f,                                                          // .x
            0.f,                                                          // .y
            1.f,                                                          // .width
            1.f,                                                          // .height
            0.f,                                                          // .minDepth
            1.f                                                           // .maxDepth
        },
        _scissor{
            { 0, 0 },                                                     // .offset
            { 1, 1 }                                                      // .extent
        },
        _viewportState{
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,        // .sType
            nullptr,                                                      // .pNext
            0,                                                            // .flags
            1,                                                            // .viewportCount
            &_viewport,                                                   // .pViewports
            1,                                                            // .scissorCount
            &_scissor                                                     // .pScissors
        },
        _rasterizer{
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,   // .sType
            nullptr,                                                      // .pNext
            0,                                                            // .flags
            VK_FALSE,                                                     // .depthClampEnable
            VK_FALSE,                                                     // .rasterizerDiscardEnable
            VK_POLYGON_MODE_FILL,                                         // .polygonMode
            VK_CULL_MODE_NONE,                                            // .cullMode
            VK_FRONT_FACE_CLOCKWISE,                                      // .frontFace
            VK_FALSE,                                                     // .depthBiasEnable
            0.0f,                                                         // .depthBiasConstantFactor
            0.0f,                                                         // .depthBiasClamp
            0.0f,                                                         // .depthBiasSlopeFactor
            1.0f                                                          // .lineWidth
        },
        _multisampler{
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,     // .sType
            nullptr,                                                      // .pNext
            0,                                                            // .flags
            VK_SAMPLE_COUNT_1_BIT,                                        // .rasterizationSamples
            VK_FALSE,                                                     // .sampleShadingEnable
            1.0f,                                                         // .minSampleShading
            nullptr,                                                      // .pSampleMask
            VK_FALSE,                                                     // .alphaToCoverageEnable
            VK_FALSE                                                      // .alphaToOneEnable
        }
    {
    }

    void RenderPipeline::setRenderPass(const RenderPass& renderPass)
    {
        _pipelineComputed = false;
        _renderPass = renderPass;
    }

    void RenderPipeline::setShader(const Shader& shader)
    {
        _pipelineComputed = false;
        _shader = shader;
    }

    void RenderPipeline::setVertexInputInfo(const VkPipelineVertexInputStateCreateInfo& description)
    {
        _pipelineComputed = false;
        _vertexInputDescription = description;
    }

    void RenderPipeline::setBlendMode(const VkPipelineColorBlendAttachmentState& blendAttachment, const VkPipelineColorBlendStateCreateInfo& blending)
    {
        _blendAttachment = blendAttachment;
        _blending = blending;

        _blending.pAttachments = &_blendAttachment;
    }

    void RenderPipeline::setPrimitiveTopology(VkPrimitiveTopology topology, bool primitiveRestartEnabled)
    {
        _pipelineComputed = false;
        _inputAssembly.topology = topology;
        _inputAssembly.primitiveRestartEnable = primitiveRestartEnabled;
    }

    void RenderPipeline::setViewportState(const VkViewport& viewport, const VkRect2D& scissor)
    {
        _pipelineComputed = false;
        _viewport = viewport;
        _scissor = scissor;
    }

    void RenderPipeline::setRasterizerState(VkPolygonMode polygonMode, VkCullModeFlags cullMode)
    {
        _pipelineComputed = false;
        _rasterizer.polygonMode = polygonMode;
        _rasterizer.cullMode = cullMode;
    }

    VkPipeline RenderPipeline::getVulkanPipeline(const Device& device) const
    {
        if (!_pipelineComputed)
        {
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 0;
            pipelineLayoutInfo.pSetLayouts = nullptr;
            pipelineLayoutInfo.pushConstantRangeCount = 0;
            pipelineLayoutInfo.pPushConstantRanges = nullptr;

            VkResult result = vkCreatePipelineLayout(device.getVulkanDevice(), &pipelineLayoutInfo, nullptr, &_pipelineLayout);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create pipeline layout. VkResult: " + std::to_string(result));

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = _shader.getVulkanShaderStages().size();
            pipelineInfo.pStages = _shader.getVulkanShaderStages().data();
            pipelineInfo.pVertexInputState = &_vertexInputDescription;
            pipelineInfo.pInputAssemblyState = &_inputAssembly;
            pipelineInfo.pViewportState = &_viewportState;
            pipelineInfo.pRasterizationState = &_rasterizer;
            pipelineInfo.pMultisampleState = &_multisampler;
            pipelineInfo.pDepthStencilState = nullptr;
            pipelineInfo.pColorBlendState = &_blending;
            pipelineInfo.pDynamicState = nullptr;
            pipelineInfo.layout = _pipelineLayout;
            pipelineInfo.renderPass = _renderPass.getVulkanRenderPass(device);
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
            pipelineInfo.basePipelineIndex = -1;

            result = vkCreateGraphicsPipelines(device.getVulkanDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create graphics pipeline. VkResult: " + std::to_string(result));

            _pipelineComputed = true;
        }

        return _pipeline;
    }

    VkRenderPass RenderPipeline::getVulkanRenderPass(const Device& device) const
    {
        return _renderPass.getVulkanRenderPass(device);
    }

}
