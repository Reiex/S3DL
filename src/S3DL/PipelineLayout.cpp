#include <S3DL/S3DL.hpp>

namespace s3dl
{
    void PipelineLayout::declareGlobalUniform(uint32_t binding, uint32_t size, uint32_t count, VkShaderStageFlags shaderStage)
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
            layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            layoutBinding.pImmutableSamplers = nullptr;

            _globalBindingsLayouts.push_back(layoutBinding);
            _globalBindings.push_back(DescriptorSetLayoutBindingState{});
        }

        _globalBindingsLayouts[i].descriptorCount = count;
        _globalBindingsLayouts[i].stageFlags = shaderStage;
        
        _globalBindings[i].size = size;
        _globalBindings[i].count = count;
        _globalBindings[i].offset = 0;
        _globalBindings[i].needsUpdate = true;
    }

    void PipelineLayout::declareDrawablesUniform(uint32_t binding, uint32_t size, uint32_t count, VkShaderStageFlags shaderStage)
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
            layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            layoutBinding.pImmutableSamplers = nullptr;

            _drawablesBindingsLayouts.push_back(layoutBinding);
            _drawablesBindings.push_back(DescriptorSetLayoutBindingState{});
        }

        _drawablesBindingsLayouts[i].descriptorCount = count;
        _drawablesBindingsLayouts[i].stageFlags = shaderStage;
        
        _drawablesBindings[i].size = size;
        _drawablesBindings[i].count = count;
        _drawablesBindings[i].offset = 0;
        _drawablesBindings[i].needsUpdate = true;
    }
    
    void PipelineLayout::lock(const Swapchain& swapchain)
    {
        if (_globalBindings.size() > 0)
            createVulkanGlobalDescriptorSetLayout();
        
        if (_drawablesBindings.size() > 0)
            createVulkanDrawablesDescriptorSetLayout();
        
        createVulkanPipelineLayout();
        
        if (_globalBindings.size() > 0)
        {
            createVulkanGlobalDescriptorSets(swapchain);
            createGlobalBuffers(swapchain);
        }

        _locked = true;
    }
    
    void PipelineLayout::unlock()
    {
        destroyGlobalBuffers();
        destroyVulkanGlobalDescriptorSets();
        destroyVulkanPipelineLayout();
        destroyVulkanDrawablesDescriptorSetLayout();
        destroyVulkanGlobalDescriptorSetLayout();

        _locked = false;
    }
    
    VkPipelineLayout PipelineLayout::getVulkanPipelineLayout() const
    {
        return _vulkanPipelineLayout;
    }
    
    PipelineLayout::~PipelineLayout()
    {
        destroyDrawablesBuffers();
        destroyVulkanDrawablesDescriptorSets();
        destroyGlobalBuffers();
        destroyVulkanGlobalDescriptorSets();
        destroyVulkanPipelineLayout();
        destroyVulkanDrawablesDescriptorSetLayout();
        destroyVulkanGlobalDescriptorSetLayout();
        destroyVulkanDescriptorPool();
    }
    
    PipelineLayout::PipelineLayout() :
        _vulkanGlobalSetLayout(VK_NULL_HANDLE),
        _vulkanDrawablesSetLayout(VK_NULL_HANDLE),
        
        _vulkanPipelineLayout(VK_NULL_HANDLE),

        _locked(false),

        _alignment(Device::Active->getPhysicalDevice().properties.limits.minUniformBufferOffsetAlignment),

        _vulkanDescriptorPool(VK_NULL_HANDLE)
    {
        createVulkanDescriptorPool();
    }
    
    void PipelineLayout::globalUpdate(const Swapchain& swapchain)
    {

    }
    
    void PipelineLayout::drawableUpdate(const Drawable& drawable, const Swapchain& swapchain)
    {

    }
    
    void PipelineLayout::bind(const Drawable& drawable, const Swapchain& swapchain)
    {

    }
    
    void PipelineLayout::createVulkanDescriptorPool()
    {
        destroyVulkanDescriptorPool();

        _descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        _descriptorPoolSize.descriptorCount = DESCRIPTOR_POOL_SIZE;

        _descriptorPool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        _descriptorPool.pNext = nullptr;
        _descriptorPool.flags = 0;
        _descriptorPool.poolSizeCount = 1;
        _descriptorPool.pPoolSizes = &_descriptorPoolSize;
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
        createInfo.pBindings = _globalBindingsLayouts.data();

        VkResult result = vkCreateDescriptorSetLayout(Device::Active->getVulkanDevice(), &createInfo, nullptr, &_vulkanGlobalSetLayout);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout. VkResult: " + std::to_string(result));

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkDescriptorSetLayout successfully created." << std::endl;
        #endif
    }

    void PipelineLayout::createVulkanDrawablesDescriptorSetLayout()
    {
        destroyVulkanGlobalDescriptorSetLayout();

        VkDescriptorSetLayoutCreateInfo createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.bindingCount = _drawablesBindingsLayouts.size();
        createInfo.pBindings = _drawablesBindingsLayouts.data();

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
        if (_vulkanDrawablesSetLayout != VK_NULL_HANDLE)
            setLayouts = { _vulkanGlobalSetLayout, _vulkanDrawablesSetLayout };
        else if (_vulkanGlobalSetLayout != VK_NULL_HANDLE)
            setLayouts = { _vulkanGlobalSetLayout };
        
        _pipelineLayout.setLayoutCount = setLayouts.size();
        _pipelineLayout.pSetLayouts = setLayouts.size() != 0 ? setLayouts.data() : nullptr;

        _pipelineLayout.pushConstantRangeCount = 0;
        _pipelineLayout.pPushConstantRanges = nullptr;

        VkResult result = vkCreatePipelineLayout(Device::Active->getVulkanDevice(), &_pipelineLayout, nullptr, &_vulkanPipelineLayout);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create pipeline layout. VkResult: " + std::to_string(result));

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkPipelineLayout successfully created." << std::endl;
        #endif
    }
    
    void PipelineLayout::createVulkanGlobalDescriptorSets(const Swapchain& swapchain)
    {
        destroyVulkanGlobalDescriptorSets();

        _vulkanGlobalDescriptorSets.resize(swapchain.getImageCount());

        std::vector<VkDescriptorSetLayout> setLayouts(swapchain.getImageCount(), _vulkanGlobalSetLayout);
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
    
    void PipelineLayout::createGlobalBuffers(const Swapchain& swapchain)
    {

    }
    
    void PipelineLayout::createVulkanDrawablesDescriptorSets(const Drawable& drawable, const Swapchain& swapchain)
    {
        // Attention à l'initialisation de l'unordered map -> On peut pas resize un vect qui n'a pas été créé !
        // Donc s'occuper de savoir à quel moment on créer toutes les ressources pour un nouveau mesh...
    }
    
    void PipelineLayout::createDrawablesBuffers(const Drawable& drawable, const Swapchain& swapchain)
    {

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
            std::clog << "<S3DL Debug> VkDescriptorSetLayouts successfully destroyed." << std::endl;
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
            std::clog << "<S3DL Debug> VkDescriptorSetLayouts successfully destroyed." << std::endl;
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
    
    void PipelineLayout::destroyVulkanGlobalDescriptorSets()
    {
        _vulkanGlobalDescriptorSets.clear();
    }
    
    void PipelineLayout::destroyGlobalBuffers()
    {

    }
    
    void PipelineLayout::destroyVulkanDrawablesDescriptorSets()
    {
        _vulkanDrawablesDescriptorSets.clear();
    }
    
    void PipelineLayout::destroyDrawablesBuffers()
    {

    }
}
