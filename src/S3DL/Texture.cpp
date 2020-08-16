#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Texture::Texture(const uvec2& size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags imageAspects) : Texture()
    {
        _size = size;
        _currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        // Create vulkan image

        _image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        _image.pNext = nullptr;
        _image.flags = 0;
        _image.imageType = VK_IMAGE_TYPE_2D;
        _image.format = format;
        _image.extent.width = _size.x;
        _image.extent.height = _size.y;
        _image.extent.depth = 1;
        _image.mipLevels = 1;
        _image.arrayLayers = 1;
        _image.samples = VK_SAMPLE_COUNT_1_BIT;
        _image.tiling = tiling;
        _image.usage = usage;
        _image.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        _image.queueFamilyIndexCount = 0;
        _image.pQueueFamilyIndices = nullptr;
        _image.initialLayout = _currentLayout;

        VkResult result = vkCreateImage(Device::Active->getVulkanDevice(), &_image, nullptr, &_vulkanImage);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create image. VkResult: " + std::to_string(result));
        
        // Allocate memory for vulkan image

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

        // Create image view

        _imageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        _imageView.pNext = nullptr;
        _imageView.flags = 0;
        _imageView.image = _vulkanImage;
        _imageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
        _imageView.format = format;
        _imageView.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        _imageView.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        _imageView.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        _imageView.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        _imageView.subresourceRange.aspectMask = imageAspects;
        _imageView.subresourceRange.baseMipLevel = 0;
        _imageView.subresourceRange.levelCount = 1;
        _imageView.subresourceRange.baseArrayLayer = 0;
        _imageView.subresourceRange.layerCount = 1;

        result = vkCreateImageView(Device::Active->getVulkanDevice(), &_imageView, nullptr, &_vulkanImageView);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create texture image view. VkResult: " + std::to_string(result));
        
        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkImage, VkImageMemory, VkImageView & VkSampler successfully created." << std::endl;
        #endif
    }

    Texture::Texture(const TextureData& textureData) : Texture(textureData.size(), VK_FORMAT_R8G8B8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT)
    {
        // Create staging buffer with image in it

        Buffer stagingBuffer(textureData.getRawSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        stagingBuffer.setData(textureData.getRawData(), textureData.getRawSize());

        // Fill vulkan image

        setLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        fillFromBuffer(stagingBuffer.getVulkanBuffer());
        setLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    void Texture::setLayout(VkImageLayout layout)
    {
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

        // Create barrier

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = _currentLayout;
        barrier.newLayout = layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = _vulkanImage;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

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

        // Change current layout

        _currentLayout = layout;
    }

    VkImage Texture::getVulkanImage() const
    {
        return _vulkanImage;
    }

    VkImageView Texture::getVulkanImageView() const
    {
        return _vulkanImageView;
    }

    VkSampler Texture::getVulkanSampler() const
    {
        return _vulkanSampler;
    }

    Texture::~Texture()
    {
        if (_vulkanImageView != VK_NULL_HANDLE)
            vkDestroyImageView(Device::Active->getVulkanDevice(), _vulkanImageView, nullptr);
        
        if (_vulkanImageMemory != VK_NULL_HANDLE)
            vkFreeMemory(Device::Active->getVulkanDevice(), _vulkanImageMemory, nullptr);
        
        if (_vulkanImage != VK_NULL_HANDLE)
            vkDestroyImage(Device::Active->getVulkanDevice(), _vulkanImage, nullptr);
            
        if (_vulkanSampler != VK_NULL_HANDLE)
            vkDestroySampler(Device::Active->getVulkanDevice(), _vulkanSampler, nullptr);
        
        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkImage, VkImageMemory, VkImageView & VkSampler successfully destroyed." << std::endl;
        #endif
    }

    Texture::Texture() :
        _image{},
        _imageView{},
        _sampler(),

        _vulkanImage(VK_NULL_HANDLE),
        _vulkanImageMemory(VK_NULL_HANDLE),
        _vulkanImageView(VK_NULL_HANDLE),
        _vulkanSampler(VK_NULL_HANDLE),

        _size(0, 0),
        _currentLayout(VK_IMAGE_LAYOUT_UNDEFINED)
    {
        // Create image sampler

        VkResult result = vkCreateSampler(Device::Active->getVulkanDevice(), &_sampler.info, nullptr, &_vulkanSampler);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create texture sampler. VkResult: " + std::to_string(result));
    }

    void Texture::fillFromBuffer(VkBuffer buffer)
    {
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
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {_size.x, _size.y, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer, _vulkanImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

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
    }
}