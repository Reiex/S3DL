#pragma once

#include <cstdint>

namespace s3dl
{
    class Instance;

    class Window;
    class RenderTarget;
    class RenderWindow;

    class Device;

    class Swapchain;


    class Attachment;
    class Subpass;
    class Dependency;
    class RenderPass;

    template<typename T> struct _vec2;
    template<typename T> struct _vec3;
    template<typename T> struct _vec4;
    typedef _vec2<float> vec2;
    typedef _vec3<float> vec3;
    typedef _vec4<float> vec4;
    typedef _vec2<int32_t> ivec2;
    typedef _vec3<int32_t> ivec3;
    typedef _vec4<int32_t> ivec4;
    typedef _vec2<uint32_t> uvec2;
    typedef _vec3<uint32_t> uvec3;
    typedef _vec4<uint32_t> uvec4;
    typedef _vec2<uint32_t> bvec2;
    typedef _vec3<uint32_t> bvec3;
    typedef _vec4<uint32_t> bvec4;
    template<typename T, unsigned int n, unsigned int m, typename C, typename D> struct _mat;
    typedef _mat<float, 2, 2, _vec2<float>, _vec2<float>> mat2;
    typedef _mat<float, 3, 2, _vec3<float>, _vec2<float>> mat2x3;
    typedef _mat<float, 4, 2, _vec4<float>, _vec2<float>> mat2x4;
    typedef _mat<float, 2, 3, _vec2<float>, _vec3<float>> mat3x2;
    typedef _mat<float, 3, 3, _vec3<float>, _vec3<float>> mat3;
    typedef _mat<float, 4, 3, _vec4<float>, _vec3<float>> mat3x4;
    typedef _mat<float, 2, 4, _vec2<float>, _vec4<float>> mat4x2;
    typedef _mat<float, 3, 4, _vec3<float>, _vec4<float>> mat4x3;
    typedef _mat<float, 4, 4, _vec4<float>, _vec4<float>> mat4;
    class Shader;

    class Pipeline;


    struct Vertex;
    class Drawable;
    template<typename T> class Mesh;

    class Buffer;
    typedef _vec4<unsigned char> Color;
    class TextureData;
    class TextureSampler;
    class Texture;

    class Framebuffer;
}
