#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Texture
    {
        public:

            Texture(const TextureData& textureData);  // En vrai il y a plus de paramètres...

        private:

            VkImage _image;
    };
}
