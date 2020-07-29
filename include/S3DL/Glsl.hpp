#pragma once

#include <cstdint>
#include <initializer_list>
#include <ostream>

namespace s3dl
{
    template<typename T, unsigned int n, typename C>
    struct _vec
    {
        T& operator[](unsigned int i);
        T const& operator[](unsigned int i) const;

        C operator+=(_vec<T, n, C> const& v);
        C operator-=(_vec<T, n, C> const& v);
        C operator*=(_vec<T, n, C> const& v);
        C operator/=(_vec<T, n, C> const& v);

        C operator+=(T x);
        C operator-=(T x);
        C operator*=(T x);
        C operator/=(T x);
    };

    template<typename T, unsigned int n, typename C>
    std::ostream& operator<<(std::ostream& stream, _vec<T, n, C> const& v);

    template<typename C>
    C cross(_vec<float, 3, C> const& u, _vec<float, 3, C> const& v);
    template<unsigned int n, typename C>
    float dot(_vec<float, n, C> const& u, _vec<float, n, C> const& v);
    template<unsigned int n, typename C>
    float length(_vec<float, n, C> const& v);
    template<unsigned int n, typename C>
    float distance(_vec<float, n, C> const& u, _vec<float, n, C> const& v);
    template<unsigned int n, typename C>
    C normalize(_vec<float, n, C> const& v);
    template<unsigned int n, typename C>
    C faceforward(_vec<float, n, C> const& u, _vec<float, n, C> const& v, _vec<float, n, C> const& w);
    template<unsigned int n, typename C>
    C reflect(_vec<float, n, C> const& u, _vec<float, n, C> const& v);
    template<unsigned int n, typename C>
    C refract(_vec<float, n, C> const& u, _vec<float, n, C> const& v, float r);

    template<typename T, unsigned int n, typename C>
    C operator+(_vec<T, n, C> const& u, _vec<T, n, C> const& v);
    template<typename T, unsigned int n, typename C>
    C operator-(_vec<T, n, C> const& u, _vec<T, n, C> const& v);
    template<typename T, unsigned int n, typename C>
    C operator*(_vec<T, n, C> const& u, _vec<T, n, C> const& v);
    template<typename T, unsigned int n, typename C>
    C operator/(_vec<T, n, C> const& u, _vec<T, n, C> const& v);
    
    template<typename T, unsigned int n, typename C>
    C operator+(_vec<T, n, C> const& u, T x);
    template<typename T, unsigned int n, typename C>
    C operator-(_vec<T, n, C> const& u, T x);
    template<typename T, unsigned int n, typename C>
    C operator*(_vec<T, n, C> const& u, T x);
    template<typename T, unsigned int n, typename C>
    C operator/(_vec<T, n, C> const& u, T x);
    template<typename T, unsigned int n, typename C>
    C operator+(T x, _vec<T, n, C> const& u);
    template<typename T, unsigned int n, typename C>
    C operator-(T x, _vec<T, n, C> const& u);
    template<typename T, unsigned int n, typename C>
    C operator*(T x, _vec<T, n, C> const& u);
    template<typename T, unsigned int n, typename C>
    C operator/(T x, _vec<T, n, C> const& u);

    template<typename T>
    struct _vec2: _vec<T, 2, _vec2<T>>
    {
        _vec2() = default;
        _vec2(T value);
        _vec2(T xValue, T yValue);
        _vec2(std::initializer_list<T> tab);
        _vec2(_vec<T, 2, _vec2<T>> const& v);

        T x, y;
    };

    template<typename T>
    struct _vec3: _vec<T, 3, _vec3<T>>
    {
        _vec3() = default;
        _vec3(T value);
        _vec3(T xValue, T yValue, T zValue);
        _vec3(std::initializer_list<T> tab);
        _vec3(_vec<T, 3, _vec3<T>> const& v);
        _vec3(_vec2<T> const& v, T zValue);
        _vec3(T xValue, _vec2<T> const& v);

        T x, y, z;
    };

    template<typename T>
    struct _vec4: _vec<T, 4, _vec4<T>>
    {
        _vec4() = default;
        _vec4(T value);
        _vec4(T xValue, T yValue, T zValue, T wValue);
        _vec4(std::initializer_list<T> tab);
        _vec4(_vec<T, 4, _vec4<T>> const& v);
        _vec4(_vec2<T> const& v, T zValue, T wValue);
        _vec4(T xValue, _vec2<T> const& v, T wValue);
        _vec4(T xValue, T yValue, _vec2<T> const& v);
        _vec4(_vec2<T> const& u, _vec2<T> const& v);
        _vec4(_vec3<T> const& v, T wValue);
        _vec4(T xValue, _vec3<T> const& v);

        T x, y, z, w;
    };

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

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    struct _mat
    {
        _mat(std::initializer_list<C const&> columns);

        C& operator[](unsigned int i);
        C const& operator[](unsigned int i) const;

        _mat<T, m, n, C, D> operator+=(_mat<T, m, n, C, D> const& M);
        _mat<T, m, n, C, D> operator-=(_mat<T, m, n, C, D> const& M);
        _mat<T, m, n, C, D> operator/=(_mat<T, m, n, C, D> const& M);

        _mat<T, m, n, C, D> operator+=(T x);
        _mat<T, m, n, C, D> operator-=(T x);
        _mat<T, m, n, C, D> operator/=(T x);
        _mat<T, m, n, C, D> operator*=(T x);

        T data[n][m];
    };
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    std::ostream& operator<<(std::ostream& stream, _mat<T, m, n, C, D> const& M);
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, n, m, D, C> transpose(_mat<T, m, n, C, D> const& M);
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator+(_mat<T, m, n, C, D> const& A, _mat<T, m, n, C, D> const& B);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator-(_mat<T, m, n, C, D> const& A, _mat<T, m, n, C, D> const& B);
    template<typename T, unsigned int m, unsigned int n, unsigned int p, typename C, typename D, typename F>
    _mat<T, m, p, C, F> operator*(_mat<T, m, n, C, D> const& A, _mat<T, n, p, D, F> const& B);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator/(_mat<T, m, n, C, D> const& A, _mat<T, m, n, C, D> const& B);

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator+(_mat<T, m, n, C, D> const& M, T x);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator-(_mat<T, m, n, C, D> const& M, T x);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator*(_mat<T, m, n, C, D> const& M, T x);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator/(_mat<T, m, n, C, D> const& M, T x);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator+(T x, _mat<T, m, n, C, D> const& M);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator-(T x, _mat<T, m, n, C, D> const& M);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator*(T x, _mat<T, m, n, C, D> const& M);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator/(T x, _mat<T, m, n, C, D> const& M);

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    C operator*(_mat<T, m, n, C, D> const& M, D const& v);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    D operator*(C const& v, _mat<T, m, n, C, D> const& M);

    typedef _mat<float, 2, 2, _vec2<float>, _vec2<float>> mat2;
    typedef _mat<float, 3, 2, _vec2<float>, _vec3<float>> mat2x3;
    typedef _mat<float, 4, 2, _vec2<float>, _vec4<float>> mat2x4;
    typedef _mat<float, 2, 3, _vec3<float>, _vec2<float>> mat3x2;
    typedef _mat<float, 3, 3, _vec3<float>, _vec3<float>> mat3;
    typedef _mat<float, 4, 3, _vec3<float>, _vec4<float>> mat3x4;
    typedef _mat<float, 2, 4, _vec4<float>, _vec2<float>> mat4x2;
    typedef _mat<float, 3, 4, _vec4<float>, _vec3<float>> mat4x3;
    typedef _mat<float, 4, 4, _vec4<float>, _vec4<float>> mat4;
}

#include <S3DL/GlslT.hpp>
