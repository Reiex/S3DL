#pragma once

#include <stdexcept>
#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    struct DescriptorSetLayoutBindingState
    {
        uint32_t size;
        uint32_t count;
        uint32_t offset;
        bool needsUpdate;
    };

    class PipelineLayout
    {
        public:

            PipelineLayout(const PipelineLayout& layout) = delete;

            PipelineLayout& operator=(const PipelineLayout& layout) = delete;

            void declareUniform(uint32_t binding, uint32_t size, uint32_t set = 0, VkShaderStageFlags shaderStage = VK_SHADER_STAGE_ALL_GRAPHICS);
            void declareUniformArray(uint32_t binding, uint32_t elementSize, uint32_t count, uint32_t set = 0, VkShaderStageFlags shaderStage = VK_SHADER_STAGE_ALL_GRAPHICS);

            void lock(const Swapchain& swapchain);
            void unlock();

            template<typename T>
            void setUniform(uint32_t binding, T value, uint32_t set = 0);
            template<typename T>
            void setUniformArray(uint32_t binding, T* values, uint32_t set = 0);
            template<typename T>
            void setUniformArrayElement(uint32_t binding, uint32_t index, T value, uint32_t set = 0);

            VkPipelineLayout getVulkanPipelineLayout();

            ~PipelineLayout();

        private:

            PipelineLayout();

            void bind(const Swapchain& swapchain, unsigned int index);

            void createVulkanDescriptorSetLayouts();
            void createVulkanPipelineLayout();
            void createVulkanDescriptorPool(const Swapchain& swapchain);
            void createVulkanDescriptorSets(const Swapchain& swapchain);
            void createBuffers(const Swapchain& swapchain);

            void destroyVulkanDescriptorSetLayouts();
            void destroyVulkanPipelineLayout();
            void destroyVulkanDescriptorPool();
            void destroyVulkanDescriptorSets();
            void destroyBuffers();

            void extendDeclaredRange(uint32_t set, uint32_t binding);

            void computeBindingStates();
            void resetBindingStates();

            std::vector<std::vector<DescriptorSetLayoutBindingState>> _bindingStates;
            std::vector<std::vector<VkDescriptorSetLayoutBinding>> _descriptorSetLayoutBindings;
            std::vector<VkDescriptorSetLayout> _vulkanDescriptorSetLayouts;

            VkPipelineLayoutCreateInfo _pipelineLayout;
            VkPipelineLayout _vulkanPipelineLayout;
            bool _vulkanPipelineLayoutComputed;

            bool _locked;

            std::vector<uint8_t> _bufferData;
            std::vector<Buffer*> _buffers;

            VkDescriptorPoolSize _descriptorPoolSize;
            VkDescriptorPoolCreateInfo _descriptorPool;
            VkDescriptorPool _vulkanDescriptorPool;

            VkDescriptorSetAllocateInfo _descriptorSets;
            std::vector<std::vector<VkDescriptorSet>> _vulkanDescriptorSets;

        friend Pipeline;
    };
}
