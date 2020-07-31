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

            const VkPhysicalDevice& getVulkanPhysicalDevice() const;

            VkPhysicalDeviceProperties properties;
            VkPhysicalDeviceFeatures features;
            std::vector<VkExtensionProperties> extensions;
            std::vector<VkQueueFamilyProperties> queueFamilies;
            SwapChainSupportDetails swapSupport;
        
        private:

            VkPhysicalDevice _handle;
    };

    class Device
    {
        public:

            static std::set<std::string> INSTANCE_EXTENSIONS;
            static std::set<std::string> VALIDATION_LAYERS;

            Device();

            static const VkInstance& getVulkanInstance();

            void setBestAvailablePhysicalDevice(const RenderTarget& target);
            std::vector<PhysicalDevice> getAvailablePhysicalDevices(const RenderTarget& target) const;
            void setPhysicalDevice(const PhysicalDevice& device);

            ~Device();

        private:

            static void createInstance();
            static void destroyInstance();

            static VkInstance _VK_INSTANCE;
            static unsigned int _DEVICE_COUNT;

            PhysicalDevice _physicalDeviceProperties;
            VkPhysicalDevice _physicalDevice;
    };
}
