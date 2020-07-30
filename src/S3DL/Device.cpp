#include <S3DL/S3DL.hpp>

namespace s3dl
{
    std::set<std::string> Device::INSTANCE_EXTENSIONS;
    std::set<std::string> Device::VALIDATION_LAYERS;

    VkInstance Device::_VK_INSTANCE;
    std::set<Device*> Device::_DEVICE_LIST;

    Device::Device()
    {
        if (_DEVICE_LIST.size() == 0)
            createInstance();
        
        _DEVICE_LIST.insert(this);
    }

    std::vector<PhysicalDevice> Device::getPhysicalDevices() const
    {
        uint32_t deviceCount = 0;
        VkResult result = vkEnumeratePhysicalDevices(_VK_INSTANCE, &deviceCount, nullptr);
        if (result != VK_SUCCESS || deviceCount == 0)
            throw std::runtime_error("Failed to find GPUs with Vulkan support. Error code: " + std::to_string(result));

        std::vector<VkPhysicalDevice> vulkanPhysicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(_VK_INSTANCE, &deviceCount, vulkanPhysicalDevices.data());

        std::vector<PhysicalDevice> physicalDevices(deviceCount);

        for (int i(0); i < deviceCount; i++)
        {
            VkPhysicalDevice vulkanDevice(vulkanPhysicalDevices[i]);
            PhysicalDevice device;

            vkGetPhysicalDeviceProperties(vulkanDevice, &device.properties);
            vkGetPhysicalDeviceFeatures(vulkanDevice, &device.features);

            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(vulkanDevice, nullptr, &extensionCount, nullptr);
            device.extensions.resize(extensionCount);
            vkEnumerateDeviceExtensionProperties(vulkanDevice, nullptr, &extensionCount, device.extensions.data());

            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(vulkanDevice, &queueFamilyCount, nullptr);
            device.queueFamilies.resize(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(vulkanDevice, &queueFamilyCount, device.queueFamilies.data());

            // Swap Chain Support

            physicalDevices.push_back(device);
        }

        return physicalDevices;
    }

    Device::~Device()
    {
        std::set<Device*>::iterator position = _DEVICE_LIST.find(this);
        if (position != _DEVICE_LIST.end())
            _DEVICE_LIST.erase(position);

        if (_DEVICE_LIST.size() == 0)
            destroyInstance();
    }

    void Device::createInstance()
    {
        // General informations on the vulkan application

        VkApplicationInfo appInfo;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "S3DL Application";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
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

        VALIDATION_LAYERS.insert("VK_LAYER_KHRONOS_validation");

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
            throw std::runtime_error("Failed to create VkInstance. Error code: " + std::to_string(result));
        
        #ifndef NDEBUG
        std::clog << "VkInstance successfully created." << std::endl;
        #endif
    }

    void Device::destroyInstance()
    {
        vkDestroyInstance(_VK_INSTANCE, nullptr);
    }
}
