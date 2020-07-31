#include <S3DL/S3DL.hpp>

namespace s3dl
{
    const VkSurfaceKHR& RenderTarget::getVulkanSurface() const
    {
        return _surface;
    }
}
