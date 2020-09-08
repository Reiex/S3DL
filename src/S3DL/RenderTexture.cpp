#include <S3DL/S3DL.hpp>

namespace s3dl
{
    RenderTexture::RenderTexture(const uvec2& size) : RenderTarget(false)
    {
        _targetSize = size;
    }
}
