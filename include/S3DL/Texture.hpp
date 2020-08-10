#pragma once

#include <string>
#include <cstring>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Texture
    {
        public:

            Texture();
            Texture(const Device& device, const TextureData& textureData, const TextureSampler& sampler = TextureSampler());

            void setLayout(VkImageLayout layout);

            ~Texture();

        private:

            void fillFromBuffer(VkBuffer buffer);

            const Device* _device;

            VkImage _image;
            VkDeviceMemory _imageMemory;
            VkImageView _imageView;
            VkSampler _sampler;

            uvec2 _size;
            VkImageLayout _currentLayout;
    };
}
