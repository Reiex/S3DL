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

            Texture(const uvec2& size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags imageAspects, TextureSampler sampler = TextureSampler(), unsigned int layerCount = 1);
            Texture(const Texture& texture) = delete;

            Texture& operator=(const Texture& texture) = delete;

            void fillFromTextureData(const TextureData& textureData, unsigned int layer = 0);
            void fillFromBuffer(const Buffer& buffer, unsigned int layer = 0);
            void fillFromTexture(const Texture& texture, unsigned int srcLayer = 0, unsigned int dstLayer = 0);

            void updateLayoutState(VkImageLayout layout) const;
            void setLayout(VkImageLayout layout) const;

            TextureData getTextureData(VkImageAspectFlagBits aspect = VK_IMAGE_ASPECT_COLOR_BIT, unsigned int layer = 0) const;
            const uvec2& getSize() const;

            VkImage getVulkanImage() const;
            VkImageView getVulkanImageView() const;
            VkImageView getVulkanImmondeView() const;
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
            VkImageView _vulkanImmondeView;
            VkSampler _vulkanSampler;

            uvec2 _size;
            mutable VkImageLayout _currentLayout;
    };
}
