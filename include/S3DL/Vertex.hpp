#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>
#include <S3DL/Glsl.hpp>

namespace s3dl
{
    struct Vertex
    {
        static VkVertexInputBindingDescription getBindingDescription(unsigned int binding = 0, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(unsigned int binding = 0);

        vec3 position;
        vec2 texCoords;
        vec3 normal;
        vec3 color;
    };
}
