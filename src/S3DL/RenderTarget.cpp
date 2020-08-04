#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderTarget::RenderTarget(bool hasSurface)
    {
        _hasSurface = hasSurface;
    }

    bool RenderTarget::hasVulkanSurface() const
    {
        return _hasSurface;
    }

    VkSurfaceKHR RenderTarget::getVulkanSurface() const
    {
        return _surface;
    }

    void RenderTarget::setDevice(const Device& device)
    {
        _device = &device;
        createRenderImages();
    }

    void RenderTarget::bindPipeline(RenderPipeline& pipeline)
    {
        _pipeline = &pipeline;
    }

    void RenderTarget::draw(const Drawable& drawable)
    {
        VkCommandBuffer commandBuffer = drawable.getVulkanCommandBuffer(*_device, *_pipeline, _framebuffers[_currentFrame], _extent);

        // Etc...
    }
}
