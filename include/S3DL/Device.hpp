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
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        std::vector<VkExtensionProperties> extensions;
        std::vector<VkQueueFamilyProperties> queueFamilies;
        SwapChainSupportDetails swapSupport;
    };

    class Device
    {
        public:

            static std::set<std::string> INSTANCE_EXTENSIONS;
            static std::set<std::string> VALIDATION_LAYERS;

            Device();

            std::vector<PhysicalDevice> getPhysicalDevices() const;

            ~Device();

        private:

            static VkInstance _VK_INSTANCE;
            static unsigned int _DEVICE_COUNT;

            static void createInstance();
            static void destroyInstance();
        
        friend class RenderWindow;
    };
}
