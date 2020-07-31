#include <S3DL/S3DL.hpp>

namespace s3dl
{
    const VkPhysicalDevice& PhysicalDevice::getVulkanPhysicalDevice() const
    {
        return _handle;
    }

    std::set<std::string> Device::INSTANCE_EXTENSIONS = { };
    std::set<std::string> Device::VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };

    VkInstance Device::_VK_INSTANCE;
    unsigned int Device::_DEVICE_COUNT = 0;

    Device::Device()
    {
        if (_DEVICE_COUNT == 0)
        {
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            createInstance();
        }
        
        _DEVICE_COUNT++;
    }

    const VkInstance& Device::getVulkanInstance()
    {
        return _VK_INSTANCE;
    }

    void Device::setBestAvailablePhysicalDevice(const RenderTarget& target)
    {
        std::vector<PhysicalDevice> availables = getAvailablePhysicalDevices(target);

        setPhysicalDevice(availables[0]);
    }

    std::vector<PhysicalDevice> Device::getAvailablePhysicalDevices(const RenderTarget& target) const
    {
        // Get list of all physical devices

        uint32_t deviceCount = 0;
        VkResult result = vkEnumeratePhysicalDevices(_VK_INSTANCE, &deviceCount, nullptr);
        if (result != VK_SUCCESS || deviceCount == 0)
            throw std::runtime_error("Failed to find GPUs with Vulkan support. VkResult: " + std::to_string(result));

        std::vector<VkPhysicalDevice> vulkanPhysicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(_VK_INSTANCE, &deviceCount, vulkanPhysicalDevices.data());

        std::vector<PhysicalDevice> physicalDevices;

        // Get physical devices properties

        for (int i(0); i < deviceCount; i++)
        {
            VkPhysicalDevice vulkanDevice(vulkanPhysicalDevices[i]);
            PhysicalDevice device;

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

            physicalDevices.push_back(device);
        }

        return physicalDevices;
    }

    void Device::setPhysicalDevice(const PhysicalDevice& device)
    {
        _physicalDeviceProperties = device;
        _physicalDevice = device.getVulkanPhysicalDevice();
    }

    Device::~Device()
    {
        _DEVICE_COUNT--;

        if (_DEVICE_COUNT == 0)
        {
            destroyInstance();
            glfwTerminate();
        }
    }

    void Device::createInstance()
    {
        // General informations on the vulkan application

        VkApplicationInfo appInfo;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        appInfo.pApplicationName = "S3DL Application";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.pApplicationInfo = &appInfo;

        // Extensions

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (int i(0); i < glfwExtensionCount; i++)
            INSTANCE_EXTENSIONS.insert(glfwExtensions[i]);

        std::vector<const char*> extensions;
        for (std::set<std::string>::iterator it(INSTANCE_EXTENSIONS.begin()); it != INSTANCE_EXTENSIONS.end(); it++)
            extensions.push_back(it->c_str());

        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        // Validation layers (activated only if in debug mode)

        #ifndef NDEBUG

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (std::set<std::string>::iterator it(VALIDATION_LAYERS.begin()); it != VALIDATION_LAYERS.end(); it++)
        {
            bool layerFound(false);
            for (int i(0); i < layerCount; i++)
            {
                if (*it == std::string(availableLayers[i].layerName))
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
                throw std::runtime_error("Validation layer '" + *it + "' not found in available validation layers.");
        }

        std::vector<const char*> validationLayers;
        for (std::set<std::string>::iterator it(VALIDATION_LAYERS.begin()); it != VALIDATION_LAYERS.end(); it++)
            validationLayers.push_back(it->c_str());

        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();

        #else

        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

        #endif

        // Instance creation itself

        VkResult result = vkCreateInstance(&createInfo, nullptr, &_VK_INSTANCE);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create VkInstance. VkResult: " + std::to_string(result));
        
        #ifndef NDEBUG
        std::clog << "VkInstance successfully created." << std::endl;
        #endif
    }

    void Device::destroyInstance()
    {
        vkDestroyInstance(_VK_INSTANCE, nullptr);
    }
}
