#include <S3DL/S3DL.hpp>

namespace s3dl
{
    VkCommandBuffer Drawable::getVulkanCommandBuffer(const Device& device, const RenderPipeline& pipeline, VkFramebuffer framebuffer, VkExtent2D extent) const
    {
        if (!_commandCreated)
        {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = device.getVulkanCommandPool();
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = 1;

            VkResult result = vkAllocateCommandBuffers(device.getVulkanDevice(), &allocInfo, &_commandBuffer);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to allocate command buffers. VkResult: " + std::to_string(result));
            
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr;

            result = vkBeginCommandBuffer(_commandBuffer, &beginInfo);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to begin recording command buffer. VkResult: " + std::to_string(result));
            
            VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = pipeline.getVulkanRenderPass(device);
            renderPassInfo.framebuffer = framebuffer;
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = extent;
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getVulkanPipeline(device));

            vkCmdDraw(_commandBuffer, 3, 1, 0, 0);
            vkCmdEndRenderPass(_commandBuffer);

            result = vkEndCommandBuffer(_commandBuffer);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to record command buffer. VkResult: " + std::to_string(result));
            
            _commandCreated = true;
        }

        return _commandBuffer;
    }

    Drawable::~Drawable()
    {
        // Oops...
    }
}
