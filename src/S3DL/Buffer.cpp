#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Buffer::Buffer(uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) :
        _size(size),
        _usage(usage),
        _properties(properties),
        _buffer(VK_NULL_HANDLE),
        _deviceMemory(VK_NULL_HANDLE)
    {
        // Create the buffer itself

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = _size;
        bufferInfo.usage = _usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(Device::Active->getVulkanDevice(), &bufferInfo, nullptr, &_buffer);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create buffer. VkResult: " + std::to_string(result));
        
        // Allocate memory to buffer

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(Device::Active->getVulkanDevice(), _buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findSuitableMemory(memRequirements.memoryTypeBits, _properties);

        result = vkAllocateMemory(Device::Active->getVulkanDevice(), &allocInfo, nullptr, &_deviceMemory);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate buffer memory. VkResult: " + std::to_string(result));

        vkBindBufferMemory(Device::Active->getVulkanDevice(), _buffer, _deviceMemory, 0);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkBuffer of size " + std::to_string(_size) + " successfully created." << std::endl;
        #endif
    }

    void Buffer::setData(const void* data, uint64_t size, uint64_t offset)
    {
        VkResult result;

        if (offset + size > _size)
            throw std::runtime_error("Cannot put " + std::to_string(size) + " bytes of data with offset of " + std::to_string(offset) + " bytes in buffer of size " + std::to_string(_size) + " bytes.");

        if (_properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        {
            void* handle;
            
            result = vkMapMemory(Device::Active->getVulkanDevice(), _deviceMemory, offset, size, 0, &handle);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to map buffer to memory. VkResult: " + std::to_string(result));
            
            std::memcpy(handle, data, size);
            vkUnmapMemory(Device::Active->getVulkanDevice(), _deviceMemory);
        }
        else
        {
            // Create a staging buffer with the data

            Buffer stagingBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            stagingBuffer.setData(data, size, 0);

            // Create a command buffer for data transfer

            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool = Device::Active->getVulkanCommandPool();
            allocInfo.commandBufferCount = 1;

            VkCommandBuffer commandBuffer;
            result = vkAllocateCommandBuffers(Device::Active->getVulkanDevice(), &allocInfo, &commandBuffer);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to allocate command buffer for Buffer memory transfer. VkResult: " + std::to_string(result));

            // Record transfer command

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to start recording command buffer for Buffer memory transfer. VkResult: " + std::to_string(result));

            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = offset;
            copyRegion.size = size;
            vkCmdCopyBuffer(commandBuffer, stagingBuffer.getVulkanBuffer(), _buffer, 1, &copyRegion);

            result = vkEndCommandBuffer(commandBuffer);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to end recording command buffer for Buffer memory transfer. VkResult: " + std::to_string(result));

            // Create fence to wait for transfer to end

            VkFence transferFence;
            VkFenceCreateInfo fenceCreateInfo{};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            result = vkCreateFence(Device::Active->getVulkanDevice(), &fenceCreateInfo, nullptr, &transferFence);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create fence for Buffer memory transfer. VkResult: " + std::to_string(result));

            // Submit command and wait its end with the fence

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkResetFences(Device::Active->getVulkanDevice(), 1, &transferFence);
            vkQueueSubmit(Device::Active->getVulkanGraphicsQueue(), 1, &submitInfo, transferFence);

            vkWaitForFences(Device::Active->getVulkanDevice(), 1, &transferFence, VK_TRUE, UINT64_MAX);

            // Free command buffer and fence (the staging buffer is freed using Buffer class)

            vkFreeCommandBuffers(Device::Active->getVulkanDevice(), Device::Active->getVulkanCommandPool(), 1, &commandBuffer);
            vkDestroyFence(Device::Active->getVulkanDevice(), transferFence, nullptr);
        }
    }

    VkBuffer Buffer::getVulkanBuffer() const
    {
        return _buffer;
    }
    
    Buffer::~Buffer()
    {
        if (_buffer != VK_NULL_HANDLE)
            vkDestroyBuffer(Device::Active->getVulkanDevice(), _buffer, nullptr);
        
        if (_deviceMemory != VK_NULL_HANDLE)
            vkFreeMemory(Device::Active->getVulkanDevice(), _deviceMemory, nullptr);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkBuffer of size " + std::to_string(_size) + " successfully destroyed." << std::endl;
        #endif
    }
    
    uint32_t Buffer::findSuitableMemory(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        const VkPhysicalDeviceMemoryProperties& memProperties = Device::Active->getPhysicalDevice().memoryProperties;

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        
        throw std::runtime_error("Failed to find suitable memory type for buffer creation.");
    }
}
