/*
** Haaf's Game Engine 1.6
** Copyright (C) 2006, Relish Games
** hge.relishgames.com
**
** Bitmap Font Builder
*/


#pragma once


#include "../../include/hgegui.h"
#include "../../include/hgesprite.h"


/*
** hgeGUIRange
*/
class hgeGUIRange : public hgeGUIObject {
public:
    hgeGUIRange(int id, float x, float y, float w, float h, int nrows, int ncols, hgeU32 color);
    ~hgeGUIRange();

    void SetRange(int first, int last);
    void GetRange(int* first, int* last) const;

    void Render() override;
    bool MouseMove(float x, float y) override;
    bool MouseLButton(bool bDown) override;

private:
    int calc_point(float x, float y) const;

    hgeSprite* spr_;
    int rows_;
    int cols_;
    int nfirst_;
    int nlast_;
    float mx_;
    float my_;
    bool pressed_;
};
