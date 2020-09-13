#pragma once

#include <string>
#include <cstring>
#include <cstdint>

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

		private:

			VkSamplerCreateInfo _sampler;
			mutable bool _vulkanSamplerComputed;
			mutable VkSampler _vulkanSampler;
	};

	class TextureArray
	{
		public:

			TextureArray(const uvec2& size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, uint32_t layerCount);

		private:
	};

	class Texture: public TextureArray
	{

	};
}
