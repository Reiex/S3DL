#pragma once

#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class PipelineLayout
    {
        public:

            PipelineLayout(const PipelineLayout& layout) = delete;

            PipelineLayout& operator=(const PipelineLayout& layout) = delete;

            void declareUniform(unsigned int binding, unsigned int size);
            void declareUniformArray(unsigned int binding, unsigned int size, unsigned int count);

            void lock(const Swapchain& swapchain);
            void unlock();

            template<typename T>
            void setUniform(unsigned int binding, T value);
            template<typename T>
            void setUniformArray(unsigned int binding, T* values);
            template<typename T>
            void setUniformArrayElement(unsigned int binding, unsigned int index, T value);

            VkPipelineLayout getVulkanPipelineLayout() const;

            ~PipelineLayout();

        private:

            PipelineLayout();

            void destroyVulkanPipelineLayout() const;

            std::vector<VkDescriptorSetLayoutBinding> _descriptorSetLayoutBindings;

            std::vector<VkDescriptorSetLayoutCreateInfo> _descriptorSetLayouts;
            std::vector<VkDescriptorSetLayout> _vulkanDescriptorSetLayouts;

            VkPipelineLayoutCreateInfo _pipelineLayout;
            mutable VkPipelineLayout _vulkanPipelineLayout;
            mutable bool _vulkanPipelineLayoutComputed;

            bool _locked;

            std::vector<Buffer*> buffers;

            VkDescriptorPoolSize _descriptorPoolSize;
            VkDescriptorPoolCreateInfo _descriptorPool;
            VkDescriptorPool _vulkanDescriptorPool;

            VkDescriptorSetAllocateInfo _descriptorSets;
            std::vector<std::vector<VkDescriptorSet>> descriptorSets;

        friend Pipeline;
    };
}
