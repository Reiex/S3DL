#pragma once

#include <S3DL/Glsl.hpp>

#include <stdexcept>
#include <string>
#include <cmath>

namespace s3dl
{
    template<typename T, unsigned int n, typename C>
    std::ostream& operator<<(std::ostream& stream, _vec<T, n, C> const& v)
    {
        for (int i(0); i < n-1; i++)
            stream << v[i] << " ";
                
        return stream << v[n-1];
    }


    template<typename T, unsigned int n, typename C>
    T& _vec<T, n, C>::operator[](unsigned int i)
    {
        if (i >= n)
            throw std::range_error("Cannot access element " + std::to_string(i) + " of vec" + std::to_string(n));
        
        return ((T*) this)[i];
    }

    template<typename T, unsigned int n, typename C>
    T const& _vec<T, n, C>::operator[](unsigned int i) const
    {
        if (i >= n)
            throw std::range_error("Cannot access element " + std::to_string(i) + " of vec" + std::to_string(n));
        
        return ((T*) this)[i];
    }

    template<typename T, unsigned int n, typename C>
    C _vec<T, n, C>::operator+=(_vec<T, n, C> const& v)
    {
        for (int i(0); i < n; i++)
            (*this)[i] += v[i];
        
        return C(*this);
    }

    template<typename T, unsigned int n, typename C>
    C _vec<T, n, C>::operator-=(_vec<T, n, C> const& v)
    {
        for (int i(0); i < n; i++)
            (*this)[i] -= v[i];
        
        return C(*this);
    }

    template<typename T, unsigned int n, typename C>
    C _vec<T, n, C>::operator*=(_vec<T, n, C> const& v)
    {
        for (int i(0); i < n; i++)
            (*this)[i] *= v[i];
        
        return C(*this);
    }

    template<typename T, unsigned int n, typename C>
    C _vec<T, n, C>::operator/=(_vec<T, n, C> const& v)
    {
        for (int i(0); i < n; i++)
            (*this)[i] /= v[i];
        
        return C(*this);
    }

    template<typename T, unsigned int n, typename C>
    C _vec<T, n, C>::operator+=(T x)
    {
        *this += C(x);

        return C(*this);
    }

    template<typename T, unsigned int n, typename C>
    C _vec<T, n, C>::operator-=(T x)
    {
        *this -= C(x);

        return C(*this);
    }

    template<typename T, unsigned int n, typename C>
    C _vec<T, n, C>::operator*=(T x)
    {
        *this *= C(x);

        return C(*this);
    }

    template<typename T, unsigned int n, typename C>
    C _vec<T, n, C>::operator/=(T x)
    {
        *this /= C(x);

        return C(*this);
    }


    template<typename C>
    C cross(_vec<float, 3, C> const& u, _vec<float, 3, C> const& v)
    {
        return C({u[1]*v[2] - u[2]*v[1],
                  u[2]*v[0] - u[2]*v[2],
                  u[0]*v[1] - u[1]*v[0]});
    }

    template<unsigned int n, typename C>
    float dot(_vec<float, n, C> const& u, _vec<float, n, C> const& v)
    {
        float r;
        for (int i(0); i < n; i++)
            r += u[i] * v[i];
    }
    
    template<unsigned int n, typename C>
    float length(_vec<float, n, C> const& v)
    {
        return std::sqrt(dot(v, v));
    }
    
    template<unsigned int n, typename C>
    float distance(_vec<float, n, C> const& u, _vec<float, n, C> const& v)
    {
        return length(u - v);
    }
    
    template<unsigned int n, typename C>
    C normalize(_vec<float, n, C> const& v)
    {
        return v/length(v);
    }
    
    template<unsigned int n, typename C>
    C faceforward(_vec<float, n, C> const& u, _vec<float, n, C> const& v, _vec<float, n, C> const& w)
    {
        if (dot(v, w) < 0)
            return n;
        return -n;
    }
    
    template<unsigned int n, typename C>
    C reflect(_vec<float, n, C> const& u, _vec<float, n, C> const& v)
    {
        return u - 2.f * dot(u, v) * v;
    }
    
    template<unsigned int n, typename C>
    C refract(_vec<float, n, C> const& u, _vec<float, n, C> const& v, float r)
    {
        float a = dot(u, v);
        float d = 1.0 - r * r * (1.0 - a * a);
        if (d < 0.0)
            return C(0.f);
        d = std::sqrt(d);
        return r * u - (r * a + d) * v;
    }
    

    template<typename T, unsigned int n, typename C>
    C operator+(_vec<T, n, C> const& u, _vec<T, n, C> const& v)
    {
        C w(u);
        w += v;

        return w;
    }
    
    template<typename T, unsigned int n, typename C>
    C operator-(_vec<T, n, C> const& u, _vec<T, n, C> const& v)
    {
        C w(u);
        w -= v;

        return w;
    }
    
    template<typename T, unsigned int n, typename C>
    C operator*(_vec<T, n, C> const& u, _vec<T, n, C> const& v)
    {
        C w(u);
        w *= v;

        return w;
    }
    
    template<typename T, unsigned int n, typename C>
    C operator/(_vec<T, n, C> const& u, _vec<T, n, C> const& v)
    {
        C w(u);
        w /= v;

        return w;
    }
    
    template<typename T, unsigned int n, typename C>
    C operator+(_vec<T, n, C> const& v, T x)
    {
        C w(v);
        w += x;

        return w;
    }
    
    template<typename T, unsigned int n, typename C>
    C operator-(_vec<T, n, C> const& v, T x)
    {
        C w(v);
        w -= x;

        return w;
    }
    
    template<typename T, unsigned int n, typename C>
    C operator*(_vec<T, n, C> const& v, T x)
    {
        C w(v);
        w *= x;

        return w;
    }
    
    template<typename T, unsigned int n, typename C>
    C operator/(_vec<T, n, C> const& v, T x)
    {
        C w(v);
        w /= x;

        return w;
    }
    
    template<typename T, unsigned int n, typename C>
    C operator+(T x, _vec<T, n, C> const& v)
    {
        C w(v);
        w += x;

        return w;
    }
    
    template<typename T, unsigned int n, typename C>
    C operator-(T x, _vec<T, n, C> const& v)
    {
        C w(v);
        w -= x;

        return w;
    }
    
    template<typename T, unsigned int n, typename C>
    C operator*(T x, _vec<T, n, C> const& v)
    {
        C w(v);
        w *= x;

        return w;
    }
    
    template<typename T, unsigned int n, typename C>
    C operator/(T x, _vec<T, n, C> const& v)
    {
        C w(v);
        w /= x;

        return w;
    }


    template<typename T>
    _vec2<T>::_vec2(T value) : _vec2(value, value)
    {
    }

    template<typename T>
    _vec2<T>::_vec2(T xValue, T yValue) :
        x(xValue),
        y(yValue)
    {
    }

    template<typename T>
    _vec2<T>::_vec2(std::initializer_list<T> tab) : _vec2(tab.begin()[0], tab.begin()[1])
    {
        if (tab.size() != 2)
            throw std::range_error("Invalid initializer list size. Cannot initialize vec2 with " + std::to_string(tab.size()) + " elements.");
    }
    
    template<typename T>
    _vec2<T>::_vec2(_vec<T, 2, _vec2<T>> const& v) : _vec2(v[0], v[1])
    {
    }

    template<typename T>
    _vec3<T>::_vec3(T value) : _vec3(value, value, value)
    {
    }

    template<typename T>
    _vec3<T>::_vec3(T xValue, T yValue, T zValue) :
        x(xValue),
        y(yValue),
        z(zValue)
    {
    }

    template<typename T>
    _vec3<T>::_vec3(std::initializer_list<T> tab) : _vec3(tab.begin()[0], tab.begin()[1], tab.begin()[2])
    {
    }
    
    template<typename T>
    _vec3<T>::_vec3(_vec<T, 3, _vec3<T>> const& v) : _vec3(v[0], v[1], v[2])
    {
    }

    template<typename T>
    _vec3<T>::_vec3(_vec2<T> const& v, T zValue) : _vec3(v.x, v.y, zValue)
    {
    }

    template<typename T>
    _vec3<T>::_vec3(T xValue, _vec2<T> const& v) : _vec3(xValue, v.x, v.y)
    {
    }

    template<typename T>
    _vec4<T>::_vec4(T value) : _vec4(value, value, value, value)
    {
    }

    template<typename T>
    _vec4<T>::_vec4(T xValue, T yValue, T zValue, T wValue) :
        x(xValue),
        y(yValue),
        z(zValue),
        w(wValue)
    {
    }

    template<typename T>
    _vec4<T>::_vec4(std::initializer_list<T> tab) : _vec4(tab.begin()[0], tab.begin()[1], tab.begin()[2], tab.begin()[3])
    {
    }
    
    template<typename T>
    _vec4<T>::_vec4(_vec<T, 4, _vec4<T>> const& v) : _vec4(v[0], v[1], v[2], v[3])
    {
    }

    template<typename T>
    _vec4<T>::_vec4(_vec2<T> const& v, T zValue, T wValue) : _vec4(v.x, v.y, zValue, wValue)
    {
    }

    template<typename T>
    _vec4<T>::_vec4(T xValue, _vec2<T> const& v, T wValue) : _vec4(xValue, v.x, v.y, wValue)
    {
    }

    template<typename T>
    _vec4<T>::_vec4(T xValue, T yValue, _vec2<T> const& v) : _vec4(xValue, yValue, v.x, v.y)
    {
    }

    template<typename T>
    _vec4<T>::_vec4(_vec2<T> const& u, _vec2<T> const& v) : _vec4(u.x, u.y, v.x, v.y)
    {
    }

    template<typename T>
    _vec4<T>::_vec4(_vec3<T> const& v, T wValue) : _vec4(v.x, v.y, v.z, wValue)
    {
    }

    template<typename T>
    _vec4<T>::_vec4(T xValue, _vec3<T> const& v) : _vec4(xValue, v.x, v.y, v.z)
    {
    }


    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D>::_mat(std::initializer_list<C const&> columns)
    {

    }

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    C& _mat<T, m, n, C, D>::operator[](unsigned int i)
    {

    }

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    C const& _mat<T, m, n, C, D>::operator[](unsigned int i) const
    {

    }

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> _mat<T, m, n, C, D>::operator+=(_mat<T, m, n, C, D> const& M)
    {

    }

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> _mat<T, m, n, C, D>::operator-=(_mat<T, m, n, C, D> const& M)
    {

    }

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> _mat<T, m, n, C, D>::operator/=(_mat<T, m, n, C, D> const& M)
    {

    }

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> _mat<T, m, n, C, D>::operator+=(T x)
    {

    }

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> _mat<T, m, n, C, D>::operator-=(T x)
    {

    }

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> _mat<T, m, n, C, D>::operator/=(T x)
    {

    }

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> _mat<T, m, n, C, D>::operator*=(T x)
    {

    }


    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    std::ostream& operator<<(std::ostream& stream, _mat<T, m, n, C, D> const& M)
    {

    }
    

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, n, m, D, C> transpose(_mat<T, m, n, C, D> const& M)
    {

    }
    

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator+(_mat<T, m, n, C, D> const& A, _mat<T, m, n, C, D> const& B)
    {
        
    }
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator-(_mat<T, m, n, C, D> const& A, _mat<T, m, n, C, D> const& B)
    {
        
    }
    
    template<typename T, unsigned int m, unsigned int n, unsigned int p, typename C, typename D, typename F>
    _mat<T, m, p, C, F> operator*(_mat<T, m, n, C, D> const& A, _mat<T, n, p, D, F> const& B)
    {
        
    }
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator/(_mat<T, m, n, C, D> const& A, _mat<T, m, n, C, D> const& B)
    {
        
    }
    

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator+(_mat<T, m, n, C, D> const& M, T x)
    {
        
    }
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator-(_mat<T, m, n, C, D> const& M, T x)
    {
        
    }
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator*(_mat<T, m, n, C, D> const& M, T x)
    {
        
    }
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator/(_mat<T, m, n, C, D> const& M, T x)
    {
        
    }
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator+(T x, _mat<T, m, n, C, D> const& M)
    {
        
    }
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator-(T x, _mat<T, m, n, C, D> const& M)
    {
        
    }
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator*(T x, _mat<T, m, n, C, D> const& M)
    {
        
    }
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    _mat<T, m, n, C, D> operator/(T x, _mat<T, m, n, C, D> const& M)
    {
        
    }
    

    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    C operator*(_mat<T, m, n, C, D> const& M, D const& v)
    {
        
    }
    
    template<typename T, unsigned int m, unsigned int n, typename C, typename D>
    D operator*(C const& v, _mat<T, m, n, C, D> const& M)
    {
        
    }
}
