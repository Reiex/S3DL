#include <S3DL/S3DL.hpp>

namespace s3dl
{
    unsigned int Instance::INSTANCE_COUNT = 0;

    Instance::Instance(const std::set<std::string>& additionalExtensions, const std::set<std::string>& additionalValidationLayers)
    {
        std::vector<const char*> extensions = {};
        #ifndef NDEBUG
        std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        #else
        std::vector<const char*> validationLayers = {};
        #endif

        if (INSTANCE_COUNT == 0)
            glfwInit();
        INSTANCE_COUNT++;

        // General informations on the vulkan application

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        appInfo.pApplicationName = "S3DL Application";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.pApplicationInfo = &appInfo;

        // Extensions

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (int i(0); i < glfwExtensionCount; i++)
            extensions.push_back(glfwExtensions[i]);

        for (const std::string& extension: additionalExtensions)
            extensions.push_back(extension.c_str());

        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Instance extensions required: ";
        for (int i(0); i < extensions.size(); i++)
            std::clog << extensions[i] << (i == extensions.size()-1 ? ".": ", ");
        std::clog << std::endl;
        #endif

        // Validation layers

        for (const std::string& layer: additionalValidationLayers)
            validationLayers.push_back(layer.c_str());

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        for (int i(0); i < validationLayers.size(); i++)
        {
            bool layerFound(false);
            for (int j(0); j < layerCount; j++)
            {
                if (std::strcmp(validationLayers[i], availableLayers[j].layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
                throw std::runtime_error("Validation layer '" + std::string(validationLayers[i]) + "' not found in available validation layers.");
        }

        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Validation layers required: ";
        for (int i(0); i < validationLayers.size(); i++)
            std::clog << validationLayers[i] << (i == validationLayers.size()-1 ? ".": ", ");
        std::clog << std::endl;
        #endif

        // Instance creation itself

        VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create VkInstance. VkResult: " + std::to_string(result));
        
        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkInstance successfully created." << std::endl;
        #endif
    }

    VkInstance Instance::getVulkanInstance() const
    {
        return _instance;
    }

    Instance::~Instance()
    {
        vkDestroyInstance(_instance, nullptr);

        INSTANCE_COUNT--;
        if (INSTANCE_COUNT == 0)
            glfwTerminate();
        
        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkInstance successfully destroyed." << std::endl;
        #endif
    }
}
