/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Tutorial 06 - Creating menus
*/

// In menuitem.cpp/h we define the
// behaviour of our custom GUI control

#pragma once

#include "../../include/hge.h"
#include "../../include/hgegui.h"
#include "../../include/hgefont.h"
#include "../../include/hgecolor.h"


class hgeGUIMenuItem : public hgeGUIObject {
public:
    hgeGUIMenuItem(int id, hgeFont* fnt, HEFFECT snd, float x, float y, float delay, char* title);

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
    char* title;

    hgeColor scolor, dcolor, scolor2, dcolor2, sshadow, dshadow;
    hgeColor color, shadow;
    float soffset, doffset, offset;
    float timer, timer2;
};
