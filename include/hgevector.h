/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeVector helper class
*/


#pragma once


#include "hge.h"
#include <math.h>


class hgeVector {
public:
    float x, y;

    // Fast 1.0/sqrtf(float) routine
    static float InvSqrt(float x);

    hgeVector(const float _x, const float _y) {
        x = _x;
        y = _y;
    }

    hgeVector() {
        x = 0;
        y = 0;
    }

    hgeVector operator-() const {
        return hgeVector(-x, -y);
    }

    hgeVector operator-(const hgeVector& v) const {
        return hgeVector(x - v.x, y - v.y);
    }

    hgeVector operator+(const hgeVector& v) const {
        return hgeVector(x + v.x, y + v.y);
    }

    hgeVector& operator-=(const hgeVector& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    hgeVector& operator+=(const hgeVector& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    bool operator==(const hgeVector& v) const {
        return (x == v.x && y == v.y);
    }

    bool operator!=(const hgeVector& v) const {
        return (x != v.x || y != v.y);
    }

    hgeVector operator/(const float scalar) const {
        return hgeVector(x / scalar, y / scalar);
    }

    hgeVector operator*(const float scalar) const {
        return hgeVector(x * scalar, y * scalar);
    }

    hgeVector& operator*=(const float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    float Dot(const hgeVector* v) const {
        return x * v->x + y * v->y;
    }

    float Length() const {
        return sqrtf(Dot(this));
    }

    float Angle(const hgeVector* v = nullptr) const;

    void Clamp(const float max) {
        if (Length() > max) {
            Normalize();
            x *= max;
            y *= max;
        }
    }

    hgeVector* Normalize() {
        const auto rc = InvSqrt(Dot(this));
        x *= rc;
        y *= rc;
        return this;
    }

    hgeVector* Rotate(float a);
};

inline hgeVector operator*(const float s, const hgeVector& v) {
    return v * s;
}

inline float operator^(const hgeVector& v, const hgeVector& u) {
    return v.Angle(&u);
}

inline float operator%(const hgeVector& v, const hgeVector& u) {
    return v.Dot(&u);
}
