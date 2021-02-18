/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// hgeColor*** helper classes implementation
//

#include <hgecolor.h>
#include <cmath>
#include <algorithm>
#undef min
#undef max

void hgeColorHSV::SetHWColor(uint32_t col) {
  a = (col >> 24) / 255.0f;
  auto r = (float)((uint8_t)(col >> 16)) * ONE_DIV_255;
  auto g = (float)((uint8_t)(col >> 8)) * ONE_DIV_255;
  auto b = (float)((uint8_t)col) * ONE_DIV_255;

  const auto minv = std::min(std::min(r, g), b);
  const auto maxv = std::max(std::max(r, g), b);
  const auto delta = maxv - minv;

  v = maxv;

  if (delta == 0) {
    h = 0;
    s = 0;
  } else {
    s = delta / maxv;
    const auto del_r = (((maxv - r) / 6) + (delta / 2)) / delta;
    const auto del_g = (((maxv - g) / 6) + (delta / 2)) / delta;
    const auto del_b = (((maxv - b) / 6) + (delta / 2)) / delta;

    if (r == maxv) {
      h = del_b - del_g;
    } else if (g == maxv) {
      h = (1.0f / 3.0f) + del_r - del_b;
    } else if (b == maxv) {
      h = (2.0f / 3.0f) + del_g - del_r;
    }

    if (h < 0) {
      h += 1;
    }
    if (h > 1) {
      h -= 1;
    }
  }
}

uint32_t hgeColorHSV::GetHWColor() const {
  float r;
  float g;
  float b;

  if (s == 0) {
    r = v;
    g = v;
    b = v;
  } else {
    auto xh = h * 6;
    if (xh == 6) {
      xh = 0;
    }
    const auto i = floorf(xh);
    const auto p1 = v * (1 - s);
    const auto p2 = v * (1 - s * (xh - i));
    const auto p3 = v * (1 - s * (1 - (xh - i)));

    if (i == 0) {
      r = v;
      g = p3;
      b = p1;
    } else if (i == 1) {
      r = p2;
      g = v;
      b = p1;
    } else if (i == 2) {
      r = p1;
      g = v;
      b = p3;
    } else if (i == 3) {
      r = p1;
      g = p2;
      b = v;
    } else if (i == 4) {
      r = p3;
      g = p1;
      b = v;
    } else {
      r = v;
      g = p1;
      b = p2;
    }
  }

  return (uint32_t(a * 255.0f) << 24)
         + (uint32_t(r * 255.0f) << 16)
         + (uint32_t(g * 255.0f) << 8)
         + uint32_t(b * 255.0f);
}
