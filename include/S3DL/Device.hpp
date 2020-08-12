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
    struct PhysicalDevice
    {
        public:

            struct SwapChainSupportDetails
            {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector<VkSurfaceFormatKHR> formats;
                std::vector<VkPresentModeKHR> presentModes;
            };

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

    class Device
    {
        public:

            static std::vector<PhysicalDevice> getAvailablePhysicalDevices(const RenderTarget& target);

            static const Device* Active;

            Device(const RenderTarget& target, const std::set<std::string>& additionalExtensions = {});
            Device(const RenderTarget& target, const PhysicalDevice& physicalDevice, const std::set<std::string>& additionalExtensions = {});
            Device(const Device& device) = delete;

            Device& operator=(const Device& device) = delete;

            void setActive() const;

            const PhysicalDevice& getPhysicalDevice() const;

            VkDevice getVulkanDevice() const;
            VkQueue getVulkanGraphicsQueue() const;
            VkQueue getVulkanPresentQueue() const;
            VkCommandPool getVulkanCommandPool() const;

            ~Device();

        private:

            void create(const RenderTarget& target, const PhysicalDevice& physicalDevice, const std::set<std::string>& additionalExtensions);

            PhysicalDevice _physicalDevice;
            VkDevice _device;
            VkQueue _graphicsQueue;
            VkQueue _presentQueue;
            VkCommandPool _commandPool;
    };
}
