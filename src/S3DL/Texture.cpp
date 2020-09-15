#include <S3DL/S3DL.hpp>

namespace s3dl
{

    TextureSampler::TextureSampler() :
        _vulkanSamplerComputed(false),
        _vulkanSampler(VK_NULL_HANDLE)
    {
        _sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        _sampler.magFilter = VK_FILTER_NEAREST;
        _sampler.minFilter = VK_FILTER_NEAREST;
        _sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        _sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        _sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        _sampler.anisotropyEnable = VK_TRUE;
        _sampler.maxAnisotropy = 16.0f;
        _sampler.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        _sampler.unnormalizedCoordinates = VK_FALSE;
        _sampler.compareEnable = VK_FALSE;
        _sampler.compareOp = VK_COMPARE_OP_ALWAYS;
        _sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        _sampler.mipLodBias = 0.0f;
        _sampler.minLod = 0.0f;
        _sampler.maxLod = 0.0f;
    }

    VkSampler TextureSampler::getVulkanSampler() const
    {
        if (!_vulkanSamplerComputed)
            createVulkanSampler();
        
        return _vulkanSampler;
    }

    TextureSampler::~TextureSampler()
    {
        destroyVulkanSampler();
    }

    void TextureSampler::createVulkanSampler() const
    {
        destroyVulkanSampler();

        VkResult result = vkCreateSampler(Device::Active->getVulkanDevice(), &_sampler, nullptr, &_vulkanSampler);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create VkSampler. VkResult: " + std::to_string(result));
        
        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkSampler successfully created." << std::endl;
        #endif
    }

    void TextureSampler::destroyVulkanSampler() const
    {
        if (_vulkanSampler != VK_NULL_HANDLE)
            vkDestroySampler(Device::Active->getVulkanDevice(), _vulkanSampler, nullptr);
        
        _vulkanSampler = VK_NULL_HANDLE;
        _vulkanSamplerComputed = false;
    }

    TextureViewParameters::TextureViewParameters(VkImageAspectFlags aspects, std::array<uint32_t, 2> layerRange)
    {
        _view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        _view.pNext = nullptr;
        _view.flags = 0;
        _view.image = VK_NULL_HANDLE;
        if (layerRange[1] - layerRange[0] > 1)
            _view.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        else
            _view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        _view.format = VK_FORMAT_UNDEFINED;
        _view.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        _view.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        _view.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        _view.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        _view.subresourceRange.aspectMask = aspects;
        _view.subresourceRange.baseMipLevel = 0;
        _view.subresourceRange.levelCount = 1;
        _view.subresourceRange.baseArrayLayer = layerRange[0];
        _view.subresourceRange.layerCount = layerRange[1] - layerRange[0];
    }

    void TextureViewParameters::setFormat(VkFormat format)
    {
        _view.format = format;
    }

    void TextureViewParameters::setViewType(VkImageViewType viewType)
    {
        _view.viewType = viewType;
    }

    void TextureViewParameters::setComponentMapping(VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a)
    {
        _view.components.r = r;
        _view.components.g = g;
        _view.components.b = b;
        _view.components.a = a;
    }

    VkImageViewCreateInfo TextureViewParameters::getVulkanImageViewCreateInfo() const
    {
        return _view;
    }

    TextureArray::TextureArray(const uvec2& size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, uint32_t layerCount) :
        _size(size),
        _layerCount(layerCount),
        _format(format),
        _tiling(tiling),
        _usage(usage),
        
        _vulkanImageMemory(VK_NULL_HANDLE),
        _vulkanImage(VK_NULL_HANDLE),
        _vulkanImageViews({}),
        _sampler(new TextureSampler()),
        _deleteSampler(true),

        _currentLayout(VK_IMAGE_LAYOUT_UNDEFINED)
    {
        // Create vulkan image

        VkImageCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.imageType = VK_IMAGE_TYPE_2D;
        createInfo.format = _format;
        createInfo.extent.width = _size.x;
        createInfo.extent.height = _size.y;
        createInfo.extent.depth = 1;
        createInfo.mipLevels = 1;
        createInfo.arrayLayers = _layerCount;
        createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        createInfo.tiling = _tiling;
        createInfo.usage = _usage;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
        createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkResult result = vkCreateImage(Device::Active->getVulkanDevice(), &createInfo, nullptr, &_vulkanImage);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create image. VkResult: " + std::to_string(result));

        // Allocate memory for image

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(Device::Active->getVulkanDevice(), _vulkanImage, &memRequirements);

        const VkPhysicalDeviceMemoryProperties& memProperties = Device::Active->getPhysicalDevice().memoryProperties;
        uint32_t index(0);
        for (; index < memProperties.memoryTypeCount; index++)
            if ((memRequirements.memoryTypeBits & (1 << index)) && (memProperties.memoryTypes[index].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
                break;
        
        if (index == memProperties.memoryTypeCount)
            throw std::runtime_error("Failed to find suitable memory type for image creation.");

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = index;

        result = vkAllocateMemory(Device::Active->getVulkanDevice(), &allocInfo, nullptr, &_vulkanImageMemory);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate image memory. VkResult: " + std::to_string(result));

        vkBindImageMemory(Device::Active->getVulkanDevice(), _vulkanImage, _vulkanImageMemory, 0);
        
        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkImage successfully created and allocated." << std::endl;
        #endif
    }

    void TextureArray::fillFromTextureData(const TextureData& textureData, uint32_t layer)
    {
        Buffer stagingBuffer(textureData.getRawSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        stagingBuffer.setData(textureData.getRawData(), textureData.getRawSize());
        fillFromBuffer(stagingBuffer, layer, 1);
    }

    void TextureArray::fillFromBuffer(const Buffer& buffer, uint32_t firstLayer, uint32_t layerCount)
    {
        VkImageLayout layout = _currentLayout;
        setLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // Start recording a command buffer

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = Device::Active->getVulkanCommandPool();
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(Device::Active->getVulkanDevice(), &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        // Create copy command

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = firstLayer;
        region.imageSubresource.layerCount = layerCount;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {_size.x, _size.y, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer.getVulkanBuffer(), _vulkanImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        // Finish recording the command buffer and execute it

        vkEndCommandBuffer(commandBuffer);

        VkFence transferFence;
        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        vkCreateFence(Device::Active->getVulkanDevice(), &fenceCreateInfo, nullptr, &transferFence);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkResetFences(Device::Active->getVulkanDevice(), 1, &transferFence);
        vkQueueSubmit(Device::Active->getVulkanGraphicsQueue(), 1, &submitInfo, transferFence);
        
        vkWaitForFences(Device::Active->getVulkanDevice(), 1, &transferFence, VK_TRUE, UINT64_MAX);

        vkFreeCommandBuffers(Device::Active->getVulkanDevice(), Device::Active->getVulkanCommandPool(), 1, &commandBuffer);
        vkDestroyFence(Device::Active->getVulkanDevice(), transferFence, nullptr);

        if (layout != VK_IMAGE_LAYOUT_UNDEFINED)
            setLayout(layout);
    }

    void TextureArray::fillFromTextureArray(const TextureArray& textureArray, uint32_t srcFirstLayer, uint32_t dstFirstLayer, uint32_t layerCount)
    {
        // VkImageLayout srcLayout = textureArray._currentLayout;
        // textureArray.setLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        // VkImageLayout dstLayout = _currentLayout;
        // setLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // // Start recording a command buffer

        // VkCommandBufferAllocateInfo allocInfo{};
        // allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        // allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        // allocInfo.commandPool = Device::Active->getVulkanCommandPool();
        // allocInfo.commandBufferCount = 1;

        // VkCommandBuffer commandBuffer;
        // vkAllocateCommandBuffers(Device::Active->getVulkanDevice(), &allocInfo, &commandBuffer);

        // VkCommandBufferBeginInfo beginInfo{};
        // beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        // vkBeginCommandBuffer(commandBuffer, &beginInfo);

        // // Create copy command
        
        // VkImageCopy copyInfo{};
        // copyInfo.srcSubresource.aspectMask = _imageView.subresourceRange.aspectMask;
        // copyInfo.srcSubresource.mipLevel = 0;
        // copyInfo.srcSubresource.baseArrayLayer = srcLayer;
        // copyInfo.srcSubresource.layerCount = 1;
        // copyInfo.srcOffset = {0, 0, 0};
        // copyInfo.dstSubresource.aspectMask = _imageView.subresourceRange.aspectMask;
        // copyInfo.dstSubresource.mipLevel = 0;
        // copyInfo.dstSubresource.baseArrayLayer = dstLayer;
        // copyInfo.dstSubresource.layerCount = 1;
        // copyInfo.dstOffset = {0, 0, 0};
        // copyInfo.extent = {_size.x, _size.y, 1};

        // vkCmdCopyImage(commandBuffer, texture._vulkanImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _vulkanImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);

        // // Finish recording the command buffer and execute it

        // vkEndCommandBuffer(commandBuffer);

        // VkFence transferFence;
        // VkFenceCreateInfo fenceCreateInfo{};
        // fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        // fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        // vkCreateFence(Device::Active->getVulkanDevice(), &fenceCreateInfo, nullptr, &transferFence);

        // VkSubmitInfo submitInfo{};
        // submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        // submitInfo.commandBufferCount = 1;
        // submitInfo.pCommandBuffers = &commandBuffer;

        // vkResetFences(Device::Active->getVulkanDevice(), 1, &transferFence);
        // vkQueueSubmit(Device::Active->getVulkanGraphicsQueue(), 1, &submitInfo, transferFence);
        
        // vkWaitForFences(Device::Active->getVulkanDevice(), 1, &transferFence, VK_TRUE, UINT64_MAX);

        // vkFreeCommandBuffers(Device::Active->getVulkanDevice(), Device::Active->getVulkanCommandPool(), 1, &commandBuffer);
        // vkDestroyFence(Device::Active->getVulkanDevice(), transferFence, nullptr);

        // if (srcLayout != VK_IMAGE_LAYOUT_UNDEFINED)
        //     texture.setLayout(srcLayout);
        // if (dstLayout != VK_IMAGE_LAYOUT_UNDEFINED)
        //     setLayout(dstLayout);
    }

    void TextureArray::fillFromTexture(const Texture& texture, uint32_t dstLayer)
    {
        // TODO
    }

    void TextureArray::setSampler(const TextureSampler& sampler)
    {

    }

    void TextureArray::updateLayoutState(VkImageLayout layout) const
    {

    }

    void TextureArray::setLayout(VkImageLayout layout) const
    {

    }

    TextureData TextureArray::getTextureData(uint32_t layer) const
    {

    }

    const uvec2& TextureArray::getSize() const
    {

    }

    VkImage TextureArray::getVulkanImage() const
    {

    }

    VkImageView TextureArray::getVulkanImageView(TextureViewParameters viewParameters) const
    {

    }

    VkSampler TextureArray::getVulkanSampler() const
    {

    }

    TextureArray::~TextureArray()
    {
        for (std::pair<const TextureViewParameters, VkImageView>& imageView: _vulkanImageViews)
        {
            if (imageView.second != VK_NULL_HANDLE)
            {
                vkDestroyImageView(Device::Active->getVulkanDevice(), imageView.second, nullptr);
                #ifndef NDEBUG
                std::clog << "<S3DL Debug> VkImageView successfully destroyed." << std::endl;
                #endif
            }
        }

        if (_deleteSampler)
            delete _sampler;

        if (_vulkanImageMemory != VK_NULL_HANDLE)
            vkFreeMemory(Device::Active->getVulkanDevice(), _vulkanImageMemory, nullptr);
        
        if (_vulkanImage != VK_NULL_HANDLE)
            vkDestroyImage(Device::Active->getVulkanDevice(), _vulkanImage, nullptr);
        
        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkImage successfully freed and destroyed." << std::endl;
        #endif
    }

    VkImageAspectFlags TextureArray::getAvailableAspects(VkFormat format)
    {
        switch (format)
        {
            case VK_FORMAT_D32_SFLOAT:
                return VK_IMAGE_ASPECT_DEPTH_BIT;
            case VK_FORMAT_D24_UNORM_S8_UINT:
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
                return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            default:
                return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }

    Texture::Texture(const uvec2& size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage)
    {

    }

    void Texture::fillFromTextureData(const TextureData& textureData)
    {

    }

    void Texture::fillFromBuffer(const Buffer& buffer, uint32_t firstLayer)
    {

    }

    void Texture::fillFromTextureArray(const TextureArray& textureArray, uint32_t srcLayer)
    {

    }

    void Texture::fillFromTexture(const Texture& texture)
    {

    }

    void Texture::setSampler(const TextureSampler& sampler)
    {

    }

    void Texture::updateLayoutState(VkImageLayout layout) const
    {

    }

    void Texture::setLayout(VkImageLayout layout) const
    {

    }

    TextureData Texture::getTextureData() const
    {

    }

    const uvec2& Texture::getSize() const
    {

    }

    VkImage Texture::getVulkanImage() const
    {

    }

    VkImageView Texture::getVulkanImageView(TextureViewParameters viewParameters) const
    {

    }

    VkSampler Texture::getVulkanSampler() const
    {

    }

    Texture::~Texture()
    {

    }
}
