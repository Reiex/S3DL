#pragma once

#include <vector>
#include <cstring>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    template<typename _VertexType>
    class Mesh : public Drawable
    {
        public:

            Mesh(int vertexCount, const _VertexType* vertices);

            void destroy();

        private:

            void draw(const Device& device, VkCommandBuffer& commandBuffer) const;

            std::vector<_VertexType> _vertices;

            mutable bool _bufferCreated;
            mutable Buffer _buffer;
    };
}

#include <S3DL/MeshT.hpp>
