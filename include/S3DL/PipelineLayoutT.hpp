#pragma once

#include <S3DL/types.hpp>
#include <S3DL/PipelineLayout.hpp>

namespace s3dl
{
    template<typename T>
    void PipelineLayout::setGlobalUniform(uint32_t binding, const T& value)
    {
        setGlobalUniformArray(binding, &value, 1);
    }

    template<typename T>
    void PipelineLayout::setGlobalUniformArray(uint32_t binding, const T* values, uint32_t count, uint32_t startIndex)
    {
        if (!_locked)
            throw std::runtime_error("Cannot set uniform value while pipeline layout is not locked.");

        memcpy(&_globalData[_globalBindings[binding].offset + _globalBindings[binding].size * startIndex], values, _globalBindings[binding].size * count);

        for (int i(0); i < _swapchainImageCount; i++)
            _globalNeedsUpdate[binding][i] = true;
    }

    template<typename T>
    void PipelineLayout::setDrawablesUniform(const Drawable& drawable, uint32_t binding, const T& value)
    {
        setDrawablesUniformArray(drawable, binding, &value, 1);
    }

    template<typename T>
    void PipelineLayout::setDrawablesUniformArray(const Drawable& drawable, uint32_t binding, const T* values, uint32_t count, uint32_t startIndex)
    {
        if (!_locked)
            throw std::runtime_error("Cannot set uniform value while pipeline layout is not locked.");

        addDrawable(drawable);
        memcpy(&_drawablesData[&drawable][_drawablesBindings[binding].offset + _drawablesBindings[binding].size * startIndex], values, _drawablesBindings[binding].size * count);

        for (int i(0); i < _swapchainImageCount; i++)
            _drawablesNeedsUpdate[binding][&drawable][i] = true;
    }
}
