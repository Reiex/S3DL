#pragma once

#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
	template<typename T>
	class Mesh : public Drawable
	{
        public:

            Mesh(const std::vector<T>& vertices);
            Mesh(const std::vector<T>& vertices, const std::vector<uint32_t>& indices);
            Mesh(const Mesh& mesh) = delete;

            Mesh& operator=(const Mesh& mesh) = delete;

            ~Mesh();

        private:

            void createVertexBuffer() const;
            void createIndexBuffer() const;
            void destroyVertexBuffer() const;
            void destroyIndexBuffer() const;

            void draw(VkCommandBuffer commandBuffer) const;

            std::vector<T> _vertices;
            mutable Buffer* _vertexBuffer;
            mutable bool _vertexBufferCreated;

            std::vector<uint32_t> _indices;
            mutable Buffer* _indexBuffer;
            mutable bool _indexBufferCreated;
	};
}

#include <S3DL/MeshT.hpp>
