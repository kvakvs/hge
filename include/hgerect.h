/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once

class hgeRect {
public:
    float x1, y1, x2, y2;

    hgeRect(const float _x1, const float _y1, const float _x2, const float _y2) {
      x1 = _x1;
      y1 = _y1;
      x2 = _x2;
      y2 = _y2;
      is_clean_ = false;
    }

    hgeRect() {
      is_clean_ = true;
    }

    void Clear() {
      is_clean_ = true;
    }

    bool IsClean() const {
      return is_clean_;
    }

    void Set(const float _x1, const float _y1, const float _x2, const float _y2) {
      x1 = _x1;
      x2 = _x2;
      y1 = _y1;
      y2 = _y2;
      is_clean_ = false;
    }

    void SetRadius(const float x, const float y, const float r) {
      x1 = x - r;
      x2 = x + r;
      y1 = y - r;
      y2 = y + r;
      is_clean_ = false;
    }

    void Encapsulate(float x, float y);

    bool TestPoint(float x, float y) const;

    bool Intersect(const hgeRect *rect) const;

private:
    bool is_clean_;
};
