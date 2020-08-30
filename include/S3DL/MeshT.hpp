#pragma once

#include <S3DL/types.hpp>

namespace s3dl
{
    template<typename T>
    Mesh<T>::Mesh(const std::vector<T>& vertices) :
        _bufferCreated(false),
        _vertices(vertices),
        _buffer(nullptr)
    {
    }

    template<typename T>
    Mesh<T>::~Mesh()
    {
        destroyBuffer();
    }

    template<typename T>
    void Mesh<T>::createBuffer() const
    {
        destroyBuffer();

        _buffer = new Buffer(_vertices.size() * sizeof(T), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        _buffer->setData(_vertices.data(), _vertices.size() * sizeof(T));
        _bufferCreated = true;
    }

    template<typename T>
    void Mesh<T>::destroyBuffer() const
    {
        if (_buffer != nullptr)
            delete _buffer;
        
        _buffer = nullptr;
        _bufferCreated = false;
    }

    template<typename T>
    void Mesh<T>::draw(VkCommandBuffer commandBuffer) const
    {
        if (!_bufferCreated)
            createBuffer();

        VkDeviceSize offsets[] = { 0 };
        VkBuffer buffer = _buffer->getVulkanBuffer();
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer, offsets);

        vkCmdDraw(commandBuffer, _vertices.size(), 1, 0, 0);
    }
}
