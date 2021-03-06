#include <S3DL/S3DL.hpp>

namespace s3dl
{
    VkVertexInputBindingDescription Vertex::getBindingDescription(unsigned int binding, VkVertexInputRate inputRate)
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = binding;
        bindingDescription.inputRate = inputRate;
        bindingDescription.stride = sizeof(Vertex);

        return bindingDescription;
    }

    std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions(unsigned int binding)
    {
        std::vector<VkVertexInputAttributeDescription> descriptions(4);

        descriptions[0].binding = binding;
        descriptions[0].location = 0;
        descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        descriptions[0].offset = offsetof(Vertex, position);

        descriptions[1].binding = binding;
        descriptions[1].location = 1;
        descriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        descriptions[1].offset = offsetof(Vertex, texCoords);

        descriptions[2].binding = binding;
        descriptions[2].location = 2;
        descriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        descriptions[2].offset = offsetof(Vertex, normal);

        descriptions[3].binding = binding;
        descriptions[3].location = 3;
        descriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        descriptions[3].offset = offsetof(Vertex, color);

        return descriptions;
    }
}
