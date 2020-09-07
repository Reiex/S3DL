#include <S3DL/S3DL.hpp>

namespace s3dl
{
    TextureData::TextureData() :
        _data(nullptr),
        _size(0, 0)
    {
    }

    TextureData::TextureData(const TextureData& texture)
    {
        *this = texture;
    }

    TextureData::TextureData(unsigned int width, unsigned int height, const Color& initialColor)
    {
        _size = {width, height};
        _data = (unsigned char*) std::malloc(sizeof(unsigned char)*width*height*4);
        for (int i(0); i < width*height; i++)
            for (int j(0); j < 4; j++)
                _data[4*i + j] = initialColor[j];
    }

    TextureData::TextureData(unsigned int width, unsigned int height, const unsigned char* data)
    {
        _size = {width, height};
        _data = (unsigned char*) std::malloc(sizeof(unsigned char)*width*height*4);
        std::memcpy(_data, data, width*height*4);
    }

    TextureData::TextureData(const std::string& filename)
    {
        int x, y;
        _data = stbi_load(filename.c_str(), &x, &y, nullptr, 4);
        _size = {(unsigned int) x, (unsigned int) y};
    }

    TextureData& TextureData::operator=(const TextureData& texture)
    {
        _size = texture._size;
        _data = (unsigned char*) std::malloc(sizeof(unsigned char)*_size.x*_size.y*4);
        std::memcpy(_data, texture._data, _size.x*_size.y*4);

        return *this;
    }

    Color& TextureData::operator()(unsigned int x, unsigned int y)
    {
        if (x < 0 || x >= _size.x || y < 0 | y >= _size.y)
            throw std::range_error("Cannot access pixel (" + std::to_string(x) + ", " + std::to_string(y) + ") of image of size (" + std::to_string(_size.x) + ", " + std::to_string(_size.y) + ").");
        
        return *((Color*) &_data[4*(y*_size.x + x)]);
    }

    const Color& TextureData::operator()(unsigned int x, unsigned int y) const
    {
        if (x < 0 || x >= _size.x || y < 0 | y >= _size.y)
            throw std::range_error("Cannot access pixel (" + std::to_string(x) + ", " + std::to_string(y) + ") of image of size (" + std::to_string(_size.x) + ", " + std::to_string(_size.y) + ").");
        
        return *((Color*) &_data[4*(y*_size.x + x)]);
    }

    const uvec2& TextureData::size() const
    {
        return _size;
    }

    unsigned int TextureData::getRawSize() const
    {
        return _size.x*_size.y*4;
    }

    const unsigned char* TextureData::getRawData() const
    {
        return _data;
    }

    void TextureData::toFile(const std::string& filename, float quality)
    {
        int n(filename.size());
        if (n > 4)
        {
            if (filename.substr(n-4, 4) == ".bmp")
            {
                stbi_write_bmp(filename.c_str(), _size.x, _size.y, 4, _data);
                return;
            }
            else if (filename.substr(n-4, 4) == ".png")
            {
                stbi_write_png(filename.c_str(), _size.x, _size.y, 4, _data, 0);
                return;
            }
            else if (filename.substr(n-4, 4) == ".jpg")
            {
                stbi_write_jpg(filename.c_str(), _size.x, _size.y, 4, _data, quality*100);
                return;
            }
        }

        if (n > 5)
        {
            if (filename.substr(n-5, 5) == ".jpeg")
            {
                stbi_write_jpg(filename.c_str(), _size.x, _size.y, 4, _data, quality*100);
                return;
            }
        }
    }

    TextureData::~TextureData()
    {
        if (_data != nullptr)
            std::free(_data);
    }
}
