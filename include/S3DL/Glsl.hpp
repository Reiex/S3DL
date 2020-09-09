#pragma once

#include <cstdint>
#include <initializer_list>
#include <ostream>
#include <stdexcept>
#include <string>
#include <cmath>

#include <S3DL/types.hpp>

namespace s3dl
{
    template<typename T, unsigned int n, typename C>
    struct _vec
    {
        T& operator[](unsigned int i);
        const T& operator[](unsigned int i) const;

        C operator+=(const _vec<T, n, C>& v);
        C operator-=(const _vec<T, n, C>& v);
        C operator*=(const _vec<T, n, C>& v);
        C operator/=(const _vec<T, n, C>& v);

        C operator+=(T x);
        C operator-=(T x);
        C operator*=(T x);
        C operator/=(T x);
    };

    template<typename T, unsigned int n, typename C>
    std::ostream& operator<<(std::ostream& stream, const _vec<T, n, C>& v);

    template<typename C>
    C cross(const _vec<float, 3, C>& u, const _vec<float, 3, C>& v);
    template<unsigned int n, typename C>
    float dot(const _vec<float, n, C>& u, const _vec<float, n, C>& v);
    template<unsigned int n, typename C>
    float length(const _vec<float, n, C>& v);
    template<unsigned int n, typename C>
    float distance(const _vec<float, n, C>& u, const _vec<float, n, C>& v);
    template<unsigned int n, typename C>
    C normalize(const _vec<float, n, C>& v);
    template<unsigned int n, typename C>
    C faceforward(const _vec<float, n, C>& u, const _vec<float, n, C>& v, const _vec<float, n, C>& w);
    template<unsigned int n, typename C>
    C reflect(const _vec<float, n, C>& u, const _vec<float, n, C>& v);
    template<unsigned int n, typename C>
    C refract(const _vec<float, n, C>& u, const _vec<float, n, C>& v, float r);

    template<typename T, unsigned int n, typename C>
    C operator+(const _vec<T, n, C>& u, const _vec<T, n, C>& v);
    template<typename T, unsigned int n, typename C>
    C operator-(const _vec<T, n, C>& u, const _vec<T, n, C>& v);
    template<typename T, unsigned int n, typename C>
    C operator*(const _vec<T, n, C>& u, const _vec<T, n, C>& v);
    template<typename T, unsigned int n, typename C>
    C operator/(const _vec<T, n, C>& u, const _vec<T, n, C>& v);
    
    template<typename T, unsigned int n, typename C>
    C operator+(const _vec<T, n, C>& u, T x);
    template<typename T, unsigned int n, typename C>
    C operator-(const _vec<T, n, C>& u, T x);
    template<typename T, unsigned int n, typename C>
    C operator*(const _vec<T, n, C>& u, T x);
    template<typename T, unsigned int n, typename C>
    C operator/(const _vec<T, n, C>& u, T x);
    template<typename T, unsigned int n, typename C>
    C operator+(T x, const _vec<T, n, C>& u);
    template<typename T, unsigned int n, typename C>
    C operator-(T x, const _vec<T, n, C>& u);
    template<typename T, unsigned int n, typename C>
    C operator*(T x, const _vec<T, n, C>& u);
    template<typename T, unsigned int n, typename C>
    C operator/(T x, const _vec<T, n, C>& u);

    template<typename T>
    struct _vec2: _vec<T, 2, _vec2<T>>
    {
        _vec2() = default;
        _vec2(T value);
        _vec2(T xValue, T yValue);
        _vec2(std::initializer_list<T> tab);
        _vec2(const _vec<T, 2, _vec2<T>>& v);

        T x, y;
    };

    template<typename T>
    struct _vec3: _vec<T, 3, _vec3<T>>
    {
        _vec3() = default;
        _vec3(T value);
        _vec3(T xValue, T yValue, T zValue);
        _vec3(std::initializer_list<T> tab);
        _vec3(const _vec<T, 3, _vec3<T>>& v);
        _vec3(const _vec2<T>& v, T zValue);
        _vec3(T xValue, const _vec2<T>& v);

        T x, y, z;
    };

    template<typename T>
    struct _vec4: _vec<T, 4, _vec4<T>>
    {
        _vec4() = default;
        _vec4(T value);
        _vec4(T xValue, T yValue, T zValue, T wValue);
        _vec4(std::initializer_list<T> tab);
        _vec4(const _vec<T, 4, _vec4<T>>& v);
        _vec4(const _vec2<T>& v, T zValue, T wValue);
        _vec4(T xValue, const _vec2<T>& v, T wValue);
        _vec4(T xValue, T yValue, const _vec2<T>& v);
        _vec4(const _vec2<T>& u, const _vec2<T>& v);
        _vec4(const _vec3<T>& v, T wValue);
        _vec4(T xValue, const _vec3<T>& v);

        T x, y, z, w;
    };

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    struct _mat
    {
        public:

            _mat() = default;
            _mat(T diagValue);
            _mat(std::initializer_list<T> diag);
            _mat(std::initializer_list<C> columns);

            C& operator[](unsigned int i);
            const C& operator[](unsigned int i) const;

            _mat<T, m, n, C, D> operator+=(const _mat<T, m, n, C, D>& M);
            _mat<T, m, n, C, D> operator-=(const _mat<T, m, n, C, D>& M);
            _mat<T, m, n, C, D> operator/=(const _mat<T, m, n, C, D>& M);

            _mat<T, m, n, C, D> operator+=(T x);
            _mat<T, m, n, C, D> operator-=(T x);
            _mat<T, m, n, C, D> operator/=(T x);
            _mat<T, m, n, C, D> operator*=(T x);
        
        private:

            T data[n][m];
    };
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    std::ostream& operator<<(std::ostream& stream, const _mat<T, m, n, C, D>& M);
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, n, m, D, C> transpose(const _mat<T, m, n, C, D>& M);
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator+(const _mat<T, m, n, C, D>& A, const _mat<T, m, n, C, D>& B);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator-(const _mat<T, m, n, C, D>& A, const _mat<T, m, n, C, D>& B);
    template<typename T, unsigned int m, unsigned int n, unsigned int p, typename C, typename D, typename F>
    _mat<T, m, p, C, F> operator*(const _mat<T, m, n, C, D>& A, const _mat<T, n, p, D, F>& B);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator/(const _mat<T, m, n, C, D>& A, const _mat<T, m, n, C, D>& B);

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator+(const _mat<T, m, n, C, D>& M, T x);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator-(const _mat<T, m, n, C, D>& M, T x);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator*(const _mat<T, m, n, C, D>& M, T x);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator/(const _mat<T, m, n, C, D>& M, T x);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator+(T x, const _mat<T, m, n, C, D>& M);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator-(T x, const _mat<T, m, n, C, D>& M);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator*(T x, const _mat<T, m, n, C, D>& M);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator/(T x, const _mat<T, m, n, C, D>& M);

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    C operator*(const _mat<T, m, n, C, D>& M, const D& v);
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    D operator*(const C& v, const _mat<T, m, n, C, D>& M);
}

#include <S3DL/GlslT.hpp>
