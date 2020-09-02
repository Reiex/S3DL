#pragma once

#include <stdexcept>
#include <vector>
#include <cstdint>
#include <unordered_map>

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

            void declareGlobalUniform(uint32_t binding, uint32_t size, uint32_t count = 1, VkShaderStageFlags shaderStage = VK_SHADER_STAGE_ALL_GRAPHICS);
            void declareDrawablesUniform(uint32_t binding, uint32_t size, uint32_t count = 1, VkShaderStageFlags shaderStage = VK_SHADER_STAGE_ALL_GRAPHICS);

            void lock(const Swapchain& swapchain);
            void unlock();

            template<typename T>
            void setGlobalUniforms(uint32_t binding, T* values, uint32_t count = 1, uint32_t startIndex = 0);

            VkPipelineLayout getVulkanPipelineLayout() const;

            ~PipelineLayout();

        private:

            static const uint32_t DESCRIPTOR_POOL_SIZE = UINT16_MAX;

            PipelineLayout();

            void globalUpdate(const Swapchain& swapchain);  // Au bind d'une pipeline
            void drawableUpdate(const Drawable& drawable, const Swapchain& swapchain);  // Au draw d'un mesh
            void bind(const Drawable& drawable, const Swapchain& swapchain);  // Au draw d'un mesh

            void createVulkanDescriptorPool();  // A l'initialisation
            void createVulkanGlobalDescriptorSetLayout();  // Au lock
            void createVulkanDrawablesDescriptorSetLayout();  // Au lock
            void createVulkanPipelineLayout();  // Au lock
            void createVulkanGlobalDescriptorSets(const Swapchain& swapchain);  // Au lock
            void createGlobalBuffers(const Swapchain& swapchain);  // Au lock
            void createVulkanDrawablesDescriptorSets(const Drawable& drawable, const Swapchain& swapchain);  // A la declaration d'un nouveau mesh
            void createDrawablesBuffers(const Drawable& drawable, const Swapchain& swapchain);  // A la declaration d'un nouveau mesh

            void destroyVulkanDescriptorPool();
            void destroyVulkanGlobalDescriptorSetLayout();
            void destroyVulkanDrawablesDescriptorSetLayout();
            void destroyVulkanPipelineLayout();
            void destroyVulkanGlobalDescriptorSets();
            void destroyGlobalBuffers();
            void destroyVulkanDrawablesDescriptorSets();
            void destroyDrawablesBuffers();

            std::vector<DescriptorSetLayoutBindingState> _globalBindings;
            std::vector<DescriptorSetLayoutBindingState> _drawablesBindings;

            std::vector<VkDescriptorSetLayoutBinding> _globalBindingsLayouts;
            std::vector<VkDescriptorSetLayoutBinding> _drawablesBindingsLayouts;

            VkDescriptorSetLayout _vulkanGlobalSetLayout;
            VkDescriptorSetLayout _vulkanDrawablesSetLayout;

            VkPipelineLayoutCreateInfo _pipelineLayout;
            VkPipelineLayout _vulkanPipelineLayout;

            bool _locked;

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
