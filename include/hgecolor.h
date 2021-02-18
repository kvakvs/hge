/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once

#include <hge.h>

inline void ColorClamp(float &x) {
  if (x < 0.0f) x = 0.0f;
  if (x > 1.0f) x = 1.0f;
}


class hgeColorRGB {
    static constexpr float ONE_DIV_255 = 1.0f / 255.0f;
public:
    float r, g, b, a;

    hgeColorRGB(const float _r, const float _g, const float _b, const float _a) noexcept {
      r = _r;
      g = _g;
      b = _b;
      a = _a;
    }

    hgeColorRGB(const uint32_t col) noexcept {
      SetHWColor(col);
    }

    hgeColorRGB() noexcept {
      r = g = b = a = 0.0f;
    }

    hgeColorRGB operator-(const hgeColorRGB &c) const {
      return hgeColorRGB(r - c.r, g - c.g, b - c.b, a - c.a);
    }

    hgeColorRGB operator+(const hgeColorRGB &c) const {
      return hgeColorRGB(r + c.r, g + c.g, b + c.b, a + c.a);
    }

    hgeColorRGB operator*(const hgeColorRGB &c) const {
      return hgeColorRGB(r * c.r, g * c.g, b * c.b, a * c.a);
    }

    hgeColorRGB &operator-=(const hgeColorRGB &c) {
      r -= c.r;
      g -= c.g;
      b -= c.b;
      a -= c.a;
      return *this;
    }

    hgeColorRGB &operator+=(const hgeColorRGB &c) {
      r += c.r;
      g += c.g;
      b += c.b;
      a += c.a;
      return *this;
    }

    bool operator==(const hgeColorRGB &c) const {
      return (r == c.r && g == c.g && b == c.b && a == c.a);
    }

    bool operator!=(const hgeColorRGB &c) const {
      return (r != c.r || g != c.g || b != c.b || a != c.a);
    }

    hgeColorRGB operator/(const float scalar) const {
      return hgeColorRGB(r / scalar, g / scalar, b / scalar, a / scalar);
    }

    hgeColorRGB operator*(const float scalar) const {
      return hgeColorRGB(r * scalar, g * scalar, b * scalar, a * scalar);
    }

    hgeColorRGB &operator*=(const float scalar) {
      r *= scalar;
      g *= scalar;
      b *= scalar;
      a *= scalar;
      return *this;
    }

    void Clamp() {
      ColorClamp(r);
      ColorClamp(g);
      ColorClamp(b);
      ColorClamp(a);
    }

    void SetHWColor(const uint32_t col) {
      a = (col >> 24) * ONE_DIV_255;
      r = ((uint8_t) (col >> 16)) * ONE_DIV_255;
      g = ((uint8_t) (col >> 8)) * ONE_DIV_255;
      b = ((uint8_t) col) * ONE_DIV_255;
    }

    hgeColor32 GetHWColor() const {
      return hgeColor32(uint8_t(a * 255.0f),
                        uint8_t(r * 255.0f),
                        uint8_t(g * 255.0f),
                        uint8_t(b * 255.0f));
    }
};

inline hgeColorRGB operator*(const float sc, const hgeColorRGB &c) {
  return c * sc;
}


class hgeColorHSV {
    static constexpr float ONE_DIV_255 = 1.0f / 255.0f;
public:
    float h, s, v, a;

    hgeColorHSV(const float _h, const float _s, const float _v, const float _a) {
      h = _h;
      s = _s;
      v = _v;
      a = _a;
    }

    hgeColorHSV(const uint32_t col) {
      SetHWColor(col);
    }

    hgeColorHSV() {
      h = s = v = a = 0;
    }

    hgeColorHSV operator-(const hgeColorHSV &c) const {
      return hgeColorHSV(h - c.h, s - c.s, v - c.v, a - c.a);
    }

    hgeColorHSV operator+(const hgeColorHSV &c) const {
      return hgeColorHSV(h + c.h, s + c.s, v + c.v, a + c.a);
    }

    hgeColorHSV operator*(const hgeColorHSV &c) const {
      return hgeColorHSV(h * c.h, s * c.s, v * c.v, a * c.a);
    }

    hgeColorHSV &operator-=(const hgeColorHSV &c) {
      h -= c.h;
      s -= c.s;
      v -= c.v;
      a -= c.a;
      return *this;
    }

    hgeColorHSV &operator+=(const hgeColorHSV &c) {
      h += c.h;
      s += c.s;
      v += c.v;
      a += c.a;
      return *this;
    }

    bool operator==(const hgeColorHSV &c) const {
      return (h == c.h && s == c.s && v == c.v && a == c.a);
    }

    bool operator!=(const hgeColorHSV &c) const {
      return (h != c.h || s != c.s || v != c.v || a != c.a);
    }

    hgeColorHSV operator/(const float scalar) const {
      return hgeColorHSV(h / scalar, s / scalar, v / scalar, a / scalar);
    }

    hgeColorHSV operator*(const float scalar) const {
      return hgeColorHSV(h * scalar, s * scalar, v * scalar, a * scalar);
    }

    hgeColorHSV &operator*=(const float scalar) {
      h *= scalar;
      s *= scalar;
      v *= scalar;
      a *= scalar;
      return *this;
    }

    void Clamp() {
      ColorClamp(h);
      ColorClamp(s);
      ColorClamp(v);
      ColorClamp(a);
    }

    void SetHWColor(uint32_t col);

    uint32_t GetHWColor() const;
};

inline hgeColorHSV operator*(const float sc, const hgeColorHSV &c) {
  return c * sc;
}

using hgeColor = hgeColorRGB;
