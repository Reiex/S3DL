#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
	template<typename T>
	class Mesh : public Drawable
	{
        public:

            Mesh(const std::vector<T>& vertices);
            Mesh(const Mesh& mesh) = delete;

            Mesh& operator=(const Mesh& mesh) = delete;

            ~Mesh();

        private:

            void createBuffer() const;
            void destroyBuffer() const;

            void draw(VkCommandBuffer commandBuffer) const;

            std::vector<T> _vertices;

            mutable bool _bufferCreated;
            mutable Buffer* _buffer;
	};
}

#include <S3DL/MeshT.hpp>
