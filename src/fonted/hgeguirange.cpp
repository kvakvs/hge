/*
** Haaf's Game Engine 1.6
** Copyright (C) 2006, Relish Games
** hge.relishgames.com
**
** Bitmap Font Builder
*/


#include "hgeguirange.h"


hgeGUIRange::hgeGUIRange(const int _id, const float x, const float y, 
                         const float w, const float h, const int _nrows,
                         const int _ncols, const hgeU32 color) {
    id = _id;
    bStatic = false;
    bVisible = true;
    bEnabled = true;
    rect.Set(x, y, x + w, y + h);

    spr = new hgeSprite(0, 0, 0, 1, 1);
    spr->SetColor(color);
    nrows = _nrows;
    ncols = _ncols;
    nfirst = nlast = 0;

    mx = 0;
    my = 0;
    bPressed = false;
}

hgeGUIRange::~hgeGUIRange() {
    if (spr) {
        delete spr;
    }
}

void hgeGUIRange::SetRange(int first, int last) {
    if (first < 0) {
        first = 0;
    }
    if (last >= nrows * ncols) {
        last = nrows * ncols - 1;
    }
    nfirst = first;
    nlast = last;
}

void hgeGUIRange::GetRange(int* first, int* last) const {
    if (nlast >= nfirst) {
        *first = nfirst;
        *last = nlast;
    }
    else {
        *first = nlast;
        *last = nfirst;
    }
}

void hgeGUIRange::Render() {
    const auto cw = (rect.x2 - rect.x1) / ncols;
    const auto ch = (rect.y2 - rect.y1) / nrows;
    int first, last;

    if (nlast >= nfirst) {
        first = nfirst;
        last = nlast;
    }
    else {
        first = nlast;
        last = nfirst;
    }

    const auto xf = rect.x1 + cw * static_cast<float>(first % ncols);
    const auto yf = rect.y1 + ch * static_cast<float>(first / ncols);

    const auto xl = rect.x1 + cw * static_cast<float>(last % ncols);
    const auto yl = rect.y1 + ch * static_cast<float>(last / ncols);

    if (yf == yl) {
        spr->RenderStretch(xf, yf, xl + cw, yf + ch);
    }
    else {
        spr->RenderStretch(xf, yf, rect.x2, yf + ch);
        spr->RenderStretch(rect.x1, yl, xl + cw, yl + ch);
        if (yl - yf > 1) {
            spr->RenderStretch(rect.x1, yf + ch, rect.x2, yl);
        }
    }

    //spr->RenderStretch(x1,y1,x2,y2);
}

bool hgeGUIRange::MouseMove(const float x, const float y) {
    mx = x;
    my = y;

    if (bPressed) {
        nlast = calc_point(mx, my);
        return true;
    }

    return false;
}

bool hgeGUIRange::MouseLButton(const bool b_down) {
    bPressed = b_down;

    if (bPressed) {
        nfirst = calc_point(mx, my);
        nlast = nfirst;
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

    cw /= ncols;
    ch /= nrows;

    const int xcell = int(x) / int(cw);
    const int ycell = int(y) / int(ch);

    return ycell * ncols + xcell;
}
