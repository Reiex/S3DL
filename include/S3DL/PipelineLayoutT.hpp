#pragma once

#include <S3DL/types.hpp>
#include <S3DL/PipelineLayout.hpp>

namespace s3dl
{
    template<typename T>
    void PipelineLayout::setUniform(uint32_t binding, T value, uint32_t set)
    {
        if (!_locked)
            throw std::runtime_error("Cannot set uniform while pipeline layout is not locked.");
        
        // Temporaire !
        uint8_t* ptr = (uint8_t*) &value;
        uint32_t offset = _bindings[set][binding].offset;
        uint32_t size = _bindings[set][binding].size;
        for (int i(0); i < size; i++)
            _bufferData[offset + i] = *(ptr + i);
        _bindings[set][binding].needsUpdate = true;
    }

    template<typename T>
    void PipelineLayout::setUniformArray(uint32_t binding, T* values, uint32_t set)
    {
        if (!_locked)
            throw std::runtime_error("Cannot set uniform while pipeline layout is not locked.");
    }

    template<typename T>
    void PipelineLayout::setUniformArrayElement(uint32_t binding, uint32_t index, T value, uint32_t set)
    {
        if (!_locked)
            throw std::runtime_error("Cannot set uniform while pipeline layout is not locked.");
    }
}


