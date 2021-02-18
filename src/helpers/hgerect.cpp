/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// hgeRect helper class implementation
//


#include <hgerect.h>
#include <cmath>


void hgeRect::Encapsulate(const float x, const float y) {
  if (is_clean_) {
    x1 = x2 = x;
    y1 = y2 = y;
    is_clean_ = false;
  } else {
    if (x < x1) {
      x1 = x;
    }
    if (x > x2) {
      x2 = x;
    }
    if (y < y1) {
      y1 = y;
    }
    if (y > y2) {
      y2 = y;
    }
  }
}

bool hgeRect::TestPoint(const float x, const float y) const {
  if (x >= x1 && x < x2 && y >= y1 && y < y2) {
    return true;
  }

  return false;
}

bool hgeRect::Intersect(const hgeRect *rect) const {
  if (fabs(x1 + x2 - rect->x1 - rect->x2) < (x2 - x1 + rect->x2 - rect->x1))
    if (fabs(y1 + y2 - rect->y1 - rect->y2) < (y2 - y1 + rect->y2 - rect->y1)) {
      return true;
    }

  return false;
}
