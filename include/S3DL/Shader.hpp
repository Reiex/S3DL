#pragma once

#include <string>
#include <vector>
#include <fstream>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Shader
    {
        public:

            Shader() = default;
            Shader(const Device& device, const std::string& vertex, const std::string& fragment, const std::vector<std::string>& subFragments = {});
            Shader(const Device& device, const std::string& vertex, const std::string& geometry, const std::string& fragment, const std::vector<std::string>& subFragments = {});
            Shader(const Device& device, const std::string& vertex, const std::string& geometry, const std::string& tessControl, const std::string& tessEval, const std::string& fragment, const std::vector<std::string>& subFragments = {});

            const std::vector<VkPipelineShaderStageCreateInfo>& getVulkanShaderStages() const;

        private:

            static VkShaderModule getShaderModule(const Device& device, const std::string& filename);

            std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
    };
}
