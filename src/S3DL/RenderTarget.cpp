#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderTarget::RenderTarget(bool hasVulkanSurface):
        _targetSize(0, 0),
        _swapchain(nullptr),

        _hasVulkanSurface(hasVulkanSurface),
        _surface(VK_NULL_HANDLE),

        _currentRenderPass(nullptr)
    {
    }

    void RenderTarget::setSwapchain(const Swapchain& swapchain)
    {
        _swapchain = &swapchain;
        _targetSize = {swapchain.getExtent().width, swapchain.getExtent().height};
    }

    void RenderTarget::beginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer, const std::vector<VkClearValue>& clearValues)
    {
        endRenderPass();
        _currentRenderPass = &renderPass;

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass.getVulkanRenderPass();
        renderPassInfo.framebuffer = framebuffer.getCurrentFramebuffer();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = _swapchain->getExtent();
        renderPassInfo.clearValueCount = clearValues.size();
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(_swapchain->getCurrentCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void RenderTarget::bindPipeline(const Pipeline* pipeline)
    {
        vkCmdBindPipeline(_swapchain->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getVulkanPipeline());
    }

    void RenderTarget::draw()
    {
        vkCmdDraw(_swapchain->getCurrentCommandBuffer(), 3, 1, 0, 0);
    }

    void RenderTarget::beginNextSubpass()
    {
        vkCmdNextSubpass(_swapchain->getCurrentCommandBuffer(), VK_SUBPASS_CONTENTS_INLINE);
    }

    void RenderTarget::subpassDraw()
    {
        vkCmdDraw(_swapchain->getCurrentCommandBuffer(), 3, 1, 0, 0);
    }

    void RenderTarget::display()
    {
        endRenderPass();
        _swapchain->updateDisplay(*this);
    }

    const uvec2& RenderTarget::getTargetSize() const
    {
        return _targetSize;
    }

    bool RenderTarget::hasVulkanSurface() const
    {
        return _hasVulkanSurface;
    }

    VkSurfaceKHR RenderTarget::getVulkanSurface() const
    {
        return _surface;
    }

    void RenderTarget::endRenderPass()
    {
        if (_currentRenderPass != nullptr)
        {
            vkCmdEndRenderPass(_swapchain->getCurrentCommandBuffer());
            _currentRenderPass = nullptr;
        }
    }
}
