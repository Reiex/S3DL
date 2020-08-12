#include <S3DL/S3DL.hpp>

namespace s3dl
{
    PhysicalDevice::PhysicalDevice(VkPhysicalDevice handle) :
        _handle(handle)
    {
    }

    VkPhysicalDevice PhysicalDevice::getVulkanPhysicalDevice() const
    {
        return _handle;
    }

    const Device* Device::Active = nullptr;

    std::vector<PhysicalDevice> Device::getAvailablePhysicalDevices(const RenderTarget& target)
    {
        // Get list of all physical devices

        uint32_t deviceCount = 0;
        VkResult result = vkEnumeratePhysicalDevices(Instance::Active->getVulkanInstance(), &deviceCount, nullptr);
        if (result != VK_SUCCESS || deviceCount == 0)
            throw std::runtime_error("Failed to find GPUs with Vulkan support. VkResult: " + std::to_string(result));

        std::vector<VkPhysicalDevice> vulkanPhysicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(Instance::Active->getVulkanInstance(), &deviceCount, vulkanPhysicalDevices.data());

        std::vector<PhysicalDevice> physicalDevices;

        // Get physical devices properties

        for (int i(0); i < deviceCount; i++)
        {
            VkPhysicalDevice vulkanDevice(vulkanPhysicalDevices[i]);
            PhysicalDevice device(vulkanDevice);

            // General properties
            vkGetPhysicalDeviceProperties(vulkanDevice, &device.properties);

            // General features
            vkGetPhysicalDeviceFeatures(vulkanDevice, &device.features);

            // Extensions support
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(vulkanDevice, nullptr, &extensionCount, nullptr);
            device.extensions.resize(extensionCount);
            vkEnumerateDeviceExtensionProperties(vulkanDevice, nullptr, &extensionCount, device.extensions.data());

            // Queue families support
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(vulkanDevice, &queueFamilyCount, nullptr);
            device.queueFamilies.resize(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(vulkanDevice, &queueFamilyCount, device.queueFamilies.data());

            if (target.hasVulkanSurface())
            {
                // Swap chain capabilities
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkanDevice, target.getVulkanSurface(), &device.swapSupport.capabilities);

                // Swap chain formats support
                uint32_t formatCount;
                vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanDevice, target.getVulkanSurface(), &formatCount, nullptr);
                if (formatCount != 0)
                {
                    device.swapSupport.formats.resize(formatCount);
                    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanDevice, target.getVulkanSurface(), &formatCount, device.swapSupport.formats.data());
                }

                // Swap chain present modes support
                uint32_t presentModeCount;
                vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanDevice, target.getVulkanSurface(), &presentModeCount, nullptr);
                if (presentModeCount != 0)
                {
                    device.swapSupport.presentModes.resize(presentModeCount);
                    vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanDevice, target.getVulkanSurface(), &presentModeCount, device.swapSupport.presentModes.data());
                }
            }

            // Memory properties
            vkGetPhysicalDeviceMemoryProperties(vulkanDevice, &device.memoryProperties);

            physicalDevices.push_back(device);
        }

        return physicalDevices;
    }

    Device::Device(const RenderTarget& target, const std::set<std::string>& additionalExtensions)
    {
        std::vector<PhysicalDevice> availables = getAvailablePhysicalDevices(target);
        create(target, availables[0], additionalExtensions);
    }

    Device::Device(const RenderTarget& target, const PhysicalDevice& physicalDevice, const std::set<std::string>& additionalExtensions)
    {
        create(target, physicalDevice, additionalExtensions);
    }

    void Device::setActive() const
    {
        Active = this;
    }

    const PhysicalDevice& Device::getPhysicalDevice() const
    {
        return _physicalDevice;
    }

    VkDevice Device::getVulkanDevice() const
    {
        return _device;
    }

    VkQueue Device::getVulkanGraphicsQueue() const
    {
        return _graphicsQueue;
    }

    VkQueue Device::getVulkanPresentQueue() const
    {
        return _presentQueue;
    }

    VkCommandPool Device::getVulkanCommandPool() const
    {
        return _commandPool;
    }

    Device::~Device()
    {
        vkDestroyCommandPool(_device, _commandPool, nullptr);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkCommandPool successfully destroyed." << std::endl;
        #endif

        vkDestroyDevice(_device, nullptr);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkDevice successfully destroyed." << std::endl;
        #endif
    }

    namespace
    {
        struct QueueFamilies
        {
            uint32_t graphicsFamily;
            uint32_t presentFamily;

            bool hasGraphicsFamily;
            bool hasPresentFamily;
        };
    }

    void Device::create(const RenderTarget& target, const PhysicalDevice& physicalDevice, const std::set<std::string>& additionalExtensions)
    {
        setActive();

        _physicalDevice = physicalDevice;

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Creating logical device from physical device: " << _physicalDevice.properties.deviceName << std::endl;
        #endif

        std::set<std::string> extensions(additionalExtensions.begin(), additionalExtensions.end());
        extensions.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        // Extract indices of the different queue families that can be needed

        QueueFamilies families;
        for (int i(0); i < _physicalDevice.queueFamilies.size(); i++)
        {
            if (_physicalDevice.queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                families.graphicsFamily = i;
                families.hasGraphicsFamily = true;
            }

            if (target.hasVulkanSurface())
            {
                VkBool32 support = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice.getVulkanPhysicalDevice(), i, target.getVulkanSurface(), &support);
                if (support)
                {
                    families.presentFamily = i;
                    families.hasPresentFamily = true;
                }
            }
        }

        if (!families.hasGraphicsFamily || (!families.hasPresentFamily && target.hasVulkanSurface()))
            throw std::runtime_error("Could not find all vulkan queue families required.");

        // Compute the different create infos for the queues

        float queuePriority = 1.0f;

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> queueFamilies = { families.graphicsFamily, families.presentFamily };

        for (std::set<uint32_t>::iterator it = queueFamilies.begin(); it != queueFamilies.end(); it++)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};

            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.pNext = nullptr;
            queueCreateInfo.flags = 0;
            queueCreateInfo.queueFamilyIndex = *it;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        // Create the device itself

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        std::vector<const char*> deviceExtensions;
        for (const std::string& extension: extensions)
            deviceExtensions.push_back(extension.c_str());

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.enabledExtensionCount = deviceExtensions.size();
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        VkResult result = vkCreateDevice(_physicalDevice.getVulkanPhysicalDevice(), &createInfo, nullptr, &_device);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create logical device. VkResult: " + std::to_string(result));
        
        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkDevice successfully created." << std::endl;
        #endif

        // Extract the queues

        vkGetDeviceQueue(_device, families.graphicsFamily, 0, &_graphicsQueue);
        vkGetDeviceQueue(_device, families.presentFamily, 0, &_presentQueue);

        // Create the command pool

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = families.graphicsFamily;
        poolInfo.flags = 0;

        result = vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create command pool. VkResult: " + std::to_string(result));
        
        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkCommandPool successfully created." << std::endl;
        #endif
    }
}
