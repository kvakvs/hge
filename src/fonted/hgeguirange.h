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
#pragma once


#include <hgegui.h>
#include <hgesprite.h>


/*
** hgeGUIRange
*/
class hgeGUIRange : public hgeGUIObject {
public:
    hgeGUIRange(int id, float x, float y, float w, float h, int nrows, int ncols, hgeColor32 color);

    ~hgeGUIRange();

    void SetRange(int first, int last);

    void GetRange(int *first, int *last) const;

    void Render() override;

    bool MouseMove(float x, float y) override;

    bool MouseLButton(bool bDown) override;

private:
    int calc_point(float x, float y) const;

    hgeSprite *spr_;
    int rows_;
    int cols_;
    int nfirst_;
    int nlast_;
    float mx_;
    float my_;
    bool pressed_;
};
