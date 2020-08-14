#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Buffer
    {
        public:

            Buffer(uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
            Buffer(const Buffer& buffer) = delete;

            Buffer& operator=(const Buffer& buffer) = delete;

            void setData(const void* data, uint64_t size, uint64_t offset = 0);

            VkBuffer getVulkanBuffer() const;
            
            ~Buffer();

        private:

            uint32_t findSuitableMemory(uint32_t typeFilter, VkMemoryPropertyFlags properties);

            uint64_t _size;
            VkBufferUsageFlags _usage;
            VkMemoryPropertyFlags _properties;

            VkBuffer _buffer;
            VkDeviceMemory _deviceMemory;
    };
}
