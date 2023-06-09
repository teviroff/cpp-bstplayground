#pragma once

#include <SFML/Graphics.hpp>
#include <array>


extern const float PI;

namespace auxillary
{
    template<class T>
    T clamp(T value, T min, T max)
    {
        if (value > max)
            return max;
        if (value < min)
            return min;
        return value;
    }

    template<class Arg, class Ret>
    Ret lerp(Arg arg, Arg l, Arg r, Ret lv, Ret rv)
    {
        if (arg > r)
            return rv;
        if (arg < l)
            return lv;
        return lv + (arg - l) / (r - l) * (rv - lv);
    }

    sf::Vector2f round(sf::Vector2f v);

    struct vec2
    {
        float x, y;

        vec2(float x = 0, float y = 0);
        vec2(const sf::Vector2f& v);

        vec2 operator+(const vec2& v) const;
        vec2& operator+=(const vec2& v);
        vec2 operator-(const vec2& v) const;
        vec2& operator-=(const vec2& v);
        vec2 operator*(const vec2& v) const;
        vec2 operator*(float s) const;
        vec2& operator*=(const vec2& v);
        vec2& operator*=(float s);

        vec2 Xconjugate() const;
        vec2 Yconjugate() const;

        operator sf::Vector2f() const;
    };

    struct BoundingBox
    {
        float left, right, top, bottom, width, height;
        auxillary::vec2 center;

        static BoundingBox CreateFromPosition(const vec2& position, const vec2& size);
        static BoundingBox CreateFromCenter(const vec2& center, const vec2& size);
        static BoundingBox CreateFromPoints(const vec2& a, const vec2& b);

        bool overlaps(const BoundingBox& other) const;
    };
}
