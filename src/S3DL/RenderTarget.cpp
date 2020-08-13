#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderTarget::RenderTarget(bool hasSurface) :
    
        _targetSize(0, 0),

        _hasSurface(hasSurface),
        _surface(VK_NULL_HANDLE)
    {
    }

    bool RenderTarget::hasVulkanSurface() const
    {
        return _hasSurface;
    }

    VkSurfaceKHR RenderTarget::getVulkanSurface() const
    {
        return _surface;
    }
}
