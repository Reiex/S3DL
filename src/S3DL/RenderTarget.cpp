#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderTarget::RenderTarget(bool hasVulkanSurface):
        _targetSize(0, 0),
        _swapchain(nullptr),

        _hasVulkanSurface(hasVulkanSurface),
        _surface(VK_NULL_HANDLE)
    {
    }

    void RenderTarget::setSwapchain(const Swapchain& swapchain)
    {
        _swapchain = &swapchain;
        _targetSize = {swapchain._extent.width, swapchain._extent.height};
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
}
