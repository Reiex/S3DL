#pragma once

#include <vector>

#include <S3DL/types.hpp>
#include <S3DL/RenderTarget.hpp>

namespace s3dl
{
    class RenderTexture: public RenderTarget
    {
        public:

            RenderTexture(const uvec2& size);
            RenderTexture(const RenderTexture& renderTexture) = delete;

            RenderTexture& operator=(const RenderTexture& renderTexture) = delete;
    };
}
