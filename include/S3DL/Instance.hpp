#pragma once

#include <set>
#include <vector>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Instance
    {
        public:

            static const Instance* Active;

            Instance(const std::set<std::string>& additionalExtensions = {}, const std::set<std::string>& additionalValidationLayers = {});
            Instance(const Instance& instance) = delete;

            Instance& operator=(const Instance& instance) = delete;

            VkInstance getVulkanInstance() const;

            ~Instance();

        private:

            static unsigned int INSTANCE_COUNT;

            VkInstance _instance;
    };
}
