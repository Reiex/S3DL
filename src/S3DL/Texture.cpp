#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Texture::Texture() :
        _device(nullptr),
        _image(VK_NULL_HANDLE),
        _imageMemory(VK_NULL_HANDLE),
        _imageView(VK_NULL_HANDLE),
        _sampler(VK_NULL_HANDLE),
        _size(0, 0),
        _currentLayout(VK_IMAGE_LAYOUT_UNDEFINED)
    {
    }

    Texture::Texture(const Device& device, const uvec2& size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags imageAspects) : Texture()
    {
        _size = size;
        _device = &device;

        // Create vulkan image

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = _size.x;
        imageInfo.extent.height = _size.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0;

        VkResult result = vkCreateImage(_device->getVulkanDevice(), &imageInfo, nullptr, &_image);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create image. VkResult: " + std::to_string(result));
        
        _currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        
        // Allocate memory for vulkan image

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(_device->getVulkanDevice(), _image, &memRequirements);

        const VkPhysicalDeviceMemoryProperties& memProperties = _device->getPhysicalDeviceProperties().memoryProperties;
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

        result = vkAllocateMemory(_device->getVulkanDevice(), &allocInfo, nullptr, &_imageMemory);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate image memory. VkResult: " + std::to_string(result));

        vkBindImageMemory(_device->getVulkanDevice(), _image, _imageMemory, 0);

        // Create image view

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = _image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = imageAspects;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(_device->getVulkanDevice(), &viewInfo, nullptr, &_imageView);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create texture image view. VkResult: " + std::to_string(result));
    }

    Texture::Texture(const Device& device, const TextureData& textureData, const TextureSampler& sampler) : Texture(device, textureData.size(), VK_FORMAT_R8G8B8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT)
    {
        // Create staging buffer with image in it

        Buffer stagingBuffer(*_device, textureData.getRawSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        stagingBuffer.setData(textureData.getRawData(), textureData.getRawSize());

        // Fill vulkan image

        setLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        fillFromBuffer(stagingBuffer.getVulkanBuffer());
        setLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        
        // Create image sampler

        VkResult result = vkCreateSampler(_device->getVulkanDevice(), &sampler.info, nullptr, &_sampler);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create texture sampler. VkResult: " + std::to_string(result));
    }

    void Texture::setLayout(VkImageLayout layout)
    {
        // Start recording a command buffer

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _device->getVulkanCommandPool();
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(_device->getVulkanDevice(), &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        // Create barrier

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = _currentLayout;
        barrier.newLayout = layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = _image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;  // TODO
        barrier.dstAccessMask = 0;  // TODO

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        switch (_currentLayout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                barrier.srcAccessMask = 0;
                break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;
            default:
                throw std::invalid_argument("Unsupported layout transition.");
        }

        switch (layout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                destinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                barrier.dstAccessMask = 0;
                break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                break;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;
            default:
                throw std::invalid_argument("Unsupported layout transition.");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        // Finish recording the command buffer and execute it

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(_device->getVulkanQueues().graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(_device->getVulkanQueues().graphicsQueue);

        vkFreeCommandBuffers(_device->getVulkanDevice(), _device->getVulkanCommandPool(), 1, &commandBuffer);

        // Change current layout

        _currentLayout = layout;
    }

    Texture::~Texture()
    {
        if (_sampler != VK_NULL_HANDLE)
            vkDestroySampler(_device->getVulkanDevice(), _sampler, nullptr);
        _sampler = VK_NULL_HANDLE;
        
        if (_imageView != VK_NULL_HANDLE)
            vkDestroyImageView(_device->getVulkanDevice(), _imageView, nullptr);
        _imageView = VK_NULL_HANDLE;
        
        if (_imageMemory != VK_NULL_HANDLE)
            vkFreeMemory(_device->getVulkanDevice(), _imageMemory, nullptr);
        _imageMemory = VK_NULL_HANDLE;
        
        if (_image != VK_NULL_HANDLE)
            vkDestroyImage(_device->getVulkanDevice(), _image, nullptr);
        _image = VK_NULL_HANDLE;
    }

    void Texture::fillFromBuffer(VkBuffer buffer)
    {
        // Start recording a command buffer

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _device->getVulkanCommandPool();
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(_device->getVulkanDevice(), &allocInfo, &commandBuffer);

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
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {_size.x, _size.y, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        // Finish recording the command buffer and execute it

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(_device->getVulkanQueues().graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(_device->getVulkanQueues().graphicsQueue);

        vkFreeCommandBuffers(_device->getVulkanDevice(), _device->getVulkanCommandPool(), 1, &commandBuffer);
    }
}
