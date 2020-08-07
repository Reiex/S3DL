#include <S3DL/S3DL.hpp>

namespace s3dl
{
    template<typename _VertexType>
    Mesh<_VertexType>::Mesh(int vertexCount, const _VertexType* vertices) :
        _bufferCreated(false)
    {
        _vertices.resize(vertexCount);
        for (int i(0); i < vertexCount; i++)
            _vertices[i] = vertices[i];
    }
    
    template<typename _VertexType>
    void Mesh<_VertexType>::destroy()
    {
        _buffer.destroy();
    }

    template<typename _VertexType>
    void Mesh<_VertexType>::draw(const Device& device, VkCommandBuffer& commandBuffer) const
    {
        if (!_bufferCreated)
        {
            _buffer.reCreate(device, _vertices.size() * sizeof(_VertexType), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            _buffer.setData(_vertices.data(), _vertices.size() * sizeof(_VertexType));
            _bufferCreated = true;
        }

        VkDeviceSize offsets[] = {0};
        VkBuffer buffer = _buffer.getVulkanBuffer();
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer, offsets);

        vkCmdDraw(commandBuffer, _vertices.size(), 1, 0, 0);
    }
}
