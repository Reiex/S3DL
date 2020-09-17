#include <S3DL/S3DL.hpp>

namespace s3dl
{
    namespace
    {
        VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
        {
            if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT && messageType != VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
                std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

            return VK_FALSE;
        }
    }

    unsigned int Instance::INSTANCE_COUNT = 0;
    const Instance* Instance::Active = nullptr;

    Instance::Instance(const std::set<std::string>& additionalExtensions, const std::set<std::string>& additionalValidationLayers) :
        _instance(VK_NULL_HANDLE),
        _debugMessenger(VK_NULL_HANDLE)
    {
        #ifndef NDEBUG
        std::set<const char*> extensions = { "VK_EXT_debug_utils" };
        std::set<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        #else
        std::set<const char*> extensions = {};
        std::set<const char*> validationLayers = {};
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
        appInfo.apiVersion = VK_API_VERSION_1_1;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.pApplicationInfo = &appInfo;

        // Extensions

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (int i(0); i < glfwExtensionCount; i++)
            extensions.insert(glfwExtensions[i]);

        for (const std::string& extension: additionalExtensions)
            extensions.insert(extension.c_str());

        std::vector<const char*> extensionsVector(extensions.begin(), extensions.end());

        createInfo.enabledExtensionCount = extensionsVector.size();
        createInfo.ppEnabledExtensionNames = extensionsVector.data();

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Instance extensions required: ";
        for (int i(0); i < extensionsVector.size(); i++)
            std::clog << extensionsVector[i] << (i == extensionsVector.size()-1 ? ".": ", ");
        std::clog << std::endl;
        #endif

        // Validation layers

        for (const std::string& layer: additionalValidationLayers)
            validationLayers.insert(layer.c_str());

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        for (const char* layer: validationLayers)
        {
            bool layerFound(false);
            for (int i(0); i < layerCount; i++)
            {
                if (std::strcmp(layer, availableLayers[i].layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
                throw std::runtime_error("Validation layer '" + std::string(layer) + "' not found in available validation layers.");
        }

        std::vector<const char*> validationLayersVector(validationLayers.begin(), validationLayers.end());

        createInfo.enabledLayerCount = validationLayersVector.size();
        createInfo.ppEnabledLayerNames = validationLayersVector.data();

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Validation layers required: ";
        for (int i(0); i < validationLayersVector.size(); i++)
            std::clog << validationLayersVector[i] << (i == validationLayersVector.size()-1 ? ".": ", ");
        std::clog << std::endl;
        #endif

        // Instance creation itself

        VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create VkInstance. VkResult: " + std::to_string(result));
        
        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkInstance successfully created." << std::endl;
        #endif

        #ifndef NDEBUG
        setDebugCallback(defaultDebugCallback, nullptr); 
        #endif
    }

    void Instance::setDebugCallback(PFN_vkDebugUtilsMessengerCallbackEXT callback, void* pUserData)
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = callback;
        createInfo.pUserData = pUserData;

        PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");
        if (CreateDebugUtilsMessengerEXT == nullptr)
            throw std::runtime_error("VK_EXT_debug_utils extension not present.");
        
        VkResult result = CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger); 
        if (result != VK_SUCCESS) 
            throw std::runtime_error("Failed to create VkDebugUtilsMessengerEXT. VkResult: " + std::to_string(result));
    }

    void Instance::setActive() const
    {
        Active = this;
    }

    VkInstance Instance::getVulkanInstance() const
    {
        return _instance;
    }

    Instance::~Instance()
    {
        if (_debugMessenger != VK_NULL_HANDLE)
        {
            PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");
            DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
        }

        if (Active == this)
            Active = nullptr;

        vkDestroyInstance(_instance, nullptr);

        INSTANCE_COUNT--;
        if (INSTANCE_COUNT == 0)
            glfwTerminate();
        
        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkInstance successfully destroyed." << std::endl;
        #endif
    }
}
