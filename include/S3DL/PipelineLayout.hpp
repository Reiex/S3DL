#pragma once

#include <stdexcept>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <cstring>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    struct DescriptorSetLayoutBindingState
    {
        uint32_t size;
        uint32_t count;
        uint32_t offset;
    };

    class PipelineLayout
    {
        public:

            PipelineLayout(const PipelineLayout& layout) = delete;
            PipelineLayout& operator=(const PipelineLayout& layout) = delete;

            void declareGlobalUniform(uint32_t binding, uint32_t size, VkShaderStageFlags shaderStage = VK_SHADER_STAGE_ALL_GRAPHICS);
            void declareGlobalUniformArray(uint32_t binding, uint32_t size, uint32_t count, VkShaderStageFlags shaderStage = VK_SHADER_STAGE_ALL_GRAPHICS);
            void declareDrawablesUniform(uint32_t binding, uint32_t size, VkShaderStageFlags shaderStage = VK_SHADER_STAGE_ALL_GRAPHICS);
            void declareDrawablesUniformArray(uint32_t binding, uint32_t size, uint32_t count, VkShaderStageFlags shaderStage = VK_SHADER_STAGE_ALL_GRAPHICS);

            void lock(const Swapchain& swapchain);
            void unlock();

            template<typename T>
            void setGlobalUniform(uint32_t binding, const T& value);
            template<typename T>
            void setGlobalUniformArray(uint32_t binding, const T* values, uint32_t count, uint32_t startIndex = 0);
            template<typename T>
            void setDrawablesUniform(const Drawable& drawable, uint32_t binding, const T& value);
            template<typename T>
            void setDrawablesUniformArray(const Drawable& drawable, uint32_t binding, const T* values, uint32_t count, uint32_t startIndex = 0);

            VkPipelineLayout getVulkanPipelineLayout() const;

            ~PipelineLayout();

        private:

            static const uint32_t DESCRIPTOR_POOL_SIZE = UINT16_MAX;

            PipelineLayout();

            void globalUpdate(const Swapchain& swapchain);
            void drawableUpdate(const Drawable& drawable, const Swapchain& swapchain);
            void bind(const Drawable& drawable, const Swapchain& swapchain);

            void createVulkanDescriptorPool();
            void createVulkanGlobalDescriptorSetLayout();
            void createVulkanDrawablesDescriptorSetLayout();
            void createVulkanPipelineLayout();
            void createVulkanGlobalDescriptorSets();
            void createGlobalBuffers();
            void createVulkanDrawablesDescriptorSets(const Drawable& drawable);
            void createDrawablesBuffers(const Drawable& drawable);

            void destroyVulkanDescriptorPool();
            void destroyVulkanGlobalDescriptorSetLayout();
            void destroyVulkanDrawablesDescriptorSetLayout();
            void destroyVulkanPipelineLayout();
            void destroyVulkanGlobalDescriptorSets();
            void destroyGlobalBuffers();
            void destroyVulkanDrawablesDescriptorSets(const Drawable& drawable);
            void destroyDrawablesBuffers(const Drawable& drawable);

            void computeBuffersOffsets();
            void computeUpdateNeeds();

            void addDrawable(const Drawable& drawable);

            std::vector<DescriptorSetLayoutBindingState> _globalBindings;
            std::vector<std::vector<bool>> _globalNeedsUpdate;
            std::vector<DescriptorSetLayoutBindingState> _drawablesBindings;
            std::vector<std::unordered_map<const Drawable*, std::vector<bool>>> _drawablesNeedsUpdate;

            std::vector<VkDescriptorSetLayoutBinding> _globalBindingsLayouts;
            std::vector<VkDescriptorSetLayoutBinding> _drawablesBindingsLayouts;

            VkDescriptorSetLayout _vulkanGlobalSetLayout;
            VkDescriptorSetLayout _vulkanDrawablesSetLayout;

            VkPipelineLayoutCreateInfo _pipelineLayout;
            VkPipelineLayout _vulkanPipelineLayout;

            bool _locked;
            uint32_t _swapchainImageCount;

            std::vector<uint8_t> _globalData;
            std::unordered_map<const Drawable*, std::vector<uint8_t>> _drawablesData;
            uint32_t _alignment;

            VkDescriptorPoolSize _descriptorPoolSize;
            VkDescriptorPoolCreateInfo _descriptorPool;
            VkDescriptorPool _vulkanDescriptorPool;

            std::vector<VkDescriptorSet> _vulkanGlobalDescriptorSets;
            std::unordered_map<const Drawable*, std::vector<VkDescriptorSet>> _vulkanDrawablesDescriptorSets;
            std::vector<Buffer*> _globalBuffers;
            std::unordered_map<const Drawable*, std::vector<Buffer*>> _drawablesBuffers;

        friend RenderTarget;
        friend Pipeline;
    };
}

#include <S3DL/PipelineLayoutT.hpp>
