#pragma once

#include <string>
#include <cstring>
#include <cstdlib>

#include <S3DL/types.hpp>

namespace s3dl
{
    class TextureData
    {
        public:

            TextureData();
            TextureData(const TextureData& texture);
            TextureData(unsigned int width, unsigned int height, const Color& initialColor = {0, 0, 0, 255});
            TextureData(unsigned int width, unsigned int height, const unsigned char* data);
            TextureData(const std::string& filename);
            // TextureData(unsigned int width, unsigned int height, unsigned char* data);

            TextureData& operator=(const TextureData& texture);

            Color& operator()(unsigned int x, unsigned int y);
            const Color& operator()(unsigned int x, unsigned int y) const;

            const uvec2& size() const;

            unsigned int getRawSize() const;
            const unsigned char* getRawData() const;

            void toFile(const std::string& filename, float quality = 0.95f);

            ~TextureData();

        private:

            unsigned char* _data;
            uvec2 _size;
    };
}
