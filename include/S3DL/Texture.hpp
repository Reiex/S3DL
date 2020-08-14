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

            Texture(const uvec2& size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags imageAspects);
            Texture(const TextureData& textureData);
            Texture(const Texture& texture) = delete;

            Texture& operator=(const Texture& texture) = delete;

            void setLayout(VkImageLayout layout);

            VkImage getVulkanImage() const;
            VkImageView getVulkanImageView() const;
            VkSampler getVulkanSampler() const;

            ~Texture();

        private:

            Texture();

            void fillFromBuffer(VkBuffer buffer);

            VkImageCreateInfo _image;
            VkImageViewCreateInfo _imageView;
            TextureSampler _sampler;

            VkImage _vulkanImage;
            VkDeviceMemory _vulkanImageMemory;
            VkImageView _vulkanImageView;
            VkSampler _vulkanSampler;

            uvec2 _size;
            VkImageLayout _currentLayout;
    };
}
