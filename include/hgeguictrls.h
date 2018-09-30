/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeGUI default controls header
*/


#pragma once


#include "hge.h"
#include "hgesprite.h"
#include "hgefont.h"
#include "hgerect.h"
#include "hgegui.h"


#define hgeButtonGetState(gui,id)		((hgeGUIButton*)gui->GetCtrl(id))->GetState()
#define hgeButtonSetState(gui,id,b)		((hgeGUIButton*)gui->GetCtrl(id))->SetState(b)
#define hgeSliderGetValue(gui,id)		((hgeGUISlider*)gui->GetCtrl(id))->GetValue()
#define hgeSliderSetValue(gui,id,f)		((hgeGUISlider*)gui->GetCtrl(id))->SetValue(f)
#define hgeGetTextCtrl(gui,id)			((hgeGUIText*)gui->GetCtrl(id))
#define hgeGetListboxCtrl(gui,id)		((hgeGUIListbox*)gui->GetCtrl(id))


/*
** hgeGUIText
*/
class hgeGUIText : public hgeGUIObject {
public:
    hgeGUIText(int id, float x, float y, float w, float h, hgeFont* fnt);

    void SetMode(int _align);
    void SetText(const char* _text);
    void printf(const char* format, ...);

    void Render() override;

private:
    hgeFont* font;
    float tx, ty;
    int align;
    char text[256];
};


/*
** hgeGUIButton
*/
class hgeGUIButton : public hgeGUIObject {
public:
    hgeGUIButton(int id, float x, float y, float w, float h, HTEXTURE tex, float tx, float ty);
    virtual ~hgeGUIButton();

    void SetMode(const bool _bTrigger) {
        bTrigger = _bTrigger;
    }

    void SetState(const bool _bPressed) {
        bPressed = _bPressed;
    }

    bool GetState() const {
        return bPressed;
    }

    void Render() override;
    bool MouseLButton(bool bDown) override;

private:
    bool bTrigger;
    bool bPressed;
    bool bOldState;
    hgeSprite *sprUp, *sprDown;
};


/*
** hgeGUISlider
*/
#define HGESLIDER_BAR			0
#define HGESLIDER_BARRELATIVE	1
#define HGESLIDER_SLIDER		2

class hgeGUISlider : public hgeGUIObject {
public:
    hgeGUISlider(int id, float x, float y, float w, float h, HTEXTURE tex, float tx, float ty,
                 float sw, float sh, bool vertical = false);
    virtual ~hgeGUISlider();

    void SetMode(const float _fMin, const float _fMax, const int _mode) {
        fMin = _fMin;
        fMax = _fMax;
        mode = _mode;
    }

    void SetValue(float _fVal);

    float GetValue() const {
        return fVal;
    }

    void Render() override;
    bool MouseMove(float x, float y) override;
    bool MouseLButton(bool b_down) override;

private:
    bool bPressed;
    bool bVertical;
    int mode;
    float fMin, fMax, fVal;
    float sl_w, sl_h;
    hgeSprite* sprSlider;
};


/*
** hgeGUIListbox
*/
struct hgeGUIListboxItem {
    char text[64];
    hgeGUIListboxItem* next;
};

class hgeGUIListbox : public hgeGUIObject {
public:
    hgeGUIListbox(int id, float x, float y, float w, float h, hgeFont* fnt, hgeU32 tColor,
                  hgeU32 thColor, hgeU32 hColor);
    virtual ~hgeGUIListbox();

    int AddItem(char* item);
    void DeleteItem(int n);

    int GetSelectedItem() {
        return nSelectedItem;
    }

    void SetSelectedItem(const int n) {
        if (n >= 0 && n < GetNumItems()) nSelectedItem = n;
    }

    int GetTopItem() {
        return nTopItem;
    }

    void SetTopItem(const int n) {
        if (n >= 0 && n <= GetNumItems() - GetNumRows()) nTopItem = n;
    }

    char* GetItemText(int n);

    int GetNumItems() {
        return nItems;
    }

    int GetNumRows() {
        return int((rect.y2 - rect.y1) / font->GetHeight());
    }

    void Clear();

    void Render() override;

    bool MouseMove(const float x, const float y) override {
        mx = x;
        my = y;
        return false;
    }

    bool MouseLButton(bool bDown) override;
    bool MouseWheel(int nNotches) override;
    bool KeyClick(int key, int chr) override;

private:
    hgeSprite* sprHighlight;
    hgeFont* font;
    hgeU32 textColor, texthilColor;

    int nItems, nSelectedItem, nTopItem;
    float mx, my;
    hgeGUIListboxItem* pItems;
};
