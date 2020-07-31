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
    namespace Instance
    {
        extern std::set<std::string> EXTENSIONS;
        extern std::set<std::string> VALIDATION_LAYERS;

        void create();
        void destroy();

        VkInstance getVulkanInstance();
    }
}
