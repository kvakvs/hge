/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Tutorial 06 - Creating menus
*/

// In menuitem.cpp/h we define the
// behaviour of our custom GUI control

#include "menuitem.h"

// This is a GUI control constructor,
// we should initialize all the variables here
hgeGUIMenuItem::hgeGUIMenuItem(const int _id, hgeFont* _fnt, const HEFFECT _snd,
                               const float _x, const float _y,
                               const float _delay, char* _title)
    : soffset(0), doffset(0) {
    id = _id;
    fnt = _fnt;
    snd = _snd;
    delay = _delay;
    title = _title;

    color.SetHWColor(0xFFFFE060);
    shadow.SetHWColor(0x30000000);
    offset = 0.0f;
    timer = -1.0f;
    timer2 = -1.0f;

    bStatic = false;
    bVisible = true;
    bEnabled = true;

    const float w = fnt->GetStringWidth(title);
    rect.Set(_x - w / 2, _y, _x + w / 2, _y + fnt->GetHeight());
}

// This method is called when the control should be rendered
void hgeGUIMenuItem::Render() {
    fnt->SetColor(shadow.GetHWColor());
    fnt->Render(rect.x1 + offset + 3, rect.y1 + 3, HGETEXT_LEFT, title);
    fnt->SetColor(color.GetHWColor());
    fnt->Render(rect.x1 - offset, rect.y1 - offset, HGETEXT_LEFT, title);
}

// This method is called each frame,
// we should update the animation here
void hgeGUIMenuItem::Update(const float dt) {
    if (timer2 != -1.0f) {
        timer2 += dt;
        if (timer2 >= delay + 0.1f) {
            color = scolor2 + dcolor2;
            shadow = sshadow + dshadow;
            offset = 0.0f;
            timer2 = -1.0f;
        }
        else {
            if (timer2 < delay) {
                color = scolor2;
                shadow = sshadow;
            }
            else {
                color = scolor2 + dcolor2 * (timer2 - delay) * 10;
                shadow = sshadow + dshadow * (timer2 - delay) * 10;
            }
        }
    }
    else if (timer != -1.0f) {
        timer += dt;
        if (timer >= 0.2f) {
            color = scolor + dcolor;
            offset = soffset + doffset;
            timer = -1.0f;
        }
        else {
            color = scolor + dcolor * timer * 5;
            offset = soffset + doffset * timer * 5;
        }
    }
}

// This method is called when the GUI
// is about to appear on the screen
void hgeGUIMenuItem::Enter() {
    hgeColor tcolor2;

    scolor2.SetHWColor(0x00FFE060);
    tcolor2.SetHWColor(0xFFFFE060);
    dcolor2 = tcolor2 - scolor2;

    sshadow.SetHWColor(0x00000000);
    tcolor2.SetHWColor(0x30000000);
    dshadow = tcolor2 - sshadow;

    timer2 = 0.0f;
}

// This method is called when the GUI
// is about to disappear from the screen
void hgeGUIMenuItem::Leave() {
    hgeColor tcolor2;

    scolor2.SetHWColor(0xFFFFE060);
    tcolor2.SetHWColor(0x00FFE060);
    dcolor2 = tcolor2 - scolor2;

    sshadow.SetHWColor(0x30000000);
    tcolor2.SetHWColor(0x00000000);
    dshadow = tcolor2 - sshadow;

    timer2 = 0.0f;
}

// This method is called to test whether the control
// have finished it's Enter/Leave animation
bool hgeGUIMenuItem::IsDone() {
    if (timer2 == -1.0f) return true;
    return false;
}

// This method is called when the control
// receives or loses keyboard input focus
void hgeGUIMenuItem::Focus(const bool b_focused) {
    hgeColor tcolor;

    if (b_focused) {
        hge->Effect_Play(snd);
        scolor.SetHWColor(0xFFFFE060);
        tcolor.SetHWColor(0xFFFFFFFF);
        soffset = 0;
        doffset = 4;
    }
    else {
        scolor.SetHWColor(0xFFFFFFFF);
        tcolor.SetHWColor(0xFFFFE060);
        soffset = 4;
        doffset = -4;
    }

    dcolor = tcolor - scolor;
    timer = 0.0f;
}

// This method is called to notify the control
// that the mouse cursor has entered or left it's area
void hgeGUIMenuItem::MouseOver(const bool b_over) {
    if (b_over) gui->SetFocus(id);
}

// This method is called to notify the control
// that the left mouse button state has changed.
// If it returns true - the caller will receive
// the control's ID
bool hgeGUIMenuItem::MouseLButton(const bool b_down) {
    if (!b_down) {
        offset = 4;
        return true;
    }
    hge->Effect_Play(snd);
    offset = 0;
    return false;
}

// This method is called to notify the
// control that a key has been clicked.
// If it returns true - the caller will
// receive the control's ID
bool hgeGUIMenuItem::KeyClick(const int key, int chr) {
    if (key == HGEK_ENTER || key == HGEK_SPACE) {
        MouseLButton(true);
        return MouseLButton(false);
    }

    return false;
}
