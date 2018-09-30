/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: input
*/


#include "hge_impl.h"


static char* g_key_names[] = {
    "?",
    "Left Mouse Button", "Right Mouse Button", "?", "Middle Mouse Button",
    "?", "?", "?", "Backspace", "Tab", "?", "?", "?", "Enter", "?", "?",
    "Shift", "Ctrl", "Alt", "Pause", "Caps Lock", "?", "?", "?", "?", "?", "?",
    "Escape", "?", "?", "?", "?",
    "Space", "Page Up", "Page Down", "End", "Home",
    "Left Arrow", "Up Arrow", "Right Arrow", "Down Arrow",
    "?", "?", "?", "?", "Insert", "Delete", "?",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "?", "?", "?", "?", "?", "?", "?",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    "Left Win", "Right Win", "Application", "?", "?",
    "NumPad 0", "NumPad 1", "NumPad 2", "NumPad 3", "NumPad 4",
    "NumPad 5", "NumPad 6", "NumPad 7", "NumPad 8", "NumPad 9",
    "Multiply", "Add", "?", "Subtract", "Decimal", "Divide",
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "Num Lock", "Scroll Lock",
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "Semicolon", "Equals", "Comma", "Minus", "Period", "Slash", "Grave",
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?",
    "Left bracket", "Backslash", "Right bracket", "Apostrophe",
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?"
};


bool HGE_CALL HGE_Impl::Input_GetEvent(hgeInputEvent* event) {

    if (queue) {
        auto eptr = queue;
        memcpy(event, &eptr->event, sizeof(hgeInputEvent));
        queue = eptr->next;
        delete eptr;
        return true;
    }

    return false;
}

void HGE_CALL HGE_Impl::Input_GetMousePos(float* x, float* y) {
    *x = Xpos;
    *y = Ypos;
}


void HGE_CALL HGE_Impl::Input_SetMousePos(const float x, const float y) {
    POINT pt;
    pt.x = static_cast<long>(x);
    pt.y = static_cast<long>(y);
    ClientToScreen(hwnd, &pt);
    SetCursorPos(pt.x, pt.y);
}

int HGE_CALL HGE_Impl::Input_GetMouseWheel() {
    return Zpos;
}

bool HGE_CALL HGE_Impl::Input_IsMouseOver() {
    return bMouseOver;
}

bool HGE_CALL HGE_Impl::Input_GetKeyState(const int key) {
    return ((GetKeyState(key) & 0x8000) != 0);
}

bool HGE_CALL HGE_Impl::Input_KeyDown(const int key) {
    return (keyz[key] & 1) != 0;
}

bool HGE_CALL HGE_Impl::Input_KeyUp(const int key) {
    return (keyz[key] & 2) != 0;
}

char* HGE_CALL HGE_Impl::Input_GetKeyName(const int key) {
    return g_key_names[key];
}

int HGE_CALL HGE_Impl::Input_GetKey() {
    return VKey;
}

int HGE_CALL HGE_Impl::Input_GetChar() {
    return Char;
}


//////// Implementation ////////


void HGE_Impl::_InputInit() {
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hwnd, &pt);
    Xpos = static_cast<float>(pt.x);
    Ypos = static_cast<float>(pt.y);

    memset(&keyz, 0, sizeof(keyz));
}

void HGE_Impl::_UpdateMouse() {
    POINT pt;
    RECT rc;

    GetCursorPos(&pt);
    GetClientRect(hwnd, &rc);
    MapWindowPoints(hwnd, nullptr, reinterpret_cast<LPPOINT>(&rc), 2);

    if (bCaptured || (PtInRect(&rc, pt) && WindowFromPoint(pt) == hwnd)) {
        bMouseOver = true;
    }
    else {
        bMouseOver = false;
    }
}

void HGE_Impl::_BuildEvent(const int type, const int key, const int scan,
                           int flags, const int x, const int y) {
    auto eptr = new CInputEventList;
    unsigned char kbstate[256];
    POINT pt;

    eptr->event.type = type;
    eptr->event.chr = 0;
    pt.x = x;
    pt.y = y;

    GetKeyboardState(kbstate);
    if (type == INPUT_KEYDOWN) {
        if ((flags & HGEINP_REPEAT) == 0) {
            keyz[key] |= 1;
        }
        ToAscii(key, scan, kbstate,
                reinterpret_cast<unsigned short *>(&eptr->event.chr), 0);
    }
    if (type == INPUT_KEYUP) {
        keyz[key] |= 2;
        ToAscii(key, scan, kbstate,
                reinterpret_cast<unsigned short *>(&eptr->event.chr), 0);
    }
    if (type == INPUT_MOUSEWHEEL) {
        eptr->event.key = 0;
        eptr->event.wheel = key;
        ScreenToClient(hwnd, &pt);
    }
    else {
        eptr->event.key = key;
        eptr->event.wheel = 0;
    }

    if (type == INPUT_MBUTTONDOWN) {
        keyz[key] |= 1;
        SetCapture(hwnd);
        bCaptured = true;
    }
    if (type == INPUT_MBUTTONUP) {
        keyz[key] |= 2;
        ReleaseCapture();
        Input_SetMousePos(Xpos, Ypos);
        pt.x = static_cast<int>(Xpos);
        pt.y = static_cast<int>(Ypos);
        bCaptured = false;
    }

    if (kbstate[VK_SHIFT] & 0x80) {
        flags |= HGEINP_SHIFT;
    }
    if (kbstate[VK_CONTROL] & 0x80) {
        flags |= HGEINP_CTRL;
    }
    if (kbstate[VK_MENU] & 0x80) {
        flags |= HGEINP_ALT;
    }
    if (kbstate[VK_CAPITAL] & 0x1) {
        flags |= HGEINP_CAPSLOCK;
    }
    if (kbstate[VK_SCROLL] & 0x1) {
        flags |= HGEINP_SCROLLLOCK;
    }
    if (kbstate[VK_NUMLOCK] & 0x1) {
        flags |= HGEINP_NUMLOCK;
    }
    eptr->event.flags = flags;

    if (pt.x == -1) {
        eptr->event.x = Xpos;
        eptr->event.y = Ypos;
    }
    else {
        if (pt.x < 0) {
            pt.x = 0;
        }
        if (pt.y < 0) {
            pt.y = 0;
        }
        if (pt.x >= nScreenWidth) {
            pt.x = nScreenWidth - 1;
        }
        if (pt.y >= nScreenHeight) {
            pt.y = nScreenHeight - 1;
        }

        eptr->event.x = static_cast<float>(pt.x);
        eptr->event.y = static_cast<float>(pt.y);
    }

    eptr->next = nullptr;

    if (!queue) {
        queue = eptr;
    }
    else {
        auto last = queue;
        while (last->next) {
            last = last->next;
        }
        last->next = eptr;
    }

    if (eptr->event.type == INPUT_KEYDOWN || eptr->event.type == INPUT_MBUTTONDOWN) {
        VKey = eptr->event.key;
        Char = eptr->event.chr;
    }
    else if (eptr->event.type == INPUT_MOUSEMOVE) {
        Xpos = eptr->event.x;
        Ypos = eptr->event.y;
    }
    else if (eptr->event.type == INPUT_MOUSEWHEEL) {
        Zpos += eptr->event.wheel;
    }
}

void HGE_Impl::_ClearQueue() {
    auto eptr = queue;

    memset(&keyz, 0, sizeof(keyz));

    while (eptr) {
        const auto nexteptr = eptr->next;
        delete eptr;
        eptr = nexteptr;
    }

    queue = nullptr;
    VKey = 0;
    Char = 0;
    Zpos = 0;
}
