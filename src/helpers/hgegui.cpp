/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeGUI helper class implementation
*/


#include "..\..\include\hgegui.h"


HGE* hgeGUI::hge = nullptr;
HGE* hgeGUIObject::hge = nullptr;


hgeGUI::hgeGUI() {
    hge = hgeCreate(HGE_VERSION);

    ctrls = nullptr;
    ctrlLock = nullptr;
    ctrlFocus = nullptr;
    ctrlOver = nullptr;
    navmode = HGEGUI_NONAVKEYS;
    bLPressed = bLReleased = false;
    bRPressed = bRReleased = false;
    nWheel = 0;
    mx = my = 0.0f;
    nEnterLeave = 0;
    sprCursor = nullptr;
}

hgeGUI::~hgeGUI() {
    hgeGUIObject* ctrl = ctrls;

    while (ctrl) {
        hgeGUIObject* nextctrl = ctrl->next;
        delete ctrl;
        ctrl = nextctrl;
    }

    hge->Release();
}

void hgeGUI::AddCtrl(hgeGUIObject* ctrl) {
    hgeGUIObject* last = ctrls;

    ctrl->gui = this;

    if (!ctrls) {
        ctrls = ctrl;
        ctrl->prev = nullptr;
        ctrl->next = nullptr;
    }
    else {
        while (last->next) {
            last = last->next;
        }
        last->next = ctrl;
        ctrl->prev = last;
        ctrl->next = nullptr;
    }
}

void hgeGUI::DelCtrl(const int id) {
    hgeGUIObject* ctrl = ctrls;

    while (ctrl) {
        if (ctrl->id == id) {
            if (ctrl->prev) {
                ctrl->prev->next = ctrl->next;
            }
            else {
                ctrls = ctrl->next;
            }
            if (ctrl->next) {
                ctrl->next->prev = ctrl->prev;
            }
            delete ctrl;
            return;
        }
        ctrl = ctrl->next;
    }
}

hgeGUIObject* hgeGUI::GetCtrl(const int id) const {
    hgeGUIObject* ctrl = ctrls;

    while (ctrl) {
        if (ctrl->id == id) {
            return ctrl;
        }
        ctrl = ctrl->next;
    }

    return nullptr;
}

void hgeGUI::MoveCtrl(const int id, const float x, const float y) {
    hgeGUIObject* ctrl = GetCtrl(id);
    ctrl->rect.x2 = x + (ctrl->rect.x2 - ctrl->rect.x1);
    ctrl->rect.y2 = y + (ctrl->rect.y2 - ctrl->rect.y1);
    ctrl->rect.x1 = x;
    ctrl->rect.y1 = y;
}

void hgeGUI::ShowCtrl(const int id, const bool b_visible) {
    GetCtrl(id)->bVisible = b_visible;
}

void hgeGUI::EnableCtrl(const int id, const bool b_enabled) {
    GetCtrl(id)->bEnabled = b_enabled;
}

void hgeGUI::SetNavMode(const int mode) {
    navmode = mode;
}

void hgeGUI::SetCursor(hgeSprite* spr) {
    sprCursor = spr;
}


void hgeGUI::SetColor(const hgeU32 color) {
    auto ctrl = ctrls;

    while (ctrl) {
        ctrl->SetColor(color);
        ctrl = ctrl->next;
    }
}


void hgeGUI::Reset() {
    auto ctrl = ctrls;

    while (ctrl) {
        ctrl->Reset();
        ctrl = ctrl->next;
    }

    ctrlLock = nullptr;
    ctrlOver = nullptr;
    ctrlFocus = nullptr;
}


void hgeGUI::Move(const float dx, const float dy) {
    auto ctrl = ctrls;

    while (ctrl) {
        ctrl->rect.x1 += dx;
        ctrl->rect.y1 += dy;
        ctrl->rect.x2 += dx;
        ctrl->rect.y2 += dy;

        ctrl = ctrl->next;
    }
}


void hgeGUI::SetFocus(const int id) {
    auto ctrlNewFocus = GetCtrl(id);

    if (ctrlNewFocus == ctrlFocus) {
        return;
    }
    if (!ctrlNewFocus) {
        if (ctrlFocus) {
            ctrlFocus->Focus(false);
        }
        ctrlFocus = nullptr;
    }
    else if (!ctrlNewFocus->bStatic && ctrlNewFocus->bVisible && ctrlNewFocus->bEnabled) {
        if (ctrlFocus) {
            ctrlFocus->Focus(false);
        }
        if (ctrlNewFocus) {
            ctrlNewFocus->Focus(true);
        }
        ctrlFocus = ctrlNewFocus;
    }
}

int hgeGUI::GetFocus() const {
    if (ctrlFocus) {
        return ctrlFocus->id;
    }
    return 0;
}

void hgeGUI::Enter() {
    hgeGUIObject* ctrl = ctrls;

    while (ctrl) {
        ctrl->Enter();
        ctrl = ctrl->next;
    }

    nEnterLeave = 2;
}

void hgeGUI::Leave() {
    hgeGUIObject* ctrl = ctrls;

    while (ctrl) {
        ctrl->Leave();
        ctrl = ctrl->next;
    }

    ctrlFocus = nullptr;
    ctrlOver = nullptr;
    ctrlLock = nullptr;
    nEnterLeave = 1;
}

void hgeGUI::Render() {
    hgeGUIObject* ctrl = ctrls;

    while (ctrl) {
        if (ctrl->bVisible) {
            ctrl->Render();
        }
        ctrl = ctrl->next;
    }

    if (hge->Input_IsMouseOver() && sprCursor) {
        sprCursor->Render(mx, my);
    }
}

int hgeGUI::Update(const float dt) {

    // Update the mouse variables

    hge->Input_GetMousePos(&mx, &my);
    bLPressed = hge->Input_KeyDown(HGEK_LBUTTON);
    bLReleased = hge->Input_KeyUp(HGEK_LBUTTON);
    bRPressed = hge->Input_KeyDown(HGEK_RBUTTON);
    bRReleased = hge->Input_KeyUp(HGEK_RBUTTON);
    nWheel = hge->Input_GetMouseWheel();

    // Update all controls

    hgeGUIObject* ctrl = ctrls;
    while (ctrl) {
        ctrl->Update(dt);
        ctrl = ctrl->next;
    }

    // Handle Enter/Leave

    if (nEnterLeave) {
        ctrl = ctrls;
        bool bDone = true;
        while (ctrl) {
            if (!ctrl->IsDone()) {
                bDone = false;
                break;
            }
            ctrl = ctrl->next;
        }
        if (!bDone) {
            return 0;
        }
        if (nEnterLeave == 1) {
            return -1;
        }
        nEnterLeave = 0;
    }

    // Handle keys

    const int key = hge->Input_GetKey();
    if (((navmode & HGEGUI_LEFTRIGHT) && key == HGEK_LEFT) ||
        ((navmode & HGEGUI_UPDOWN) && key == HGEK_UP)) {
        ctrl = ctrlFocus;
        if (!ctrl) {
            ctrl = ctrls;
            if (!ctrl) {
                return 0;
            }
        }
        do {
            ctrl = ctrl->prev;
            if (!ctrl && ((navmode & HGEGUI_CYCLED) || !ctrlFocus)) {
                ctrl = ctrls;
                while (ctrl->next) {
                    ctrl = ctrl->next;
                }
            }
            if (!ctrl || ctrl == ctrlFocus) {
                break;
            }
        }
        while (ctrl->bStatic == true || ctrl->bVisible == false || ctrl->bEnabled == false);

        if (ctrl && ctrl != ctrlFocus) {
            if (ctrlFocus) {
                ctrlFocus->Focus(false);
            }
            if (ctrl) {
                ctrl->Focus(true);
            }
            ctrlFocus = ctrl;
        }
    }
    else if (((navmode & HGEGUI_LEFTRIGHT) && key == HGEK_RIGHT) ||
        ((navmode & HGEGUI_UPDOWN) && key == HGEK_DOWN)) {
        ctrl = ctrlFocus;
        if (!ctrl) {
            ctrl = ctrls;
            if (!ctrl) {
                return 0;
            }
            while (ctrl->next) {
                ctrl = ctrl->next;
            }
        }
        do {
            ctrl = ctrl->next;
            if (!ctrl && ((navmode & HGEGUI_CYCLED) || !ctrlFocus)) {
                ctrl = ctrls;
            }
            if (!ctrl || ctrl == ctrlFocus) {
                break;
            }
        }
        while (ctrl->bStatic == true || ctrl->bVisible == false || ctrl->bEnabled == false);

        if (ctrl && ctrl != ctrlFocus) {
            if (ctrlFocus) {
                ctrlFocus->Focus(false);
            }
            if (ctrl) {
                ctrl->Focus(true);
            }
            ctrlFocus = ctrl;
        }
    }
    else if (ctrlFocus && key && key != HGEK_LBUTTON && key != HGEK_RBUTTON) {
        if (ctrlFocus->KeyClick(key, hge->Input_GetChar())) {
            return ctrlFocus->id;
        }
    }

    // Handle mouse

    const auto b_l_down = hge->Input_GetKeyState(HGEK_LBUTTON);
    const auto b_r_down = hge->Input_GetKeyState(HGEK_RBUTTON);

    if (ctrlLock) {
        ctrl = ctrlLock;
        if (!b_l_down && !b_r_down) {
            ctrlLock = nullptr;
        }
        if (ProcessCtrl(ctrl)) {
            return ctrl->id;
        }
    }
    else {
        // Find last (topmost) control

        ctrl = ctrls;
        if (ctrl)
            while (ctrl->next) {
                ctrl = ctrl->next;
            }

        while (ctrl) {
            if (ctrl->rect.TestPoint(mx, my) && ctrl->bEnabled) {
                if (ctrlOver != ctrl) {
                    if (ctrlOver) {
                        ctrlOver->MouseOver(false);
                    }
                    ctrl->MouseOver(true);
                    ctrlOver = ctrl;
                }

                if (ProcessCtrl(ctrl)) {
                    return ctrl->id;
                }
                return 0;
            }
            ctrl = ctrl->prev;
        }

        if (ctrlOver) {
            ctrlOver->MouseOver(false);
            ctrlOver = nullptr;
        }

    }

    return 0;
}

bool hgeGUI::ProcessCtrl(hgeGUIObject* ctrl) {
    bool bResult = false;

    if (bLPressed) {
        ctrlLock = ctrl;
        SetFocus(ctrl->id);
        bResult = bResult || ctrl->MouseLButton(true);
    }
    if (bRPressed) {
        ctrlLock = ctrl;
        SetFocus(ctrl->id);
        bResult = bResult || ctrl->MouseRButton(true);
    }
    if (bLReleased) {
        bResult = bResult || ctrl->MouseLButton(false);
    }
    if (bRReleased) {
        bResult = bResult || ctrl->MouseRButton(false);
    }
    if (nWheel) {
        bResult = bResult || ctrl->MouseWheel(nWheel);
    }
    bResult = bResult || ctrl->MouseMove(mx - ctrl->rect.x1, my - ctrl->rect.y1);

    return bResult;
}
