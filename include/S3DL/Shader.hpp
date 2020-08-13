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

            Shader(const std::string& fragment);
            Shader(const std::string& vertex, const std::string& fragment);
            Shader(const std::string& vertex, const std::string& geometry, const std::string& fragment);
            Shader(const std::string& vertex, const std::string& geometry, const std::string& tessControl, const std::string& tessEval, const std::string& fragment);

            const std::vector<VkPipelineShaderStageCreateInfo>& getVulkanShaderStages() const;

            ~Shader();

        private:

            static VkShaderModule getShaderModule(const std::string& filename, const std::string& shaderType);

            std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
    };
}
