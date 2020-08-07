#pragma once

#include <string>

#include <S3DL/types.hpp>

namespace s3dl
{
    class TextureData
    {
        public:

            TextureData(unsigned int width, unsigned int height, const uvec4& initialColor = {0, 0, 0, 255});
            TextureData(const std::string& filename);

            uvec4& operator()(unsigned int x, unsigned int y);
            const uvec4& operator()(unsigned int x, unsigned int y) const;

            const unsigned char* getData() const;

        private:

            unsigned char* _data;
            vec2 _size;
    };
}
