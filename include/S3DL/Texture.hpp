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

            Texture(const uvec2& size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags imageAspects, TextureSampler sampler = TextureSampler());
            Texture(const Texture& texture) = delete;

            Texture& operator=(const Texture& texture) = delete;

            void fillFromTextureData(const TextureData& textureData);
            void fillFromBuffer(const Buffer& buffer);
            void fillFromTexture(const Texture& texture);

            void setLayout(VkImageLayout layout) const;

            TextureData getTextureData() const;
            const uvec2& getSize() const;

            VkImage getVulkanImage() const;
            VkImageView getVulkanImageView() const;
            VkSampler getVulkanSampler() const;

            ~Texture();

        private:

            Texture(TextureSampler sampler);

            VkImageCreateInfo _image;
            VkImageViewCreateInfo _imageView;
            TextureSampler _sampler;

            VkImage _vulkanImage;
            VkDeviceMemory _vulkanImageMemory;
            VkImageView _vulkanImageView;
            VkSampler _vulkanSampler;

            uvec2 _size;
            mutable VkImageLayout _currentLayout;
    };
}
