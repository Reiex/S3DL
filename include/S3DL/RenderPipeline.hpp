#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>
#include <S3DL/RenderTarget.hpp>
#include <S3DL/RenderPass.hpp>
#include <S3DL/Shader.hpp>

namespace s3dl
{
    class RenderPipeline
    {
        public:

            RenderPipeline();

            void setRenderPass(const RenderPass& renderPass);
            void setShader(const Shader& shader);
            void setVertexInputInfo(const VkPipelineVertexInputStateCreateInfo& description);

            void setBlendMode(const VkPipelineColorBlendAttachmentState& blendAttachment, const VkPipelineColorBlendStateCreateInfo& blending);
            void setPrimitiveTopology(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, bool primitiveRestartEnabled = false);
            void setViewportState(const VkViewport& viewport, const VkRect2D& scissor);
            void setRasterizerState(VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL, VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT);

            VkPipeline getVulkanPipeline(const Device& device) const;
            VkRenderPass getVulkanRenderPass(const Device& device) const;

        private:

            RenderPass _renderPass;
            Shader _shader;
            VkPipelineVertexInputStateCreateInfo _vertexInputDescription;

            VkPipelineColorBlendAttachmentState _blendAttachment;
            VkPipelineColorBlendStateCreateInfo _blending;
            VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
            VkViewport _viewport;
            VkRect2D _scissor;
            VkPipelineViewportStateCreateInfo _viewportState;
            VkPipelineRasterizationStateCreateInfo _rasterizer;
            VkPipelineMultisampleStateCreateInfo _multisampler;

            mutable bool _pipelineComputed;
            mutable VkPipelineLayout _pipelineLayout;
            mutable VkPipeline _pipeline;
    };
}
