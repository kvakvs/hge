/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once

#include <string>
#include <hge.h>
#include "hgesprite.h"
#include "hgefont.h"
#include "hgerect.h"
#include "hgegui.h"

/*
** hgeGUIText
*/
class hgeGUIText : public hgeGUIObject {
public:
    hgeGUIText(int id, float x, float y, float w, float h,
               hgeMUTABLE hgeFont *fnt);

    void SetMode(int _align);

    void SetText(const char *t);

    void printf(const char *format, ...);

    void Render() override;

private:
    hgeFont *font_;
    float tx_;
    float ty_;
    int align_;
    std::string text_;
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
enum {
    HGESLIDER_BAR = 0,
    HGESLIDER_BARRELATIVE = 1,
    HGESLIDER_SLIDER = 2,
};

class hgeGUISlider : public hgeGUIObject {
public:
    hgeGUISlider(int id, float x, float y, float w, float h, HTEXTURE tex, float tx, float ty,
                 float sw, float sh, bool vertical = false);

    virtual ~hgeGUISlider();

    void SetMode(const float _fMin, const float _fMax, const int _mode) {
      min_ = _fMin;
      max_ = _fMax;
      mode_ = _mode;
    }

    void SetValue(float _fVal);

    float GetValue() const {
      return val_;
    }

    void Render() override;

    bool MouseMove(float x, float y) override;

    bool MouseLButton(bool b_down) override;

private:
    bool pressed_;
    bool vertical_;
    int mode_;
    float min_;
    float max_;
    float val_;
    float sl_w_;
    float sl_h_;
    hgeSprite *spr_slider_;
};


/*
** hgeGUIListbox
*/
struct hgeGUIListboxItem {
    std::string text;
    hgeGUIListboxItem *next;
};

class hgeGUIListbox : public hgeGUIObject {
public:
    hgeGUIListbox(int id, float x, float y, float w, float h, hgeFont *fnt,
                  hgeColor32 tColor, hgeColor32 thColor, hgeColor32 hColor);

    virtual ~hgeGUIListbox();

    int AddItem(const char *item);

    void DeleteItem(int n);

    int GetSelectedItem() {
      return selected_item_;
    }

    void SetSelectedItem(const int n) {
      if (n >= 0 && n < GetNumItems()) { selected_item_ = n; }
    }

    int GetTopItem() {
      return top_item_;
    }

    void SetTopItem(const int n) {
      if (n >= 0 && n <= GetNumItems() - GetNumRows()) { top_item_ = n; }
    }

    const char *GetItemText(int n);

    int GetNumItems() {
      return items_;
    }

    int GetNumRows() {
      return int((rect.y2 - rect.y1) / font_->GetHeight());
    }

    void Clear();

    void Render() override;

    bool MouseMove(const float x, const float y) override {
      mx_ = x;
      my_ = y;
      return false;
    }

    bool MouseLButton(bool bDown) override;

    bool MouseWheel(int nNotches) override;

    bool KeyClick(int key, int chr) override;

private:
    hgeSprite *spr_highlight_;
    hgeFont *font_;
    hgeColor32 text_color_;
    hgeColor32 text_highlight_color_;

    int items_;
    int selected_item_;
    int top_item_;
    float mx_;
    float my_;
    hgeGUIListboxItem *items_2_;
};


#define hgeButtonGetState(gui, id)                ((hgeGUIButton*)gui->GetCtrl(id))->GetState()
#define hgeButtonSetState(gui, id, b)                ((hgeGUIButton*)gui->GetCtrl(id))->SetState(b)
#define hgeSliderGetValue(gui, id)                ((hgeGUISlider*)gui->GetCtrl(id))->GetValue()
#define hgeSliderSetValue(gui, id, f)                ((hgeGUISlider*)gui->GetCtrl(id))->SetValue(f)
#define hgeGetTextCtrl(gui, id)                        ((hgeGUIText*)gui->GetCtrl(id))

static hgeGUIListbox *hgeGetListboxCtrl(hgeGUI *gui, int id) {
  return (hgeGUIListbox *) gui->GetCtrl(id);
}
