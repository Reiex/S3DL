#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Buffer::Buffer() :
        _device(nullptr),
        _size(0),
        _usage{},
        _properties{},
        _buffer(VK_NULL_HANDLE),
        _deviceMemory(VK_NULL_HANDLE)
    {
    }

    Buffer::Buffer(const Device& device, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) :
        _device(&device),
        _size(size),
        _usage(usage),
        _properties(properties),
        _buffer(VK_NULL_HANDLE),
        _deviceMemory(VK_NULL_HANDLE)
    {
        create();
    }

    void Buffer::reCreate(const Device& device, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    {
        destroy();

        _device = &device;
        _size = size;
        _usage = usage;
        _properties = properties;

        create();
    }
    
    void Buffer::setData(const void* data, uint64_t size, uint64_t offset)
    {
        if (_properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        {
            void* handle;
            vkMapMemory(_device->getVulkanDevice(), _deviceMemory, offset, size, 0, &handle);
            std::memcpy(handle, data, size);
            vkUnmapMemory(_device->getVulkanDevice(), _deviceMemory);
        }
        else
        {
            Buffer stagingBuffer(*_device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            stagingBuffer.setData(data, size, 0);

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

            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = offset;
            copyRegion.size = size;
            vkCmdCopyBuffer(commandBuffer, stagingBuffer.getVulkanBuffer(), _buffer, 1, &copyRegion);

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

    VkBuffer Buffer::getVulkanBuffer() const
    {
        return _buffer;
    }
    
    Buffer::~Buffer()
    {
        destroy();
    }
    
    uint32_t Buffer::findSuitableMemory(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        const VkPhysicalDeviceMemoryProperties& memProperties = _device->getPhysicalDeviceProperties().memoryProperties;

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        
        throw std::runtime_error("Failed to find suitable memory type for buffer creation.");
    }

    void Buffer::create()
    {
        // Create the buffer itself

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = _size;
        bufferInfo.usage = _usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(_device->getVulkanDevice(), &bufferInfo, nullptr, &_buffer);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create buffer. VkResult: " + std::to_string(result));
        
        // Allocate memory to buffer

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(_device->getVulkanDevice(), _buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findSuitableMemory(memRequirements.memoryTypeBits, _properties);

        result = vkAllocateMemory(_device->getVulkanDevice(), &allocInfo, nullptr, &_deviceMemory);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate buffer memory. VkResult: " + std::to_string(result));

        vkBindBufferMemory(_device->getVulkanDevice(), _buffer, _deviceMemory, 0);
    }  

    void Buffer::destroy()
    {
        if (_buffer != VK_NULL_HANDLE)
            vkDestroyBuffer(_device->getVulkanDevice(), _buffer, nullptr);
        
        if (_deviceMemory != VK_NULL_HANDLE)
            vkFreeMemory(_device->getVulkanDevice(), _deviceMemory, nullptr);

        _buffer = VK_NULL_HANDLE;
        _deviceMemory = VK_NULL_HANDLE;
    }
}
