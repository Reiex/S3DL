#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Shader::Shader(const std::string& fragment)
    {
        VkPipelineShaderStageCreateInfo shaderStage{};
        shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStage.pName = "main";
        shaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStage.module = getShaderModule(fragment, "Fragment");
        _shaderStages.push_back(shaderStage);
    }

    Shader::Shader(const std::string& vertex, const std::string& fragment)
    {
        VkPipelineShaderStageCreateInfo shaderStage{};
        shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStage.pName = "main";

        shaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStage.module = getShaderModule(vertex, "Vertex");
        _shaderStages.push_back(shaderStage);

        shaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStage.module = getShaderModule(fragment, "Fragment");
        _shaderStages.push_back(shaderStage);
    }

    Shader::Shader(const std::string& vertex, const std::string& geometry, const std::string& fragment)
    {
        VkPipelineShaderStageCreateInfo shaderStage{};
        shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStage.pName = "main";

        shaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStage.module = getShaderModule(vertex, "Vertex");
        _shaderStages.push_back(shaderStage);

        shaderStage.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        shaderStage.module = getShaderModule(geometry, "Geometry");
        _shaderStages.push_back(shaderStage);

        shaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStage.module = getShaderModule(fragment, "Fragment");
        _shaderStages.push_back(shaderStage);
    }

    Shader::Shader(const std::string& vertex, const std::string& geometry, const std::string& tessControl, const std::string& tessEval, const std::string& fragment)
    {
        VkPipelineShaderStageCreateInfo shaderStage{};
        shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStage.pName = "main";

        shaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStage.module = getShaderModule(vertex, "Vertex");
        _shaderStages.push_back(shaderStage);

        shaderStage.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        shaderStage.module = getShaderModule(tessControl, "Tessellation control");
        _shaderStages.push_back(shaderStage);

        shaderStage.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        shaderStage.module = getShaderModule(tessEval, "Tessellation evaluation");
        _shaderStages.push_back(shaderStage);

        shaderStage.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        shaderStage.module = getShaderModule(geometry, "Geometry");
        _shaderStages.push_back(shaderStage);

        shaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStage.module = getShaderModule(fragment, "Fragment");
        _shaderStages.push_back(shaderStage);
    }

    const std::vector<VkPipelineShaderStageCreateInfo>& Shader::getVulkanShaderStages() const
    {
        return _shaderStages;
    }

    void Shader::destroy(const Device& device)
    {
        for (int i(0); i < _shaderStages.size(); i++)
            vkDestroyShaderModule(device.getVulkanDevice(), _shaderStages[i].module, nullptr);

        _shaderStages.clear();
    }

    VkShaderModule Shader::getShaderModule(const std::string& filename, const std::string& shaderType)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
            throw std::runtime_error("Failed to open shader source " + filename + ".");

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = buffer.size();
        createInfo.pCode = (const uint32_t*) (buffer.data());

        VkShaderModule shaderModule;
        VkResult result = vkCreateShaderModule(Device::Active->getVulkanDevice(), &createInfo, nullptr, &shaderModule);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create shader module. VkResult: " + std::to_string(result));
        
        #ifndef NDEBUG
        std::clog << "<S3DL Debug> " << shaderType << " shader '" << filename << "' successfully compiled." << std::endl;  
        #endif
        
        return shaderModule;
    }
}
