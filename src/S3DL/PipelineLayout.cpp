#include <S3DL/S3DL.hpp>

namespace s3dl
{
    void PipelineLayout::declareUniform(uint32_t binding, uint32_t size, uint32_t set, VkShaderStageFlags shaderStage)
    {
        declareUniformArray(binding, size, 1, set, shaderStage);
    }

    void PipelineLayout::declareUniformArray(uint32_t binding, uint32_t elementSize, uint32_t count, uint32_t set, VkShaderStageFlags shaderStage)
    {
        if (_locked)
            throw std::runtime_error("Cannot declare uniform while pipeline layout is locked.");
            
        extendDeclaredRange(set, binding);
        
        _bindings[set][binding].size = elementSize;
        _bindings[set][binding].count = count;
        
        _descriptorSetLayoutBindings[set][binding].descriptorCount = count;
        _descriptorSetLayoutBindings[set][binding].stageFlags = shaderStage;
    }
    
    void PipelineLayout::lock(const Swapchain& swapchain)
    {
        if (_bindings.size() > 0)
        {
            _alignment = Device::Active->getPhysicalDevice().properties.limits.minUniformBufferOffsetAlignment;
            computeBindingStates();

            createVulkanDescriptorSetLayouts();
            createVulkanPipelineLayout();
            createVulkanDescriptorPool(swapchain);
            createVulkanDescriptorSets(swapchain);
            createBuffers(swapchain);
        }

        _locked = true;
    }
    
    void PipelineLayout::unlock()
    {
        destroyBuffers();
        destroyVulkanDescriptorSets();
        destroyVulkanDescriptorPool();
        destroyVulkanPipelineLayout();
        destroyVulkanDescriptorSetLayouts();

        resetBindingStates();

        _locked = false;
    }
    
    VkPipelineLayout PipelineLayout::getVulkanPipelineLayout()
    {
        if (!_vulkanPipelineLayoutComputed)
            createVulkanPipelineLayout();
        
        return _vulkanPipelineLayout;
    }
    
    PipelineLayout::~PipelineLayout()
    {
        unlock();
    }
    
    PipelineLayout::PipelineLayout() :
        _vulkanPipelineLayout(VK_NULL_HANDLE),
        _vulkanPipelineLayoutComputed(false),
        _locked(false),
        _vulkanDescriptorPool(VK_NULL_HANDLE)
    {
        _pipelineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        _pipelineLayout.pNext = nullptr;
        _pipelineLayout.flags = 0;
        _pipelineLayout.setLayoutCount = 0;
        _pipelineLayout.pSetLayouts = nullptr;
        _pipelineLayout.pushConstantRangeCount = 0;
        _pipelineLayout.pPushConstantRanges = nullptr;

        _descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        _descriptorPoolSize.descriptorCount = 0;

        _descriptorPool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        _descriptorPool.pNext = nullptr;
        _descriptorPool.flags = 0;
        _descriptorPool.poolSizeCount = 1;
        _descriptorPool.pPoolSizes = &_descriptorPoolSize;
        _descriptorPool.maxSets = 0;

        _descriptorSets.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        _descriptorSets.pNext = nullptr;
        _descriptorSets.descriptorPool = VK_NULL_HANDLE;
        _descriptorSets.descriptorSetCount = 0;
        _descriptorSets.pSetLayouts = nullptr;
    }

    void PipelineLayout::update(const Swapchain& swapchain)
    {
        if (!_locked)
            throw std::runtime_error("Cannot draw using a pipeline while its pipeline layout is not locked.");

        if (_bindings.size() == 0)
            return;

        _buffers[swapchain.getCurrentImage()]->setData(_bufferData.data(), _bufferData.size());

        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkWriteDescriptorSet> descriptorWrites;
        for (int i(0); i < _bindings.size(); i++)
        {
            for (int j(0); j < _bindings[i].size(); j++)
            {
                if (_bindings[i][j].needsUpdate)
                {
                    VkDescriptorBufferInfo bufferInfo{};
                    bufferInfo.buffer = _buffers[swapchain.getCurrentImage()]->getVulkanBuffer();
                    bufferInfo.offset = _bindings[i][j].offset;
                    bufferInfo.range = _bindings[i][j].size;

                    bufferInfos.push_back(bufferInfo);
                }
            }
        }
        for (int i(0); i < _bindings.size(); i++)
        {
            for (int j(0); j < _bindings[i].size(); j++)
            {
                if (_bindings[i][j].needsUpdate)
                {
                    VkWriteDescriptorSet descriptorWrite{};
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = _vulkanDescriptorSets[swapchain.getCurrentImage()][i];
                    descriptorWrite.dstBinding = j;
                    descriptorWrite.dstArrayElement = 0;
                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    descriptorWrite.descriptorCount = _bindings[i][j].count;
                    descriptorWrite.pBufferInfo = &bufferInfos[descriptorWrites.size()];
                    descriptorWrite.pImageInfo = nullptr;
                    descriptorWrite.pTexelBufferView = nullptr;

                    descriptorWrites.push_back(descriptorWrite);
                }
            }
        }

        if (descriptorWrites.size() != 0)
            vkUpdateDescriptorSets(Device::Active->getVulkanDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }

    void PipelineLayout::bind(const Swapchain& swapchain)
    {
        if (!_locked)
            throw std::runtime_error("Cannot draw using a pipeline while its pipeline layout is not locked.");

        if (_bindings.size() == 0)
            return;

        vkCmdBindDescriptorSets(swapchain.getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _vulkanPipelineLayout, 0, _vulkanDescriptorSets[swapchain.getCurrentImage()].size(), _vulkanDescriptorSets[swapchain.getCurrentImage()].data(), 0, nullptr);
    }

    void PipelineLayout::createVulkanDescriptorSetLayouts()
    {
        destroyVulkanDescriptorSetLayouts();

        _vulkanDescriptorSetLayouts.resize(_descriptorSetLayoutBindings.size());

        for (int i(0); i < _vulkanDescriptorSetLayouts.size(); i++)
        {
            VkDescriptorSetLayoutCreateInfo createInfo{};

            createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            createInfo.pNext = nullptr;
            createInfo.flags = 0;
            createInfo.bindingCount = _descriptorSetLayoutBindings[i].size();
            createInfo.pBindings = _descriptorSetLayoutBindings[i].data();

            VkResult result = vkCreateDescriptorSetLayout(Device::Active->getVulkanDevice(), &createInfo, nullptr, &_vulkanDescriptorSetLayouts[i]);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create descriptor set layout. VkResult: " + std::to_string(result));
        }

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> " << std::to_string(_vulkanDescriptorSetLayouts.size()) << " VkDescriptorSetLayouts successfully created." << std::endl;
        #endif
    }

    void PipelineLayout::createVulkanPipelineLayout()
    {
        destroyVulkanPipelineLayout();

        _pipelineLayout.setLayoutCount = _vulkanDescriptorSetLayouts.size();
        _pipelineLayout.pSetLayouts = _vulkanDescriptorSetLayouts.size() != 0 ? _vulkanDescriptorSetLayouts.data(): nullptr;

        VkResult result = vkCreatePipelineLayout(Device::Active->getVulkanDevice(), &_pipelineLayout, nullptr, &_vulkanPipelineLayout);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create pipeline layout. VkResult: " + std::to_string(result));

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkPipelineLayout successfully created." << std::endl;
        #endif

        _vulkanPipelineLayoutComputed = true;
    }

    void PipelineLayout::createVulkanDescriptorPool(const Swapchain& swapchain)
    {
        destroyVulkanDescriptorPool();

        int setCount = _vulkanDescriptorSetLayouts.size() * swapchain.getImageCount();

        _descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        _descriptorPoolSize.descriptorCount = setCount;

        _descriptorPool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        _descriptorPool.pNext = nullptr;
        _descriptorPool.flags = 0;
        _descriptorPool.poolSizeCount = 1;
        _descriptorPool.pPoolSizes = &_descriptorPoolSize;
        _descriptorPool.maxSets = setCount;

        VkResult result = vkCreateDescriptorPool(Device::Active->getVulkanDevice(), &_descriptorPool, nullptr, &_vulkanDescriptorPool);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor pool. VkResult: " + std::to_string(result));

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkDescriptorPool successfully created." << std::endl;
        #endif
    }
    
    void PipelineLayout::createVulkanDescriptorSets(const Swapchain& swapchain)
    {
        destroyVulkanDescriptorSets();

        _vulkanDescriptorSets.resize(swapchain.getImageCount());

        _descriptorSets.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        _descriptorSets.descriptorPool = _vulkanDescriptorPool;
        _descriptorSets.descriptorSetCount = _vulkanDescriptorSetLayouts.size();
        _descriptorSets.pSetLayouts = _vulkanDescriptorSetLayouts.data();

        for (int i(0); i < swapchain.getImageCount(); i++)
        {
            _vulkanDescriptorSets[i].resize(_vulkanDescriptorSetLayouts.size());

            VkResult result = vkAllocateDescriptorSets(Device::Active->getVulkanDevice(), &_descriptorSets, _vulkanDescriptorSets[i].data());
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to allocate descriptor sets. VkResult: " + std::to_string(result));
        }
    }

    void PipelineLayout::createBuffers(const Swapchain& swapchain)
    {
        destroyBuffers();

        uint32_t n, m;
        n = _bindings.size() - 1;
        m = _bindings[n].size() - 1;
        uint32_t totalSize = _bindings[n][m].offset + _bindings[n][m].size * _bindings[n][m].count;
        
        _bufferData.resize(totalSize, 0);
        _buffers.resize(swapchain.getImageCount());

        for (int i(0); i < swapchain.getImageCount(); i++)
            _buffers[i] = new Buffer(totalSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
    
    void PipelineLayout::destroyVulkanDescriptorSetLayouts()
    {
        if (_vulkanDescriptorSetLayouts.size() != 0)
        {
            for (int i(0); i < _vulkanDescriptorSetLayouts.size(); i++)
                vkDestroyDescriptorSetLayout(Device::Active->getVulkanDevice(), _vulkanDescriptorSetLayouts[i], nullptr);

            #ifndef NDEBUG
            std::clog << "<S3DL Debug> " << std::to_string(_vulkanDescriptorSetLayouts.size()) << " VkDescriptorSetLayouts successfully destroyed." << std::endl;
            #endif
        }

        _vulkanDescriptorSetLayouts.clear();
    }

    void PipelineLayout::destroyVulkanPipelineLayout()
    {
        if (_vulkanPipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(Device::Active->getVulkanDevice(), _vulkanPipelineLayout, nullptr);

            #ifndef NDEBUG
            std::clog << "<S3DL Debug> VkPipelineLayout successfully destroyed." << std::endl;
            #endif
        }

        _vulkanPipelineLayoutComputed = false;
        _vulkanPipelineLayout = VK_NULL_HANDLE;
    }
    
    void PipelineLayout::destroyVulkanDescriptorPool()
    {
        if (_vulkanDescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(Device::Active->getVulkanDevice(), _vulkanDescriptorPool, nullptr);

            #ifndef NDEBUG
            std::clog << "<S3DL Debug> VkDescriptorPool successfully destroyed." << std::endl;
            #endif
        }

        _vulkanDescriptorPool = VK_NULL_HANDLE;
    }
    
    void PipelineLayout::destroyVulkanDescriptorSets()
    {
        _vulkanDescriptorSets.clear();
    }

    void PipelineLayout::destroyBuffers()
    {
        for (int i(0); i < _buffers.size(); i++)
            delete _buffers[i];
        
        _buffers.clear();
        _bufferData.clear();
    }

    void PipelineLayout::extendDeclaredRange(uint32_t set, uint32_t binding)
    {
        if (_bindings.size() <= set)
        {
            _bindings.resize(set + 1);
            _descriptorSetLayoutBindings.resize(set + 1);
        }
        
        if (_bindings[set].size() <= binding)
        {
            DescriptorSetLayoutBindingState bindingState;
            bindingState.size = 0;
            bindingState.count = 0;
            bindingState.offset = 0;
            bindingState.needsUpdate = true;
            _bindings[set].resize(binding + 1, bindingState);

            int n = _descriptorSetLayoutBindings[set].size();
            _descriptorSetLayoutBindings[set].resize(binding + 1);
            for (int i(n); i < binding + 1; i++)
            {
                VkDescriptorSetLayoutBinding emptyBinding{};
                emptyBinding.binding = i;
                emptyBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                emptyBinding.descriptorCount = 0;
                emptyBinding.stageFlags = 0;
                emptyBinding.pImmutableSamplers = nullptr;

                _descriptorSetLayoutBindings[set][i] = emptyBinding;
            }
        }
    }

    void PipelineLayout::computeBindingStates()
    {
        uint32_t offset = 0;
        for (int i(0); i < _bindings.size(); i++)
        {
            for (int j(0); j < _bindings[i].size(); j++)
            {
                _bindings[i][j].offset = offset;
                offset += _bindings[i][j].size * _bindings[i][j].count;
                offset += (_alignment - offset) % _alignment;
            }
        }
    }

    void PipelineLayout::resetBindingStates()
    {
        for (int i(0); i < _bindings.size(); i++)
            for (int j(0); j < _bindings[i].size(); j++)
                _bindings[i][j].needsUpdate = true;
    }
}
