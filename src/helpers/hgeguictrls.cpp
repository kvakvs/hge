/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// hgeGUI default controls implementation
//

#include <hgeguictrls.h>
#include <cstring>
#include <cstdio>


/*
** hgeGUIText
*/

// NOLINTNEXTLINE
hgeGUIText::hgeGUIText(const int _id, const float x, const float y,
                       const float w, const float h,
                       hgeMUTABLE hgeFont *fnt)
        : align_(0) {
  id = _id;
  bStatic = true;
  bVisible = true;
  bEnabled = true;
  rect.Set(x, y, x + w, y + h);

  font_ = fnt;
  tx_ = x;
  ty_ = y + (h - fnt->GetHeight()) / 2.0f;
}

void hgeGUIText::SetMode(const int _align) {
  align_ = _align;
  if (align_ == HGETEXT_RIGHT) {
    tx_ = rect.x2;
  } else if (align_ == HGETEXT_CENTER) {
    tx_ = (rect.x1 + rect.x2) / 2.0f;
  } else {
    tx_ = rect.x1;
  }
}

void hgeGUIText::SetText(const char *t) {
  text_ = t;
}

void hgeGUIText::printf(const char *format, ...) {
  static char tmp[256];
  vsprintf(tmp, format, reinterpret_cast<char *>(&format) + sizeof(format));
  text_ = tmp;
}

void hgeGUIText::Render() {
  font_->SetColor(color);
  font_->Render(tx_, ty_, align_, text_.c_str());
}

/*
** hgeGUIButton
*/

hgeGUIButton::hgeGUIButton(const int _id, const float x, const float y,
                           const float w, const float h, const HTEXTURE tex,
                           const float tx, const float ty) : bOldState(false) {
  id = _id;
  bStatic = false;
  bVisible = true;
  bEnabled = true;
  rect.Set(x, y, x + w, y + h);

  bPressed = false;
  bTrigger = false;

  sprUp = new hgeSprite(tex, tx, ty, w, h);
  sprDown = new hgeSprite(tex, tx + w, ty, w, h);
}

hgeGUIButton::~hgeGUIButton() {
  delete sprUp;
  delete sprDown;
}

void hgeGUIButton::Render() {
  if (bPressed) {
    sprDown->Render(rect.x1, rect.y1);
  } else {
    sprUp->Render(rect.x1, rect.y1);
  }
}

bool hgeGUIButton::MouseLButton(const bool b_down) {
  if (b_down) {
    bOldState = bPressed;
    bPressed = true;
    return false;
  }
  if (bTrigger) {
    bPressed = !bOldState;
  } else {
    bPressed = false;
  }
  return true;
}

/*
** hgeGUISlider
*/

hgeGUISlider::hgeGUISlider(const int _id, const float x, const float y,
                           const float w, const float h,
                           const HTEXTURE tex, const float tx,
                           const float ty, const float sw,
                           const float sh, const bool vertical) {
  id = _id;
  bStatic = false;
  bVisible = true;
  bEnabled = true;
  pressed_ = false;
  vertical_ = vertical;
  rect.Set(x, y, x + w, y + h);

  mode_ = HGESLIDER_BAR;
  min_ = 0;
  max_ = 100;
  val_ = 50;
  sl_w_ = sw;
  sl_h_ = sh;

  spr_slider_ = new hgeSprite(tex, tx, ty, sw, sh);
}

hgeGUISlider::~hgeGUISlider() {
  delete spr_slider_;
}

void hgeGUISlider::SetValue(const float _fVal) {
  if (_fVal < min_) {
    val_ = min_;
  } else if (_fVal > max_) {
    val_ = max_;
  } else {
    val_ = _fVal;
  }
}

void hgeGUISlider::Render() {
  float x1 = 0;
  float y1 = 0;
  float x2 = 0;
  float y2 = 0;

  const auto xx = rect.x1 + (rect.x2 - rect.x1) * (val_ - min_) / (max_ - min_);
  const auto yy = rect.y1 + (rect.y2 - rect.y1) * (val_ - min_) / (max_ - min_);

  if (vertical_) {
    switch (mode_) {
      case HGESLIDER_BAR:
        x1 = rect.x1;
        y1 = rect.y1;
        x2 = rect.x2;
        y2 = yy;
        break;
      case HGESLIDER_BARRELATIVE:
        x1 = rect.x1;
        y1 = (rect.y1 + rect.y2) / 2;
        x2 = rect.x2;
        y2 = yy;
        break;
      case HGESLIDER_SLIDER:
        x1 = (rect.x1 + rect.x2 - sl_w_) / 2;
        y1 = yy - sl_h_ / 2;
        x2 = (rect.x1 + rect.x2 + sl_w_) / 2;
        y2 = yy + sl_h_ / 2;
        break;
    }
  } else {
    switch (mode_) {
      case HGESLIDER_BAR:
        x1 = rect.x1;
        y1 = rect.y1;
        x2 = xx;
        y2 = rect.y2;
        break;
      case HGESLIDER_BARRELATIVE:
        x1 = (rect.x1 + rect.x2) / 2;
        y1 = rect.y1;
        x2 = xx;
        y2 = rect.y2;
        break;
      case HGESLIDER_SLIDER:
        x1 = xx - sl_w_ / 2;
        y1 = (rect.y1 + rect.y2 - sl_h_) / 2;
        x2 = xx + sl_w_ / 2;
        y2 = (rect.y1 + rect.y2 + sl_h_) / 2;
        break;
    }
  }

  spr_slider_->RenderStretch(x1, y1, x2, y2);
}

bool hgeGUISlider::MouseLButton(const bool b_down) {
  pressed_ = b_down;
  return false;
}

bool hgeGUISlider::MouseMove(float x, float y) {
  if (pressed_) {
    if (vertical_) {
      if (y > rect.y2 - rect.y1) {
        y = rect.y2 - rect.y1;
      }
      if (y < 0) {
        y = 0;
      }
      val_ = min_ + (max_ - min_) * y / (rect.y2 - rect.y1);
    } else {
      if (x > rect.x2 - rect.x1) {
        x = rect.x2 - rect.x1;
      }
      if (x < 0) {
        x = 0;
      }
      val_ = min_ + (max_ - min_) * x / (rect.x2 - rect.x1);
    }
    return true;
  }

  return false;
}


/*
** hgeGUIListbox
*/

hgeGUIListbox::hgeGUIListbox(const int _id, const float x, const float y,
                             const float w, const float h,
                             hgeMUTABLE hgeFont *fnt,
                             const hgeColor32 tColor,
                             const hgeColor32 thColor,
                             const hgeColor32 hColor) {
  id = _id;
  bStatic = false;
  bVisible = true;
  bEnabled = true;
  rect.Set(x, y, x + w, y + h);
  font_ = fnt;
  spr_highlight_ = new hgeSprite(0, 0, 0, w, fnt->GetHeight());
  spr_highlight_->SetColor(hColor);
  text_color_ = tColor;
  text_highlight_color_ = thColor;

  selected_item_ = 0;
  top_item_ = 0;
  mx_ = 0;
  my_ = 0;
}

hgeGUIListbox::~hgeGUIListbox() {
  Clear();
  delete spr_highlight_;
}


size_t hgeGUIListbox::AddItem(const char *item) {
  list_items_.emplace_back(item);
  return list_items_.size();
}


void hgeGUIListbox::DeleteItem(const size_t n) {
  if (n < 0 || n >= GetNumItems()) {
    return;
  }

  auto remove_iter = list_items_.begin();
  for (size_t i = 0; i < n; i++) {
    remove_iter++;
  }

  if (remove_iter != list_items_.end()) {
    list_items_.erase(remove_iter);
  }
}

const char *hgeGUIListbox::GetItemText(const size_t n) {
  if (n < 0 || n >= GetNumItems()) {
    return nullptr;
  }

  auto item_iter = list_items_.cbegin();
  for (size_t i = 0; i < n; i++) {
    item_iter++;
  }

  if (item_iter != list_items_.end()) {
    return item_iter->c_str();
  }
  return nullptr; // not found, should not arrive at this line
}

void hgeGUIListbox::Clear() {
  list_items_.clear();
}

void hgeGUIListbox::Render() {
  auto item_iter = list_items_.cbegin();

  for (size_t i = 0; i < top_item_; i++) {
    item_iter++;
  }

  for (size_t i = 0; i < GetNumRows(); i++) {
    if (item_iter == list_items_.cend()) { return; }

    if (top_item_ + i == selected_item_) {
      spr_highlight_->Render(rect.x1, rect.y1 + i * font_->GetHeight());
      font_->SetColor(text_highlight_color_);
    } else {
      font_->SetColor(text_color_);
    }

    font_->Render(rect.x1 + 3, rect.y1 + i * font_->GetHeight(),
                  HGETEXT_LEFT,
                  item_iter->c_str());
    item_iter++;
  }
}

bool hgeGUIListbox::MouseLButton(const bool b_down) {
  if (b_down) {
    const auto n_item = top_item_ + int(my_) / int(font_->GetHeight());

    if (n_item < list_items_.size()) {
      selected_item_ = n_item;
      return true;
    }
  }
  return false;
}


bool hgeGUIListbox::MouseWheel(const int n_notches) {
  top_item_ -= n_notches;
  if (top_item_ < 0) {
    top_item_ = 0;
  }
  if (top_item_ > GetNumItems() - GetNumRows()) {
    top_item_ = GetNumItems() - GetNumRows();
  }

  return true;
}

bool hgeGUIListbox::KeyClick(const int key, int chr) {
  switch (key) {
    case HGEK_DOWN:
      if (selected_item_ < list_items_.size() - 1) {
        selected_item_++;
        if (selected_item_ > top_item_ + GetNumRows() - 1) {
          top_item_ = selected_item_ - GetNumRows() + 1;
        }
        return true;
      }
      break;

    case HGEK_UP:
      if (selected_item_ > 0) {
        selected_item_--;
        if (selected_item_ < top_item_) {
          top_item_ = selected_item_;
        }
        return true;
      }
      break;

    default:
      break;
  }
  return false;
}
