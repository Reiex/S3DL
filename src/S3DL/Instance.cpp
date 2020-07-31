#include <S3DL/S3DL.hpp>

namespace s3dl
{
    namespace Instance
    {
        namespace
        {
            VkInstance S3DL_VK_INSTANCE;
        }

        std::set<std::string> EXTENSIONS = { };
        std::set<std::string> VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };

        void create()
        {
            glfwInit();

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
                EXTENSIONS.insert(glfwExtensions[i]);

            std::vector<const char*> extensions;
            for (std::set<std::string>::iterator it(EXTENSIONS.begin()); it != EXTENSIONS.end(); it++)
                extensions.push_back(it->c_str());

            createInfo.enabledExtensionCount = extensions.size();
            createInfo.ppEnabledExtensionNames = extensions.data();

            #ifndef NDEBUG
            std::clog << "Instance extensions required: ";
            for (int i(0); i < extensions.size(); i++)
                std::clog << extensions[i] << (i == extensions.size()-1 ? ".": ", ");
            std::clog << std::endl;
            #endif

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

            std::clog << "Validation layers required: ";
            for (int i(0); i < validationLayers.size(); i++)
                std::clog << validationLayers[i] << (i == validationLayers.size()-1 ? ".": ", ");
            std::clog << std::endl;

            #else

            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;

            #endif

            // Instance creation itself

            VkResult result = vkCreateInstance(&createInfo, nullptr, &S3DL_VK_INSTANCE);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create VkInstance. VkResult: " + std::to_string(result));
            
            #ifndef NDEBUG
            std::clog << "VkInstance successfully created." << std::endl;
            #endif
        }
        
        void destroy()
        {
            vkDestroyInstance(S3DL_VK_INSTANCE, nullptr);
            glfwTerminate();
        }

        VkInstance getVulkanInstance()
        {
            return S3DL_VK_INSTANCE;
        }
    }
}
