#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    struct TextureSampler
    {
        TextureSampler();

        VkSamplerCreateInfo info;
    };
}
