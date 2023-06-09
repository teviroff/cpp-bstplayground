#include "auxillary.h"


const float PI = 3.141592653589793f;

namespace auxillary
{
    sf::Vector2f round(sf::Vector2f v)
    {
        return sf::Vector2f(std::round(v.x), std::round(v.y));
    }

    #pragma region vec2
    vec2::vec2(float x, float y) : x(x), y(y) {}
    vec2::vec2(const sf::Vector2f& v) : x(v.x), y(v.y) {}

    vec2 vec2::operator+(const vec2& v) const
    {
        return vec2(x + v.x, y + v.y);
    }

    vec2& vec2::operator+=(const vec2& v)
    {
        return x += v.x, y += v.y, *this;
    }

    vec2 vec2::operator-(const vec2& v) const
    {
        return vec2(x - v.x, y - v.y);
    }

    vec2& vec2::operator-=(const vec2& v)
    {
        return x -= v.x, y -= v.y, *this;
    }

    vec2 vec2::operator*(const vec2& v) const
    {
        return vec2(x * v.x, y * v.y);
    }

    vec2 vec2::operator*(float s) const
    {
        return vec2(x * s, y * s);
    }

    vec2& vec2::operator*=(const vec2& v)
    {
        return x *= v.x, y *= v.y, *this;
    }

    vec2& vec2::operator*=(float s)
    {
        return x *= s, y *= s, *this;
    }

    vec2 vec2::Xconjugate() const
    {
        return vec2(-x, y);
    }

    vec2 vec2::Yconjugate() const
    {
        return vec2(x, -y);
    }

    vec2::operator sf::Vector2f() const
    {
        return sf::Vector2f(x, y);
    }
    #pragma endregion

    #pragma region BoundingBox
    BoundingBox BoundingBox::CreateFromPosition(const vec2& position, const vec2& size)
    {
        float left = position.x, bottom = position.y, 
            right = position.x + size.x, top = position.y + size.y,
            width = std::fabs(size.x), height = std::fabs(size.y);
        if (left > right)
            std::swap(left, right);
        if (top < bottom)
            std::swap(top, bottom);
        auxillary::vec2 center((left + right) / 2, (bottom + top) / 2);
        return BoundingBox{left, right, top, bottom, width, height, center};
    }

    BoundingBox BoundingBox::CreateFromCenter(const vec2& center, const vec2& size)
    {
        float left = center.x - size.x / 2, bottom = center.y - size.y / 2,
            right = center.x + size.x / 2, top = center.y + size.y / 2,
            width = std::fabs(size.x), height = std::fabs(size.y);
        if (left > right)
            std::swap(left, right);
        if (top < bottom)
            std::swap(top, bottom);
        return BoundingBox{ left, right, top, bottom, width, height, center};
    }

    BoundingBox BoundingBox::CreateFromPoints(const vec2& a, const vec2& b)
    {
        float left = a.x, bottom = a.y, right = b.x, top = b.y,
            width = std::fabs(a.x - b.x), height = std::fabs(a.y - b.y);
        if (left > right)
            std::swap(left, right);
        if (top < bottom)
            std::swap(top, bottom);
        auxillary::vec2 center((left + right) / 2, (bottom + top) / 2);
        return BoundingBox{ left, right, top, bottom, width, height, center };
    }

    bool BoundingBox::overlaps(const BoundingBox& other) const
    {
        if (right <= other.left || left >= other.right || bottom >= other.top || top <= other.bottom)
            return false;
        return true;
    }
    #pragma endregion
}
