#pragma once

#include <set>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct PhysicalDevice
    {
        public:

            PhysicalDevice() = default;
            PhysicalDevice(VkPhysicalDevice handle);
            VkPhysicalDevice getVulkanPhysicalDevice() const;

            VkPhysicalDeviceProperties properties;
            VkPhysicalDeviceFeatures features;
            std::vector<VkExtensionProperties> extensions;
            std::vector<VkQueueFamilyProperties> queueFamilies;
            SwapChainSupportDetails swapSupport;
            VkPhysicalDeviceMemoryProperties memoryProperties;
        
        private:

            VkPhysicalDevice _handle;
    };

    struct DeviceQueues
    {
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        bool hasGraphicsQueue;
        bool hasPresentQueue;
    };

    class Device
    {
        public:

            static Device createFromPhysicalDevice(const PhysicalDevice& physicalDevice, const RenderTarget& target);
            static Device createBestPossible(const RenderTarget& target);

            static std::vector<PhysicalDevice> getAvailablePhysicalDevices(const RenderTarget& target);

            static std::vector<std::string> EXTENSIONS;

            const PhysicalDevice& getPhysicalDeviceProperties() const;
            VkDevice getVulkanDevice() const;
            VkCommandPool getVulkanCommandPool() const;
            const DeviceQueues& getVulkanQueues() const;

            void updateProperties(const RenderTarget& target) const;

            void destroy();

        private:

            Device();

            void create(const RenderTarget& target);

            mutable PhysicalDevice _physicalDeviceProperties;
            VkPhysicalDevice _physicalDevice;

            VkDevice _device;

            DeviceQueues _queues;

            VkCommandPool _commandPool;
    };
}
