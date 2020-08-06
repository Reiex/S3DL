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

    Device Device::createFromPhysicalDevice(const PhysicalDevice& physicalDevice, const RenderTarget& target)
    {
        Device device;

        device._physicalDeviceProperties = physicalDevice;
        device._physicalDevice = physicalDevice.getVulkanPhysicalDevice();

        device.create(target);

        return device;
    }

    Device Device::createBestPossible(const RenderTarget& target)
    {
        std::vector<PhysicalDevice> availables = getAvailablePhysicalDevices(target);

        #ifndef NDEBUG
        std::clog << availables.size() << " physical device(s) available(s):" << std::endl;
        for (int i(0); i < availables.size(); i++)
            std::clog << "- " << availables[i].properties.deviceName << std::endl;
        #endif

        // TODO: Less arbitrary way of chosing the device

        return createFromPhysicalDevice(availables[0], target);
    }

    std::vector<PhysicalDevice> Device::getAvailablePhysicalDevices(const RenderTarget& target)
    {
        // Get list of all physical devices

        uint32_t deviceCount = 0;
        VkResult result = vkEnumeratePhysicalDevices(Instance::getVulkanInstance(), &deviceCount, nullptr);
        if (result != VK_SUCCESS || deviceCount == 0)
            throw std::runtime_error("Failed to find GPUs with Vulkan support. VkResult: " + std::to_string(result));

        std::vector<VkPhysicalDevice> vulkanPhysicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(Instance::getVulkanInstance(), &deviceCount, vulkanPhysicalDevices.data());

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

            physicalDevices.push_back(device);
        }

        return physicalDevices;
    }

    std::vector<std::string> Device::EXTENSIONS = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    const PhysicalDevice& Device::getPhysicalDeviceProperties() const
    {
        return _physicalDeviceProperties;
    }

    VkDevice Device::getVulkanDevice() const
    {
        return _device;
    }

    VkCommandPool Device::getVulkanCommandPool() const
    {
        return _commandPool;
    }

    const DeviceQueues& Device::getVulkanQueues() const
    {
        return _queues;
    }

    void Device::destroy()
    {
        if (_commandPool != VK_NULL_HANDLE)
            vkDestroyCommandPool(_device, _commandPool, nullptr);
        _commandPool = VK_NULL_HANDLE;

        if (_device != VK_NULL_HANDLE)
            vkDestroyDevice(_device, nullptr);
        _device = VK_NULL_HANDLE;
    }

    Device::~Device()
    {
        destroy();
    }

    Device::Device() :
        _physicalDeviceProperties{},
        _physicalDevice(VK_NULL_HANDLE),
        _device(VK_NULL_HANDLE),
        _queues{},
        _commandPool(VK_NULL_HANDLE)
    {
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

    void Device::create(const RenderTarget& target)
    {
        #ifndef NDEBUG
        std::clog << "Physical device chosen: " << _physicalDeviceProperties.properties.deviceName << std::endl;
        #endif

        // Extract indices of the different queue families that can be needed

        QueueFamilies families;
        for (int i(0); i < _physicalDeviceProperties.queueFamilies.size(); i++)
        {
            if (_physicalDeviceProperties.queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                families.graphicsFamily = i;
                families.hasGraphicsFamily = true;
            }

            if (target.hasVulkanSurface())
            {
                VkBool32 support = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, target.getVulkanSurface(), &support);
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

        std::vector<const char*> deviceExtensions;
        for (int i(0); i < EXTENSIONS.size(); i++)
            deviceExtensions.push_back(EXTENSIONS[i].c_str());

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = 0;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.enabledExtensionCount = deviceExtensions.size();
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        VkResult result = vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create logical device. VkResult: " + std::to_string(result));
        
        #ifndef NDEBUG
        std::clog << "VkDevice successfully created." << std::endl;
        #endif

        // Extract the queues

        vkGetDeviceQueue(_device, families.graphicsFamily, 0, &_queues.graphicsQueue);
        vkGetDeviceQueue(_device, families.presentFamily, 0, &_queues.presentQueue);

        // Create the command pool

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = families.graphicsFamily;
        poolInfo.flags = 0;

        result = vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create command pool. VkResult: " + std::to_string(result));
        
        #ifndef NDEBUG
        std::clog << "VkCommandPool successfully created." << std::endl;
        #endif
    }
}
