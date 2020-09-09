#include <S3DL/S3DL.hpp>

namespace s3dl
{
    void PipelineLayout::declareGlobalUniform(uint32_t binding, uint32_t size, VkShaderStageFlags shaderStage)
    {
        declareGlobalUniformArray(binding, size, 1, shaderStage);
    }

    void PipelineLayout::declareGlobalUniformArray(uint32_t binding, uint32_t size, uint32_t count, VkShaderStageFlags shaderStage)
    {
        if (_locked)
            throw std::runtime_error("Cannot declare uniform while pipeline layout is locked.");
        
        int i(0);
        for (; i < _globalBindingsLayouts.size(); i++)
            if (_globalBindingsLayouts[i].binding == binding)
                break;
        
        if (i == _globalBindingsLayouts.size())
        {
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = binding;

            _globalBindingsLayouts.push_back(layoutBinding);
            _globalBindings.push_back(DescriptorSetLayoutBindingState{});
        }

        _globalBindingsLayouts[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        _globalBindingsLayouts[i].descriptorCount = count;
        _globalBindingsLayouts[i].stageFlags = shaderStage;
        _globalBindingsLayouts[i].pImmutableSamplers = nullptr;
        
        _globalBindings[i].size = size;
        _globalBindings[i].count = count;
        _globalBindings[i].offset = 0;
    }

    void PipelineLayout::declareGlobalUniformSampler(uint32_t binding, VkShaderStageFlags shaderStage)
    {
        if (_locked)
            throw std::runtime_error("Cannot declare uniform while pipeline layout is locked.");
        
        int i(0);
        for (; i < _globalBindingsLayouts.size(); i++)
            if (_globalBindingsLayouts[i].binding == binding)
                break;
        
        if (i == _globalBindingsLayouts.size())
        {
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = binding;

            _globalBindingsLayouts.push_back(layoutBinding);
            _globalBindings.push_back(DescriptorSetLayoutBindingState{});
        }

        _globalBindingsLayouts[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        _globalBindingsLayouts[i].descriptorCount = 1;
        _globalBindingsLayouts[i].stageFlags = shaderStage;
        _globalBindingsLayouts[i].pImmutableSamplers = nullptr;
        
        _globalBindings[i].size = 0;
        _globalBindings[i].count = 1;
        _globalBindings[i].offset = 0;
    }

    void PipelineLayout::declareDrawablesUniform(uint32_t binding, uint32_t size, VkShaderStageFlags shaderStage)
    {
        declareDrawablesUniformArray(binding, size, 1, shaderStage);
    }

    void PipelineLayout::declareDrawablesUniformArray(uint32_t binding, uint32_t size, uint32_t count, VkShaderStageFlags shaderStage)
    {
        if (_locked)
            throw std::runtime_error("Cannot declare uniform while pipeline layout is locked.");
        
        int i(0);
        for (; i < _drawablesBindingsLayouts.size(); i++)
            if (_drawablesBindingsLayouts[i].binding == binding)
                break;
        
        if (i == _drawablesBindingsLayouts.size())
        {
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = binding;

            _drawablesBindingsLayouts.push_back(layoutBinding);
            _drawablesBindings.push_back(DescriptorSetLayoutBindingState{});
        }

        _drawablesBindingsLayouts[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        _drawablesBindingsLayouts[i].descriptorCount = count;
        _drawablesBindingsLayouts[i].stageFlags = shaderStage;
        _drawablesBindingsLayouts[i].pImmutableSamplers = nullptr;
        
        _drawablesBindings[i].size = size;
        _drawablesBindings[i].count = count;
        _drawablesBindings[i].offset = 0;
    }
    
    void PipelineLayout::declareDrawablesUniformSampler(uint32_t binding, VkShaderStageFlags shaderStage)
    {
        if (_locked)
            throw std::runtime_error("Cannot declare uniform while pipeline layout is locked.");
        
        int i(0);
        for (; i < _drawablesBindingsLayouts.size(); i++)
            if (_drawablesBindingsLayouts[i].binding == binding)
                break;
        
        if (i == _drawablesBindingsLayouts.size())
        {
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = binding;

            _drawablesBindingsLayouts.push_back(layoutBinding);
            _drawablesBindings.push_back(DescriptorSetLayoutBindingState{});
        }

        _drawablesBindingsLayouts[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        _drawablesBindingsLayouts[i].descriptorCount = 1;
        _drawablesBindingsLayouts[i].stageFlags = shaderStage;
        _drawablesBindingsLayouts[i].pImmutableSamplers = nullptr;
        
        _drawablesBindings[i].size = 0;
        _drawablesBindings[i].count = 1;
        _drawablesBindings[i].offset = 0;
    }

    void PipelineLayout::lock(const Swapchain& swapchain)
    {
        _swapchainImageCount = swapchain.getImageCount();

        createVulkanGlobalDescriptorSetLayout();
        createVulkanDrawablesDescriptorSetLayout();

        createVulkanPipelineLayout();

        computeBuffersOffsets();

        createVulkanAttachmentsDescriptorSets();
        createVulkanGlobalDescriptorSets();

        createGlobalBuffers();

        computeUpdateNeeds();

        _locked = true;
    }
    
    void PipelineLayout::unlock()
    {
        for (std::pair<const Drawable*, std::vector<uint8_t>> drawableData: _drawablesData)
        {
            destroyDrawablesBuffers(*drawableData.first);

            destroyVulkanDrawablesDescriptorSets(*drawableData.first);
        }
        destroyGlobalBuffers();

        destroyVulkanGlobalDescriptorSets();
        destroyVulkanAttachmentsDescriptorSets();

        destroyVulkanPipelineLayout();

        destroyVulkanDrawablesDescriptorSetLayout();
        destroyVulkanGlobalDescriptorSetLayout();

        _locked = false;
    }
    
    void PipelineLayout::setGlobalUniformSampler(uint32_t binding, const Texture& texture)
    {
        if (!_locked)
            throw std::runtime_error("Cannot set uniform value while pipeline layout is not locked.");

        _globalSamplers[binding] = &texture;

        for (int i(0); i < _swapchainImageCount; i++)
            _globalNeedsUpdate[binding][i] = true;
    }

    void PipelineLayout::setDrawablesUniformSampler(const Drawable& drawable, uint32_t binding, const Texture& texture)
    {
        if (!_locked)
            throw std::runtime_error("Cannot set uniform value while pipeline layout is not locked.");

        addDrawable(drawable);
        _drawablesSamplers[&drawable][binding] = &texture;

        for (int i(0); i < _swapchainImageCount; i++)
            _drawablesNeedsUpdate[binding][&drawable][i] = true;
    }

    VkPipelineLayout PipelineLayout::getVulkanPipelineLayout() const
    {
        return _vulkanPipelineLayout;
    }
    
    PipelineLayout::~PipelineLayout()
    {
        unlock();

        vkDestroyDescriptorSetLayout(Device::Active->getVulkanDevice(), _vulkanAttachmentsSetLayout, nullptr);
        _vulkanDrawablesSetLayout = VK_NULL_HANDLE;

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkDescriptorSetLayout successfully destroyed." << std::endl;
        #endif

        destroyVulkanDescriptorPool();
    }
    
    PipelineLayout::PipelineLayout(const std::vector<bool>& attachmentsBitmap) :
        _vulkanAttachmentsSetLayout(VK_NULL_HANDLE),
        _vulkanGlobalSetLayout(VK_NULL_HANDLE),
        _vulkanDrawablesSetLayout(VK_NULL_HANDLE),
        
        _vulkanPipelineLayout(VK_NULL_HANDLE),

        _locked(false),
        _swapchainImageCount(0),

        _alignment(Device::Active->getPhysicalDevice().properties.limits.minUniformBufferOffsetAlignment),

        _vulkanDescriptorPool(VK_NULL_HANDLE)
    {
        createVulkanDescriptorPool();

        for (int i(0); i < attachmentsBitmap.size(); i++)
        {
            if (attachmentsBitmap[i])
            {
                VkDescriptorSetLayoutBinding binding{};
                binding.binding = i;
                binding.descriptorCount = 1;
                binding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                binding.pImmutableSamplers = nullptr;
                binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

                _attachmentsBindingsLayouts.push_back(binding);
            }
        }

        VkDescriptorSetLayoutCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.bindingCount = _attachmentsBindingsLayouts.size();
        createInfo.pBindings = _attachmentsBindingsLayouts.size() != 0 ? _attachmentsBindingsLayouts.data(): nullptr;

        VkResult result = vkCreateDescriptorSetLayout(Device::Active->getVulkanDevice(), &createInfo, nullptr, &_vulkanAttachmentsSetLayout);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout. VkResult: " + std::to_string(result));

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkDescriptorSetLayout successfully created." << std::endl;
        #endif
    }
    
    void PipelineLayout::attachmentUpdate(const Swapchain& swapchain, std::vector<VkImageView> attachmentsViews)
    {
        if (!_locked)
            throw std::runtime_error("Cannot bind a pipeline while its pipeline layout is not locked.");
        
        if (_attachmentsBindingsLayouts.size() == 0)
            return;

        uint32_t frame = swapchain.getCurrentImage();

        std::vector<VkDescriptorImageInfo> imageInfos(_attachmentsBindingsLayouts.size());
        std::vector<VkWriteDescriptorSet> descriptorWrites(_attachmentsBindingsLayouts.size());
        for (int i(0); i < _attachmentsBindingsLayouts.size(); i++)
        {
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            imageInfo.imageView = attachmentsViews[_attachmentsBindingsLayouts[i].binding];
            imageInfo.sampler = VK_NULL_HANDLE;

            imageInfos[i] = imageInfo;

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = _vulkanAttachmentsDescriptorSets[frame];
            descriptorWrite.dstBinding = _attachmentsBindingsLayouts[i].binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
            descriptorWrite.pBufferInfo = nullptr;
            descriptorWrite.pImageInfo = &imageInfos[i];
            descriptorWrite.pTexelBufferView = nullptr;

            descriptorWrites[i] = descriptorWrite;
        }

        vkUpdateDescriptorSets(Device::Active->getVulkanDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }

    void PipelineLayout::globalUpdate(const Swapchain& swapchain)
    {
        if (!_locked)
            throw std::runtime_error("Cannot bind a pipeline while its pipeline layout is not locked.");

        if (_globalBindings.size() == 0)
            return;

        uint32_t frame = swapchain.getCurrentImage();

        if (_globalData.size() != 0)
            _globalBuffers[frame]->setData(_globalData.data(), _globalData.size());

        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;
        for (int i(0); i < _globalBindings.size(); i++)
        {
            if (_globalNeedsUpdate[i][frame])
            {
                bufferInfos.push_back({});
                imageInfos.push_back({});

                switch (_globalBindingsLayouts[i].descriptorType)
                {
                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                    {
                        VkDescriptorBufferInfo bufferInfo{};
                        bufferInfo.buffer = _globalBuffers[frame]->getVulkanBuffer();
                        bufferInfo.offset = _globalBindings[i].offset;
                        bufferInfo.range = _globalBindings[i].size;

                        bufferInfos[i] = bufferInfo;
                        break;
                    }
                    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                    {
                        if (_globalSamplers[i] == nullptr)
                            throw std::runtime_error("Sampler at global binding " + std::to_string(i) + " declared but not set.");

                        VkDescriptorImageInfo imageInfo{};
                        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        imageInfo.imageView = _globalSamplers[i]->getVulkanImageView();
                        imageInfo.sampler = _globalSamplers[i]->getVulkanSampler();

                        imageInfos[i] = imageInfo;
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        std::vector<VkWriteDescriptorSet> descriptorWrites;
        for (int i(0); i < _globalBindings.size(); i++)
        {
            if (_globalNeedsUpdate[i][frame])
            {
                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = _vulkanGlobalDescriptorSets[frame];
                descriptorWrite.dstBinding = i;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorCount = _globalBindings[i].count;
                descriptorWrite.pBufferInfo = nullptr;
                descriptorWrite.pImageInfo = nullptr;
                descriptorWrite.pTexelBufferView = nullptr;

                switch (_globalBindingsLayouts[i].descriptorType)
                {
                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:

                        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        descriptorWrite.pBufferInfo = &bufferInfos[descriptorWrites.size()];
                        break;

                    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:

                        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        descriptorWrite.pImageInfo = &imageInfos[descriptorWrites.size()];
                        break;

                    default:
                        break;
                }

                descriptorWrites.push_back(descriptorWrite);
                _globalNeedsUpdate[i][frame] = false;
            }
        }

        if (descriptorWrites.size() != 0)
            vkUpdateDescriptorSets(Device::Active->getVulkanDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }
    
    void PipelineLayout::drawableUpdate(const Drawable& drawable, const Swapchain& swapchain)
    {
        if (!_locked)
            throw std::runtime_error("Cannot bind a pipeline while its pipeline layout is not locked.");

        if (_drawablesBindings.size() == 0)
            return;
        
        addDrawable(drawable);

        uint32_t frame = swapchain.getCurrentImage();

        if (_drawablesData[&drawable].size() != 0)
            _drawablesBuffers[&drawable][frame]->setData(_drawablesData[&drawable].data(), _drawablesData[&drawable].size());

        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;
        for (int i(0); i < _drawablesBindings.size(); i++)
        {
            bufferInfos.push_back({});
            imageInfos.push_back({});
            
            if (_drawablesNeedsUpdate[i][&drawable][frame])
            {
                switch (_drawablesBindingsLayouts[i].descriptorType)
                {
                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                    {
                        VkDescriptorBufferInfo bufferInfo{};
                        bufferInfo.buffer = _drawablesBuffers[&drawable][frame]->getVulkanBuffer();
                        bufferInfo.offset = _drawablesBindings[i].offset;
                        bufferInfo.range = _drawablesBindings[i].size;

                        bufferInfos[i] = bufferInfo;
                        break;
                    }
                    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                    {
                        if (_drawablesSamplers[&drawable][i] == nullptr)
                            throw std::runtime_error("Sampler at drawable binding " + std::to_string(i) + " declared but not set.");

                        VkDescriptorImageInfo imageInfo{};
                        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        imageInfo.imageView = _drawablesSamplers[&drawable][i]->getVulkanImageView();
                        imageInfo.sampler = _drawablesSamplers[&drawable][i]->getVulkanSampler();

                        imageInfos[i] = imageInfo;
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        std::vector<VkWriteDescriptorSet> descriptorWrites;
        for (int i(0); i < _drawablesBindings.size(); i++)
        {
            if (_drawablesNeedsUpdate[i][&drawable][frame])
            {
                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = _vulkanDrawablesDescriptorSets[&drawable][frame];
                descriptorWrite.dstBinding = i;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorCount = _drawablesBindings[i].count;
                descriptorWrite.pBufferInfo = nullptr;
                descriptorWrite.pImageInfo = nullptr;
                descriptorWrite.pTexelBufferView = nullptr;

                switch (_drawablesBindingsLayouts[i].descriptorType)
                {
                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:

                        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        descriptorWrite.pBufferInfo = &bufferInfos[i];
                        break;

                    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:

                        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        descriptorWrite.pImageInfo = &imageInfos[i];
                        break;

                    default:
                        break;
                }

                descriptorWrites.push_back(descriptorWrite);
                _drawablesNeedsUpdate[i][&drawable][frame] = false;
            }
        }

        if (descriptorWrites.size() != 0)
            vkUpdateDescriptorSets(Device::Active->getVulkanDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }
    
    void PipelineLayout::bind(const Drawable& drawable, const Swapchain& swapchain)
    {
        if (!_locked)
            throw std::runtime_error("Cannot draw using a pipeline while its pipeline layout is not locked.");

        int i = swapchain.getCurrentImage();

        std::vector<VkDescriptorSet> descriptorSets;
        descriptorSets.push_back(_vulkanAttachmentsDescriptorSets[i]);
        descriptorSets.push_back(_vulkanGlobalDescriptorSets[i]);
        if (_drawablesBindings.size() != 0)
            descriptorSets.push_back(_vulkanDrawablesDescriptorSets[&drawable][i]);

        vkCmdBindDescriptorSets(swapchain.getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _vulkanPipelineLayout, 0, descriptorSets.size(), descriptorSets.data(), 0, nullptr);
    }
    
    void PipelineLayout::createVulkanDescriptorPool()
    {
        destroyVulkanDescriptorPool();

        _descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        _descriptorPoolSizes[0].descriptorCount = DESCRIPTOR_POOL_SIZE;

        _descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        _descriptorPoolSizes[1].descriptorCount = DESCRIPTOR_POOL_SIZE;

        _descriptorPoolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        _descriptorPoolSizes[2].descriptorCount = DESCRIPTOR_POOL_SIZE;

        _descriptorPool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        _descriptorPool.pNext = nullptr;
        _descriptorPool.flags = 0;
        _descriptorPool.poolSizeCount = _descriptorPoolSizes.size();
        _descriptorPool.pPoolSizes = _descriptorPoolSizes.data();
        _descriptorPool.maxSets = DESCRIPTOR_POOL_SIZE;

        VkResult result = vkCreateDescriptorPool(Device::Active->getVulkanDevice(), &_descriptorPool, nullptr, &_vulkanDescriptorPool);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor pool. VkResult: " + std::to_string(result));

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkDescriptorPool successfully created." << std::endl;
        #endif
    }
    
    void PipelineLayout::createVulkanGlobalDescriptorSetLayout()
    {
        destroyVulkanGlobalDescriptorSetLayout();

        VkDescriptorSetLayoutCreateInfo createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.bindingCount = _globalBindingsLayouts.size();
        createInfo.pBindings = _globalBindingsLayouts.size() != 0 ?_globalBindingsLayouts.data() : nullptr;

        VkResult result = vkCreateDescriptorSetLayout(Device::Active->getVulkanDevice(), &createInfo, nullptr, &_vulkanGlobalSetLayout);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout. VkResult: " + std::to_string(result));

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkDescriptorSetLayout successfully created." << std::endl;
        #endif
    }

    void PipelineLayout::createVulkanDrawablesDescriptorSetLayout()
    {
        destroyVulkanDrawablesDescriptorSetLayout();

        VkDescriptorSetLayoutCreateInfo createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.bindingCount = _drawablesBindingsLayouts.size();
        createInfo.pBindings = _drawablesBindingsLayouts.size() != 0 ? _drawablesBindingsLayouts.data() : nullptr;

        VkResult result = vkCreateDescriptorSetLayout(Device::Active->getVulkanDevice(), &createInfo, nullptr, &_vulkanDrawablesSetLayout);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout. VkResult: " + std::to_string(result));

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkDescriptorSetLayout successfully created." << std::endl;
        #endif
    }
    
    void PipelineLayout::createVulkanPipelineLayout()
    {
        destroyVulkanPipelineLayout();

        _pipelineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        _pipelineLayout.pNext = nullptr;
        _pipelineLayout.flags = 0;

        std::vector<VkDescriptorSetLayout> setLayouts;
        setLayouts.push_back(_vulkanAttachmentsSetLayout);
        setLayouts.push_back(_vulkanGlobalSetLayout);
        setLayouts.push_back(_vulkanDrawablesSetLayout);

        _pipelineLayout.setLayoutCount = setLayouts.size();
        _pipelineLayout.pSetLayouts = setLayouts.data();

        _pipelineLayout.pushConstantRangeCount = 0;
        _pipelineLayout.pPushConstantRanges = nullptr;

        VkResult result = vkCreatePipelineLayout(Device::Active->getVulkanDevice(), &_pipelineLayout, nullptr, &_vulkanPipelineLayout);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create pipeline layout. VkResult: " + std::to_string(result));

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkPipelineLayout successfully created." << std::endl;
        #endif
    }
    
    void PipelineLayout::createVulkanAttachmentsDescriptorSets()
    {
        destroyVulkanAttachmentsDescriptorSets();

        _vulkanAttachmentsDescriptorSets.resize(_swapchainImageCount);

        std::vector<VkDescriptorSetLayout> setLayouts(_swapchainImageCount, _vulkanAttachmentsSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.descriptorPool = _vulkanDescriptorPool;
        allocInfo.descriptorSetCount = setLayouts.size();
        allocInfo.pSetLayouts = setLayouts.data();

        VkResult result = vkAllocateDescriptorSets(Device::Active->getVulkanDevice(), &allocInfo, _vulkanAttachmentsDescriptorSets.data());
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate descriptor sets. VkResult: " + std::to_string(result));
    }

    void PipelineLayout::createVulkanGlobalDescriptorSets()
    {
        destroyVulkanGlobalDescriptorSets();

        _vulkanGlobalDescriptorSets.resize(_swapchainImageCount);

        std::vector<VkDescriptorSetLayout> setLayouts(_swapchainImageCount, _vulkanGlobalSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.descriptorPool = _vulkanDescriptorPool;
        allocInfo.descriptorSetCount = setLayouts.size();
        allocInfo.pSetLayouts = setLayouts.data();

        VkResult result = vkAllocateDescriptorSets(Device::Active->getVulkanDevice(), &allocInfo, _vulkanGlobalDescriptorSets.data());
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate descriptor sets. VkResult: " + std::to_string(result));
    }
    
    void PipelineLayout::createGlobalBuffers()
    {
        destroyGlobalBuffers();

        if (_globalBindings.size() == 0)
            return;
    
        _globalSamplers.resize(_globalBindings.size(), nullptr);

        uint32_t n;
        n = _globalBindings.size() - 1;
        uint32_t totalSize = _globalBindings[n].offset + _globalBindings[n].size * _globalBindings[n].count;
        
        if (totalSize == 0)
            return;

        _globalData.resize(totalSize, 0);
        _globalBuffers.resize(_swapchainImageCount);

        for (int i(0); i < _swapchainImageCount; i++)
            _globalBuffers[i] = new Buffer(totalSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
    
    void PipelineLayout::createVulkanDrawablesDescriptorSets(const Drawable& drawable)
    {
        destroyVulkanDrawablesDescriptorSets(drawable);

        _vulkanDrawablesDescriptorSets[&drawable].resize(_swapchainImageCount);

        std::vector<VkDescriptorSetLayout> setLayouts(_swapchainImageCount, _vulkanDrawablesSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.descriptorPool = _vulkanDescriptorPool;
        allocInfo.descriptorSetCount = setLayouts.size();
        allocInfo.pSetLayouts = setLayouts.data();

        VkResult result = vkAllocateDescriptorSets(Device::Active->getVulkanDevice(), &allocInfo, _vulkanDrawablesDescriptorSets[&drawable].data());
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate descriptor sets. VkResult: " + std::to_string(result));
    }
    
    void PipelineLayout::createDrawablesBuffers(const Drawable& drawable)
    {
        destroyDrawablesBuffers(drawable);

        if (_drawablesBindings.size() == 0)
            return;
    
        _drawablesSamplers[&drawable].resize(_drawablesBindings.size(), nullptr);

        uint32_t n;
        n = _drawablesBindings.size() - 1;
        uint32_t totalSize = _drawablesBindings[n].offset + _drawablesBindings[n].size * _drawablesBindings[n].count;
        
        if (totalSize == 0)
            return;
        
        _drawablesData[&drawable].resize(totalSize, 0);
        _drawablesBuffers[&drawable].resize(_swapchainImageCount);

        for (int i(0); i < _swapchainImageCount; i++)
            _drawablesBuffers[&drawable][i] = new Buffer(totalSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
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
    
    void PipelineLayout::destroyVulkanGlobalDescriptorSetLayout()
    {
        if (_vulkanGlobalSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(Device::Active->getVulkanDevice(), _vulkanGlobalSetLayout, nullptr);

            #ifndef NDEBUG
            std::clog << "<S3DL Debug> VkDescriptorSetLayout successfully destroyed." << std::endl;
            #endif
        }

        _vulkanGlobalSetLayout = VK_NULL_HANDLE;
    }

    void PipelineLayout::destroyVulkanDrawablesDescriptorSetLayout()
    {
        if (_vulkanDrawablesSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(Device::Active->getVulkanDevice(), _vulkanDrawablesSetLayout, nullptr);

            #ifndef NDEBUG
            std::clog << "<S3DL Debug> VkDescriptorSetLayout successfully destroyed." << std::endl;
            #endif
        }

        _vulkanDrawablesSetLayout = VK_NULL_HANDLE;
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

        _vulkanPipelineLayout = VK_NULL_HANDLE;
    }
    
    void PipelineLayout::destroyVulkanAttachmentsDescriptorSets()
    {
        _vulkanAttachmentsDescriptorSets.clear();
    }

    void PipelineLayout::destroyVulkanGlobalDescriptorSets()
    {
        _vulkanGlobalDescriptorSets.clear();
    }
    
    void PipelineLayout::destroyGlobalBuffers()
    {
        for (int i(0); i < _globalBuffers.size(); i++)
            delete _globalBuffers[i];
        
        _globalData.clear();
        _globalBuffers.clear();
        _globalSamplers.clear();
    }
    
    void PipelineLayout::destroyVulkanDrawablesDescriptorSets(const Drawable& drawable)
    {
        _vulkanDrawablesDescriptorSets[&drawable].clear();
    }
    
    void PipelineLayout::destroyDrawablesBuffers(const Drawable& drawable)
    {
        for (int i(0); i < _drawablesBuffers[&drawable].size(); i++)
            delete _drawablesBuffers[&drawable][i];
        
        _drawablesData[&drawable].clear();
        _drawablesBuffers[&drawable].clear();
        _drawablesSamplers[&drawable].clear();
    }

    void PipelineLayout::computeBuffersOffsets()
    {
        uint32_t offset;

        offset = 0;
        for (int i(0); i < _globalBindings.size(); i++)
        {
            _globalBindings[i].offset = offset;
            offset += _globalBindings[i].size * _globalBindings[i].count;
            offset += (_alignment - offset) % _alignment;
        }

        offset = 0;
        for (int i(0); i < _drawablesBindings.size(); i++)
        {
            _drawablesBindings[i].offset = offset;
            offset += _drawablesBindings[i].size * _drawablesBindings[i].count;
            offset += (_alignment - offset) % _alignment;
        }
    }

    void PipelineLayout::computeUpdateNeeds()
    {
        _globalNeedsUpdate.clear();
        _drawablesNeedsUpdate.clear();

        _globalNeedsUpdate.resize(_globalBindings.size());
        for (int i(0); i < _globalNeedsUpdate.size(); i++)
            _globalNeedsUpdate[i].resize(_swapchainImageCount, true);
        
        _drawablesNeedsUpdate.resize(_drawablesBindings.size());
    }

    void PipelineLayout::addDrawable(const Drawable& drawable)
    {
        if (_drawablesData.count(&drawable) == 0)
        {
            _drawablesData[&drawable] = {};
            _vulkanDrawablesDescriptorSets[&drawable] = {};
            _drawablesBuffers[&drawable] = {};

            createVulkanDrawablesDescriptorSets(drawable);
            createDrawablesBuffers(drawable);

            for (int i(0); i < _drawablesNeedsUpdate.size(); i++)
                _drawablesNeedsUpdate[i][&drawable].resize(_swapchainImageCount, true);
        }
    }
}
