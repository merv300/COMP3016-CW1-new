#pragma once

#include <cmath>

namespace rtz {

// Minimal 2D vector used for positions, velocities and directions.
// Screen space follows SDL's convention: +x is right, +y is down.
struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float xValue, float yValue) : x(xValue), y(yValue) {}

    Vec2 operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
    Vec2 operator-(const Vec2& other) const { return {x - other.x, y - other.y}; }
    Vec2 operator*(float scalar) const { return {x * scalar, y * scalar}; }

    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    float LengthSquared() const { return x * x + y * y; }
    float Length() const { return std::sqrt(LengthSquared()); }

    Vec2 Normalized() const {
        const float length = Length();
        if (length <= 0.000001f) {
            return {0.0f, 0.0f};
        }
        return {x / length, y / length};
    }
};

// Converts a direction expressed in screen space into world space for the
// current view rotation. The world is presented rotated clockwise by
// (quarterTurns * 90) degrees, so input has to be rotated the opposite way.
// This is what keeps "up on screen" meaning "up on screen" at every rotation.
inline Vec2 WorldFromScreen(const Vec2& screenDirection, int quarterTurns) {
    const int turns = ((quarterTurns % 4) + 4) % 4;
    switch (turns) {
    case 1:
        return {screenDirection.y, -screenDirection.x};
    case 2:
        return {-screenDirection.x, -screenDirection.y};
    case 3:
        return {-screenDirection.y, screenDirection.x};
    default:
        return screenDirection;
    }
}

} // namespace rtz
