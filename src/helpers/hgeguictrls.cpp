/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeGUI default controls implementation
*/


#include "../../include/hgeguictrls.h"
#include <string.h>
#include <stdio.h>


/*
** hgeGUIText
*/

hgeGUIText::hgeGUIText(const int _id, const float x, const float y,
                       const float w, const float h, hgeFont* fnt)
: align_(0) {
    id = _id;
    bStatic = true;
    bVisible = true;
    bEnabled = true;
    rect.Set(x, y, x + w, y + h);

    font_ = fnt;
    tx_ = x;
    ty_ = y + (h - fnt->GetHeight()) / 2.0f;

    text_[0] = 0;
}

void hgeGUIText::SetMode(const int _align) {
    align_ = _align;
    if (align_ == HGETEXT_RIGHT) {
        tx_ = rect.x2;
    }
    else if (align_ == HGETEXT_CENTER) {
        tx_ = (rect.x1 + rect.x2) / 2.0f;
    }
    else {
        tx_ = rect.x1;
    }
}

void hgeGUIText::SetText(const char* _text) {
    strcpy(text_, _text);
}

void hgeGUIText::printf(const char* format, ...) {
    vsprintf(text_, format, reinterpret_cast<char *>(&format) + sizeof(format));
}

void hgeGUIText::Render() {
    font_->SetColor(color);
    font_->Render(tx_, ty_, align_, text_);
}

/*
** hgeGUIButton
*/

hgeGUIButton::hgeGUIButton(const int _id, const float x, const float y,
                           const float w, const float h, const HTEXTURE tex,
                           const float tx, const float ty): bOldState(false) {
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
    if (sprUp) {
        delete sprUp;
    }
    if (sprDown) {
        delete sprDown;
    }
}

void hgeGUIButton::Render() {
    if (bPressed) {
        sprDown->Render(rect.x1, rect.y1);
    }
    else {
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
    }
    else {
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
    if (spr_slider_) {
        delete spr_slider_;
    }
}

void hgeGUISlider::SetValue(const float _fVal) {
    if (_fVal < min_) {
        val_ = min_;
    }
    else if (_fVal > max_) {
        val_ = max_;
    }
    else {
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

    if (vertical_)
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
    else
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
        }
        else {
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
                             const float w, const float h, hgeFont* fnt,
                             const uint32_t tColor, const uint32_t thColor,
                             const uint32_t hColor) {
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
    items_2_ = nullptr;
    items_ = 0;

    selected_item_ = 0;
    top_item_ = 0;
    mx_ = 0;
    my_ = 0;
}

hgeGUIListbox::~hgeGUIListbox() {
    Clear();
    if (spr_highlight_) {
        delete spr_highlight_;
    }
}


int hgeGUIListbox::AddItem(char* item) {
    hgeGUIListboxItem *pItem = items_2_, *pPrev = nullptr, *pNew;

    pNew = new hgeGUIListboxItem;
    memcpy(pNew->text, item, min(sizeof(pNew->text), strlen(item)+1));
    pNew->text[sizeof(pNew->text) - 1] = '\0';
    pNew->next = nullptr;

    while (pItem) {
        pPrev = pItem;
        pItem = pItem->next;
    }

    if (pPrev) {
        pPrev->next = pNew;
    }
    else {
        items_2_ = pNew;
    }
    items_++;

    return items_ - 1;
}

void hgeGUIListbox::DeleteItem(const int n) {
    hgeGUIListboxItem *pItem = items_2_, *pPrev = nullptr;

    if (n < 0 || n >= GetNumItems()) {
        return;
    }

    for (int i = 0; i < n; i++) {
        pPrev = pItem;
        pItem = pItem->next;
    }

    if (pPrev) {
        pPrev->next = pItem->next;
    }
    else {
        items_2_ = pItem->next;
    }

    delete pItem;
    items_--;
}

char* hgeGUIListbox::GetItemText(const int n) {
    hgeGUIListboxItem* pItem = items_2_;

    if (n < 0 || n >= GetNumItems()) {
        return nullptr;
    }

    for (int i = 0; i < n; i++) {
        pItem = pItem->next;
    }

    return pItem->text;
}

void hgeGUIListbox::Clear() {
    hgeGUIListboxItem* pItem = items_2_;

    while (pItem) {
        hgeGUIListboxItem* pNext = pItem->next;
        delete pItem;
        pItem = pNext;
    }

    items_2_ = nullptr;
    items_ = 0;
}

void hgeGUIListbox::Render() {
    int i;
    hgeGUIListboxItem* pItem = items_2_;

    for (i = 0; i < top_item_; i++) {
        pItem = pItem->next;
    }
    for (i = 0; i < GetNumRows(); i++) {
        if (i >= items_) {
            return;
        }

        if (top_item_ + i == selected_item_) {
            spr_highlight_->Render(rect.x1, rect.y1 + i * font_->GetHeight());
            font_->SetColor(text_highlight_color_);
        }
        else {
            font_->SetColor(text_color_);
        }

        font_->Render(rect.x1 + 3, rect.y1 + i * font_->GetHeight(), HGETEXT_LEFT, pItem->text);
        pItem = pItem->next;
    }
}

bool hgeGUIListbox::MouseLButton(const bool b_down) {

    if (b_down) {
        const auto n_item = top_item_ + int(my_) / int(font_->GetHeight());
        if (n_item < items_) {
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
        if (selected_item_ < items_ - 1) {
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
    }
    return false;
}
