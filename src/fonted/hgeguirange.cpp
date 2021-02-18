/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Bitmap Font Builder
//

#include "hgeguirange.h"


hgeGUIRange::hgeGUIRange(const int _id, const float x, const float y,
                         const float w, const float h, const int _nrows,
                         const int _ncols, const hgeColor32 color) {
  id = _id;
  bStatic = false;
  bVisible = true;
  bEnabled = true;
  rect.Set(x, y, x + w, y + h);

  spr_ = new hgeSprite(0, 0, 0, 1, 1);
  spr_->SetColor(color);
  rows_ = _nrows;
  cols_ = _ncols;
  nfirst_ = nlast_ = 0;

  mx_ = 0;
  my_ = 0;
  pressed_ = false;
}

hgeGUIRange::~hgeGUIRange() {
  delete spr_;
}

void hgeGUIRange::SetRange(int first, int last) {
  if (first < 0) {
    first = 0;
  }
  if (last >= rows_ * cols_) {
    last = rows_ * cols_ - 1;
  }
  nfirst_ = first;
  nlast_ = last;
}

void hgeGUIRange::GetRange(int *first, int *last) const {
  if (nlast_ >= nfirst_) {
    *first = nfirst_;
    *last = nlast_;
  } else {
    *first = nlast_;
    *last = nfirst_;
  }
}

void hgeGUIRange::Render() {
  const auto cw = (rect.x2 - rect.x1) / cols_;
  const auto ch = (rect.y2 - rect.y1) / rows_;
  int first, last;

  if (nlast_ >= nfirst_) {
    first = nfirst_;
    last = nlast_;
  } else {
    first = nlast_;
    last = nfirst_;
  }

  const auto xf = rect.x1 + cw * static_cast<float>(first % cols_);
  const auto yf = rect.y1 + ch * static_cast<float>(first / cols_);

  const auto xl = rect.x1 + cw * static_cast<float>(last % cols_);
  const auto yl = rect.y1 + ch * static_cast<float>(last / cols_);

  if (yf == yl) {
    spr_->RenderStretch(xf, yf, xl + cw, yf + ch);
  } else {
    spr_->RenderStretch(xf, yf, rect.x2, yf + ch);
    spr_->RenderStretch(rect.x1, yl, xl + cw, yl + ch);
    if (yl - yf > 1) {
      spr_->RenderStretch(rect.x1, yf + ch, rect.x2, yl);
    }
  }

  //spr->RenderStretch(x1,y1,x2,y2);
}

bool hgeGUIRange::MouseMove(const float x, const float y) {
  mx_ = x;
  my_ = y;

  if (pressed_) {
    nlast_ = calc_point(mx_, my_);
    return true;
  }

  return false;
}

bool hgeGUIRange::MouseLButton(const bool b_down) {
  pressed_ = b_down;

  if (pressed_) {
    nfirst_ = calc_point(mx_, my_);
    nlast_ = nfirst_;
    return true;
  }
  return false;
}

int hgeGUIRange::calc_point(float x, float y) const {
  float cw = rect.x2 - rect.x1;
  float ch = rect.y2 - rect.y1;

  if (x < 0) {
    x = 0;
  }
  if (x > cw - 1) {
    x = cw - 1;
  }

  if (y < 0) {
    y = 0;
  }
  if (y > ch - 1) {
    y = ch - 1;
  }

  cw /= cols_;
  ch /= rows_;

  const int xcell = int(x) / int(cw);
  const int ycell = int(y) / int(ch);

  return ycell * cols_ + xcell;
}
