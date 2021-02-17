/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Tutorial 06 - Creating menus
//

// In menuitem.cpp/h we define the
// behaviour of our custom GUI control

#pragma once

#include <string>
#include "../../include/hge.h"
#include "../../include/hgegui.h"
#include "../../include/hgefont.h"
#include "../../include/hgecolor.h"


class hgeGUIMenuItem : public hgeGUIObject {
public:
    hgeGUIMenuItem(int id, hgeFont* fnt, HEFFECT snd, float x, float y, float delay, const char* title);

    void Render() override;
    void Update(float dt) override;

    void Enter() override;
    void Leave() override;
    bool IsDone() override;
    void Focus(bool bFocused) override;
    void MouseOver(bool bOver) override;

    bool MouseLButton(bool bDown) override;
    bool KeyClick(int key, int chr) override;

private:
    hgeFont* fnt;
    HEFFECT snd;
    float delay;
    std::string title;

    hgeColor scolor, dcolor, scolor2, dcolor2, sshadow, dshadow;
    hgeColor color, shadow;
    float soffset, doffset, offset;
    float timer, timer2;
};
