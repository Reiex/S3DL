#pragma once

#include <string>
#include <cstring>
#include <cstdint>
#include <array>
#include <unordered_map>
#include <boost/functional/hash.hpp>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class TextureSampler
    {
        public:

            TextureSampler();
            TextureSampler(const TextureSampler& sampler) = delete;

            TextureSampler& operator=(const TextureSampler& sampler) = delete;

            VkSampler getVulkanSampler() const;

            ~TextureSampler();

        private:

            void createVulkanSampler() const;
            void destroyVulkanSampler() const;

            VkSamplerCreateInfo _sampler;
            mutable bool _vulkanSamplerComputed;
            mutable VkSampler _vulkanSampler;
    };

    class TextureViewParameters
    {
        public:

            TextureViewParameters(const TextureViewParameters& ) = default;
            TextureViewParameters(VkImageAspectFlags aspects, std::array<uint32_t, 2> layerRange = {0, 1});

            void setFormat(VkFormat format);
            void setViewType(VkImageViewType viewType);
            void setComponentMapping(VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a = VK_COMPONENT_SWIZZLE_IDENTITY);

            const VkImageViewCreateInfo& getVulkanImageViewCreateInfo() const;

            struct Hasher
            {
                size_t operator()(const TextureViewParameters& x) const;
            };

            struct Comparator
            {
                bool operator()(const TextureViewParameters& x, const TextureViewParameters& y) const;
            };

        private:

            VkImageViewCreateInfo _view;
    };

    class TextureArray
    {
        public:

            TextureArray(const uvec2& size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, uint32_t layerCount);
            TextureArray(const TextureArray& textureArray) = delete;

            TextureArray& operator=(const TextureArray& textureArray) = delete;

            void fillFromTextureData(const TextureData& textureData, uint32_t layer);
            void fillFromBuffer(const Buffer& buffer, uint32_t firstLayer, uint32_t layerCount);
            void fillFromTextureArray(const TextureArray& textureArray, uint32_t srcFirstLayer, uint32_t dstFirstLayer, uint32_t layerCount);
            void fillFromTexture(const Texture& texture, uint32_t dstLayer);

            void setSampler(const TextureSampler& sampler);

            void updateLayoutState(VkImageLayout layout) const;
            void setLayout(VkImageLayout layout) const;

            VkFormat getFormat() const;

            TextureData getTextureData(uint32_t layer) const;
            const uvec2& getSize() const;

            VkImage getVulkanImage() const;
            VkImageView getVulkanImageView(const TextureViewParameters& viewParameters) const;
            VkSampler getVulkanSampler() const;

            ~TextureArray();

        protected:

            static VkImageAspectFlags getAvailableAspects(VkFormat format);

            uvec2 _size;
            uint32_t _layerCount;
            VkFormat _format;
            VkImageTiling _tiling;
            VkImageUsageFlags _usage;

            VkDeviceMemory _vulkanImageMemory;
            VkImage _vulkanImage;
            mutable std::unordered_map<TextureViewParameters, VkImageView, TextureViewParameters::Hasher, TextureViewParameters::Comparator> _vulkanImageViews;
            const TextureSampler* _sampler;
            bool _deleteSampler;

            mutable VkImageLayout _currentLayout;
    };

    class Texture: private TextureArray
    {
        public:

            Texture(const uvec2& size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
            Texture(const Texture& texture) = delete;

            Texture& operator=(const Texture& texture) = delete;

            void fillFromTextureData(const TextureData& textureData);
            void fillFromBuffer(const Buffer& buffer);
            void fillFromTextureArray(const TextureArray& textureArray, uint32_t srcLayer);
            void fillFromTexture(const Texture& texture);

            void setSampler(const TextureSampler& sampler);

            void updateLayoutState(VkImageLayout layout) const;
            void setLayout(VkImageLayout layout) const;

            VkFormat getFormat() const;

            TextureData getTextureData() const;
            const uvec2& getSize() const;

            VkImage getVulkanImage() const;
            VkImageView getVulkanImageView(const TextureViewParameters& viewParameters) const;
            VkSampler getVulkanSampler() const;

            ~Texture();
        
        friend TextureArray;
    };
}
