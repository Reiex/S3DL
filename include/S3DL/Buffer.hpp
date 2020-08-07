#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Buffer
    {
        public:

            Buffer();
            Buffer(const Device& device, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

            void reCreate(const Device& device, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

            void setData(const void* data, uint64_t size, uint64_t offset = 0);

            VkBuffer getVulkanBuffer() const;

            void destroy();
            
            ~Buffer();

        private:

            uint32_t findSuitableMemory(uint32_t typeFilter, VkMemoryPropertyFlags properties);
            void create();

            const Device* _device;

            uint64_t _size;
            VkBufferUsageFlags _usage;
            VkMemoryPropertyFlags _properties;

            VkBuffer _buffer;
            VkDeviceMemory _deviceMemory;
    };
}
