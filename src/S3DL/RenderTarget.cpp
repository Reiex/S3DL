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

    void RenderTarget::createRenderImages()
    {
    }

    void RenderTarget::destroyRenderImages()
    {
    }
}
