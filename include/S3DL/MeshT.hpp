#pragma once

#include <S3DL/types.hpp>
#include <S3DL/Mesh.hpp>

namespace s3dl
{
    template<typename T>
    Mesh<T>::Mesh(const std::vector<T>& vertices) :
        _vertices(vertices),
        _vertexBuffer(nullptr),
        _vertexBufferCreated(false),
        _indexBuffer(nullptr),
        _indexBufferCreated(false)
    {
        _indices.resize(_vertices.size());
        for (int i(0); i < _indices.size(); i++)
            _indices[i] = i;
    }

    template<typename T>
    Mesh<T>::Mesh(const std::vector<T>& vertices, const std::vector<uint32_t>& indices) :
        _vertices(vertices),
        _vertexBuffer(nullptr),
        _vertexBufferCreated(false),
        _indices(indices),
        _indexBuffer(nullptr),
        _indexBufferCreated(false)
    {
    }

    template<typename T>
    Mesh<T>::~Mesh()
    {
        destroyVertexBuffer();
        destroyIndexBuffer();
    }

    template<typename T>
    void Mesh<T>::createVertexBuffer() const
    {
        destroyVertexBuffer();

        _vertexBuffer = new Buffer(_vertices.size() * sizeof(T), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        _vertexBuffer->setData(_vertices.data(), _vertices.size() * sizeof(T));
        _vertexBufferCreated = true;
    }

    template<typename T>
    void Mesh<T>::createIndexBuffer() const
    {
        destroyIndexBuffer();

        _indexBuffer = new Buffer(_indices.size() * sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        _indexBuffer->setData(_indices.data(), _indices.size() * sizeof(uint32_t));
        _indexBufferCreated = true;
    }

    template<typename T>
    void Mesh<T>::destroyVertexBuffer() const
    {
        if (_vertexBuffer != nullptr)
            delete _vertexBuffer;
        
        _vertexBuffer = nullptr;
        _vertexBufferCreated = false;
    }

    template<typename T>
    void Mesh<T>::destroyIndexBuffer() const
    {
        if (_indexBuffer != nullptr)
            delete _indexBuffer;
        
        _indexBuffer = nullptr;
        _indexBufferCreated = false;
    }

    template<typename T>
    void Mesh<T>::draw(VkCommandBuffer commandBuffer) const
    {
        if (!_vertexBufferCreated)
            createVertexBuffer();
        
        if (!_indexBufferCreated)
            createIndexBuffer();

        VkDeviceSize offsets[] = { 0 };
        VkBuffer vertexBuffer = _vertexBuffer->getVulkanBuffer();
        VkBuffer indexBuffer = _indexBuffer->getVulkanBuffer();
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, offsets);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(commandBuffer, _indices.size(), 1, 0, 0, 0);
    }
}
