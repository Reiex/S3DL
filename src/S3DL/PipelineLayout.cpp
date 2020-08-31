#include <S3DL/S3DL.hpp>

namespace s3dl
{
    void PipelineLayout::declareUniform(unsigned int binding, unsigned int size)
    {

    }

    void PipelineLayout::declareUniformArray(unsigned int binding, unsigned int size, unsigned int count)
    {

    }
    
    void PipelineLayout::lock(const Swapchain& swapchain)
    {
        _locked = true;
    }
    
    void PipelineLayout::unlock()
    {
        _locked = false;
    }
    
    template<typename T>
    void PipelineLayout::setUniform(unsigned int binding, T value)
    {
        if (!_locked)
            throw std::runtime_error("Cannot set uniform while pipeline layout is not locked.")
    }
    
    template<typename T>
    void PipelineLayout::setUniformArray(unsigned int binding, T* values)
    {
        if (!_locked)
            throw std::runtime_error("Cannot set uniform while pipeline layout is not locked.")
    }
    
    template<typename T>
    void PipelineLayout::setUniformArrayElement(unsigned int binding, unsigned int index, T value)
    {
        if (!_locked)
            throw std::runtime_error("Cannot set uniform while pipeline layout is not locked.")
    }
    
    VkPipelineLayout PipelineLayout::getVulkanPipelineLayout() const
    {

    }
    
    PipelineLayout::~PipelineLayout()
    {
    }
    
    PipelineLayout::PipelineLayout()
    {

    }

    void PipelineLayout::destroyVulkanPipelineLayout() const
    {

    }
}
