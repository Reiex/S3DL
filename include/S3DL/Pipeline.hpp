#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Pipeline
    {
        public:

            Pipeline(const Pipeline&) = delete;

            Pipeline& operator=(const Pipeline& pipeline) = delete;

            void setVertexInput(const std::vector<VkVertexInputBindingDescription>& bindings, const std::vector<VkVertexInputAttributeDescription>& attributes);
            void setDepthTest(bool depthTestEnabled, bool depthWriteEnabled, VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS);

            PipelineLayout* getPipelineLayout() const;
            VkPipeline getVulkanPipeline() const;

            ~Pipeline();

        private:

            Pipeline(const RenderPass& renderPass, unsigned int subpass, const Shader& shader, const RenderTarget& target);
            void destroyVulkanPipeline() const;

            const Shader* _shader;

            std::vector<VkVertexInputBindingDescription> _inputBindings;
            std::vector<VkVertexInputAttributeDescription> _inputAttributes;
            VkPipelineVertexInputStateCreateInfo _vertexInput;

            VkPipelineInputAssemblyStateCreateInfo _inputAssembly;

            VkViewport _viewport;
            VkRect2D _scissor;
            VkPipelineViewportStateCreateInfo _viewportState;

            VkPipelineRasterizationStateCreateInfo _rasterizer;
            
            VkPipelineMultisampleStateCreateInfo _multisampler;

            VkPipelineDepthStencilStateCreateInfo _depthStencil;

            std::vector<VkPipelineColorBlendAttachmentState> _blendAttachments;
            VkPipelineColorBlendStateCreateInfo _blendState;

            PipelineLayout* _pipelineLayout;

            mutable VkGraphicsPipelineCreateInfo _pipeline;

            mutable bool _vulkanPipelineComputed;
            mutable VkPipeline _vulkanPipeline;
        
        friend RenderPass;
    };
}
