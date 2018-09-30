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
: align(0) {
    id = _id;
    bStatic = true;
    bVisible = true;
    bEnabled = true;
    rect.Set(x, y, x + w, y + h);

    font = fnt;
    tx = x;
    ty = y + (h - fnt->GetHeight()) / 2.0f;

    text[0] = 0;
}

void hgeGUIText::SetMode(const int _align) {
    align = _align;
    if (align == HGETEXT_RIGHT) {
        tx = rect.x2;
    }
    else if (align == HGETEXT_CENTER) {
        tx = (rect.x1 + rect.x2) / 2.0f;
    }
    else {
        tx = rect.x1;
    }
}

void hgeGUIText::SetText(const char* _text) {
    strcpy(text, _text);
}

void hgeGUIText::printf(const char* format, ...) {
    vsprintf(text, format, reinterpret_cast<char *>(&format) + sizeof(format));
}

void hgeGUIText::Render() {
    font->SetColor(color);
    font->Render(tx, ty, align, text);
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
    bPressed = false;
    bVertical = vertical;
    rect.Set(x, y, x + w, y + h);

    mode = HGESLIDER_BAR;
    fMin = 0;
    fMax = 100;
    fVal = 50;
    sl_w = sw;
    sl_h = sh;

    sprSlider = new hgeSprite(tex, tx, ty, sw, sh);
}

hgeGUISlider::~hgeGUISlider() {
    if (sprSlider) {
        delete sprSlider;
    }
}

void hgeGUISlider::SetValue(const float _fVal) {
    if (_fVal < fMin) {
        fVal = fMin;
    }
    else if (_fVal > fMax) {
        fVal = fMax;
    }
    else {
        fVal = _fVal;
    }
}

void hgeGUISlider::Render() {
    float x1 = 0;
    float y1 = 0;
    float x2 = 0;
    float y2 = 0;

    const auto xx = rect.x1 + (rect.x2 - rect.x1) * (fVal - fMin) / (fMax - fMin);
    const auto yy = rect.y1 + (rect.y2 - rect.y1) * (fVal - fMin) / (fMax - fMin);

    if (bVertical)
        switch (mode) {
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
            x1 = (rect.x1 + rect.x2 - sl_w) / 2;
            y1 = yy - sl_h / 2;
            x2 = (rect.x1 + rect.x2 + sl_w) / 2;
            y2 = yy + sl_h / 2;
            break;
        }
    else
        switch (mode) {
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
            x1 = xx - sl_w / 2;
            y1 = (rect.y1 + rect.y2 - sl_h) / 2;
            x2 = xx + sl_w / 2;
            y2 = (rect.y1 + rect.y2 + sl_h) / 2;
            break;
        }

    sprSlider->RenderStretch(x1, y1, x2, y2);
}

bool hgeGUISlider::MouseLButton(const bool b_down) {
    bPressed = b_down;
    return false;
}

bool hgeGUISlider::MouseMove(float x, float y) {
    if (bPressed) {
        if (bVertical) {
            if (y > rect.y2 - rect.y1) {
                y = rect.y2 - rect.y1;
            }
            if (y < 0) {
                y = 0;
            }
            fVal = fMin + (fMax - fMin) * y / (rect.y2 - rect.y1);
        }
        else {
            if (x > rect.x2 - rect.x1) {
                x = rect.x2 - rect.x1;
            }
            if (x < 0) {
                x = 0;
            }
            fVal = fMin + (fMax - fMin) * x / (rect.x2 - rect.x1);
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
                             const hgeU32 tColor, const hgeU32 thColor,
                             const hgeU32 hColor) {
    id = _id;
    bStatic = false;
    bVisible = true;
    bEnabled = true;
    rect.Set(x, y, x + w, y + h);
    font = fnt;
    sprHighlight = new hgeSprite(0, 0, 0, w, fnt->GetHeight());
    sprHighlight->SetColor(hColor);
    textColor = tColor;
    texthilColor = thColor;
    pItems = nullptr;
    nItems = 0;

    nSelectedItem = 0;
    nTopItem = 0;
    mx = 0;
    my = 0;
}

hgeGUIListbox::~hgeGUIListbox() {
    Clear();
    if (sprHighlight) {
        delete sprHighlight;
    }
}


int hgeGUIListbox::AddItem(char* item) {
    hgeGUIListboxItem *pItem = pItems, *pPrev = nullptr, *pNew;

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
        pItems = pNew;
    }
    nItems++;

    return nItems - 1;
}

void hgeGUIListbox::DeleteItem(const int n) {
    hgeGUIListboxItem *pItem = pItems, *pPrev = nullptr;

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
        pItems = pItem->next;
    }

    delete pItem;
    nItems--;
}

char* hgeGUIListbox::GetItemText(const int n) {
    hgeGUIListboxItem* pItem = pItems;

    if (n < 0 || n >= GetNumItems()) {
        return nullptr;
    }

    for (int i = 0; i < n; i++) {
        pItem = pItem->next;
    }

    return pItem->text;
}

void hgeGUIListbox::Clear() {
    hgeGUIListboxItem* pItem = pItems;

    while (pItem) {
        hgeGUIListboxItem* pNext = pItem->next;
        delete pItem;
        pItem = pNext;
    }

    pItems = nullptr;
    nItems = 0;
}

void hgeGUIListbox::Render() {
    int i;
    hgeGUIListboxItem* pItem = pItems;

    for (i = 0; i < nTopItem; i++) {
        pItem = pItem->next;
    }
    for (i = 0; i < GetNumRows(); i++) {
        if (i >= nItems) {
            return;
        }

        if (nTopItem + i == nSelectedItem) {
            sprHighlight->Render(rect.x1, rect.y1 + i * font->GetHeight());
            font->SetColor(texthilColor);
        }
        else {
            font->SetColor(textColor);
        }

        font->Render(rect.x1 + 3, rect.y1 + i * font->GetHeight(), HGETEXT_LEFT, pItem->text);
        pItem = pItem->next;
    }
}

bool hgeGUIListbox::MouseLButton(const bool b_down) {

    if (b_down) {
        const auto n_item = nTopItem + int(my) / int(font->GetHeight());
        if (n_item < nItems) {
            nSelectedItem = n_item;
            return true;
        }
    }
    return false;
}


bool hgeGUIListbox::MouseWheel(const int n_notches) {
    nTopItem -= n_notches;
    if (nTopItem < 0) {
        nTopItem = 0;
    }
    if (nTopItem > GetNumItems() - GetNumRows()) {
        nTopItem = GetNumItems() - GetNumRows();
    }

    return true;
}

bool hgeGUIListbox::KeyClick(const int key, int chr) {
    switch (key) {
    case HGEK_DOWN:
        if (nSelectedItem < nItems - 1) {
            nSelectedItem++;
            if (nSelectedItem > nTopItem + GetNumRows() - 1) {
                nTopItem = nSelectedItem - GetNumRows() + 1;
            }
            return true;
        }
        break;

    case HGEK_UP:
        if (nSelectedItem > 0) {
            nSelectedItem--;
            if (nSelectedItem < nTopItem) {
                nTopItem = nSelectedItem;
            }
            return true;
        }
        break;
    }
    return false;
}
