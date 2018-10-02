/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeGUI helper classes header
*/


#pragma once


#include "hge.h"
#include "hgesprite.h"
#include "hgerect.h"


#define HGEGUI_NONAVKEYS		0
#define HGEGUI_LEFTRIGHT		1
#define HGEGUI_UPDOWN			2
#define HGEGUI_CYCLED			4

class hgeGUI;

/*
** hgeGUIObject
*/
class hgeGUIObject {
public:
    hgeGUIObject() {
        hge_ = hgeCreate(HGE_VERSION);
        color = 0xFFFFFFFF;
    }

    virtual ~hgeGUIObject() {
        hge_->Release();
    }

    virtual void Render() = 0;

    virtual void Update(float dt) {
    }

    virtual void Enter() {
    }

    virtual void Leave() {
    }

    virtual void Reset() {
    }

    virtual bool IsDone() {
        return true;
    }

    virtual void Focus(bool bFocused) {
    }

    virtual void MouseOver(bool bOver) {
    }

    virtual bool MouseMove(float x, float y) {
        return false;
    }

    virtual bool MouseLButton(bool bDown) {
        return false;
    }

    virtual bool MouseRButton(bool bDown) {
        return false;
    }

    virtual bool MouseWheel(int nNotches) {
        return false;
    }

    virtual bool KeyClick(int key, int chr) {
        return false;
    }

    virtual void SetColor(const uint32_t _color) {
        color = _color;
    }

    int id;
    bool bStatic;
    bool bVisible;
    bool bEnabled;
    hgeRect rect;
    uint32_t color;

    hgeGUI* gui;
    hgeGUIObject* next;
    hgeGUIObject* prev;

protected:
    hgeGUIObject(const hgeGUIObject& go);
    hgeGUIObject& operator=(const hgeGUIObject& go);

    static HGE* hge_;
};


/*
** hgeGUI
*/
class hgeGUI {
public:
    hgeGUI();
    ~hgeGUI();

    void AddCtrl(hgeGUIObject* ctrl);
    void DelCtrl(int id);
    hgeGUIObject* GetCtrl(int id) const;

    void MoveCtrl(int id, float x, float y);
    void ShowCtrl(int id, bool bVisible);
    void EnableCtrl(int id, bool bEnabled);

    void SetNavMode(int mode);
    void SetCursor(hgeSprite* spr);
    void SetColor(const uint32_t color);
    void SetFocus(int id);
    int GetFocus() const;

    void Enter();
    void Leave();
    void Reset();
    void Move(float dx, float dy);

    int Update(float dt);
    void Render();

private:
    hgeGUI(const hgeGUI&);
    hgeGUI& operator=(const hgeGUI&);
    bool ProcessCtrl(hgeGUIObject* ctrl);

    static HGE* hge_;

    hgeGUIObject* ctrls_;
    hgeGUIObject* ctrl_lock_;
    hgeGUIObject* ctrl_focus_;
    hgeGUIObject* ctrl_over_;

    int navmode_;
    int enter_leave_;
    hgeSprite* spr_cursor_;

    float mx_;
    float my_;
    int wheel_;
    bool l_pressed_;
    bool l_released_;
    bool r_pressed_;
    bool r_released_;
};
