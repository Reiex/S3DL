#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    template<typename _VertexType>
    class Mesh : public Drawable
    {
        public:

            Mesh(int vertexCount, _VertexType* vertices);

        private:

            std::vector<_VertexType> vertices;
    };
}
