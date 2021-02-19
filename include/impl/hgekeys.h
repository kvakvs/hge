/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once


//
// HGE Input Event structure
//
struct hgeInputEvent {
    hgeInputEvent(int type, int chr) : type(type), chr(chr) {}

    int type; // event type
    int key{}; // key code
    int flags{}; // event flags
    int chr; // character code
    int wheel{}; // wheel shift
    float x{}; // mouse cursor x-coordinate
    float y{}; // mouse cursor y-coordinate
};


//
// HGE Input Event type constants
//
enum {
    INPUT_KEYDOWN = 1,
    INPUT_KEYUP = 2,
    INPUT_MBUTTONDOWN = 3,
    INPUT_MBUTTONUP = 4,
    INPUT_MOUSEMOVE = 5,
    INPUT_MOUSEWHEEL = 6,
};


//
// HGE Input Event flags
//
enum {
    HGEINP_SHIFT = 1,
    HGEINP_CTRL = 2,
    HGEINP_ALT = 4,
    HGEINP_CAPSLOCK = 8,
    HGEINP_SCROLLLOCK = 16,
    HGEINP_NUMLOCK = 32,
    HGEINP_REPEAT = 64,
};

//
// HGE Virtual-key codes
//
typedef enum {
    HGEK_NO_KEY = 0x00,
    HGEK_LBUTTON = 0x01,
    HGEK_RBUTTON = 0x02,
    HGEK_MBUTTON = 0x04,
    HGEK_ESCAPE = 0x1B,
    HGEK_BACKSPACE = 0x08,
    HGEK_TAB = 0x09,
    HGEK_ENTER = 0x0D,
    HGEK_SPACE = 0x20,
    HGEK_SHIFT = 0x10,
    HGEK_CTRL = 0x11,
    HGEK_ALT = 0x12,
    HGEK_LWIN = 0x5B,
    HGEK_RWIN = 0x5C,
    HGEK_APPS = 0x5D,
    HGEK_PAUSE = 0x13,
    HGEK_CAPSLOCK = 0x14,
    HGEK_NUMLOCK = 0x90,
    HGEK_SCROLLLOCK = 0x91,
    HGEK_PGUP = 0x21,
    HGEK_PGDN = 0x22,
    HGEK_HOME = 0x24,
    HGEK_END = 0x23,
    HGEK_INSERT = 0x2D,
    HGEK_DELETE = 0x2E,
    HGEK_LEFT = 0x25,
    HGEK_UP = 0x26,
    HGEK_RIGHT = 0x27,
    HGEK_DOWN = 0x28,
    HGEK_0 = 0x30,
    HGEK_1 = 0x31,
    HGEK_2 = 0x32,
    HGEK_3 = 0x33,
    HGEK_4 = 0x34,
    HGEK_5 = 0x35,
    HGEK_6 = 0x36,
    HGEK_7 = 0x37,
    HGEK_8 = 0x38,
    HGEK_9 = 0x39,
    HGEK_A = 0x41,
    HGEK_B = 0x42,
    HGEK_C = 0x43,
    HGEK_D = 0x44,
    HGEK_E = 0x45,
    HGEK_F = 0x46,
    HGEK_G = 0x47,
    HGEK_H = 0x48,
    HGEK_I = 0x49,
    HGEK_J = 0x4A,
    HGEK_K = 0x4B,
    HGEK_L = 0x4C,
    HGEK_M = 0x4D,
    HGEK_N = 0x4E,
    HGEK_O = 0x4F,
    HGEK_P = 0x50,
    HGEK_Q = 0x51,
    HGEK_R = 0x52,
    HGEK_S = 0x53,
    HGEK_T = 0x54,
    HGEK_U = 0x55,
    HGEK_V = 0x56,
    HGEK_W = 0x57,
    HGEK_X = 0x58,
    HGEK_Y = 0x59,
    HGEK_Z = 0x5A,
    HGEK_GRAVE = 0xC0,
    HGEK_MINUS = 0xBD,
    HGEK_EQUALS = 0xBB,
    HGEK_BACKSLASH = 0xDC,
    HGEK_LBRACKET = 0xDB,
    HGEK_RBRACKET = 0xDD,
    HGEK_SEMICOLON = 0xBA,
    HGEK_APOSTROPHE = 0xDE,
    HGEK_COMMA = 0xBC,
    HGEK_PERIOD = 0xBE,
    HGEK_SLASH = 0xBF,
    HGEK_NUMPAD0 = 0x60,
    HGEK_NUMPAD1 = 0x61,
    HGEK_NUMPAD2 = 0x62,
    HGEK_NUMPAD3 = 0x63,
    HGEK_NUMPAD4 = 0x64,
    HGEK_NUMPAD5 = 0x65,
    HGEK_NUMPAD6 = 0x66,
    HGEK_NUMPAD7 = 0x67,
    HGEK_NUMPAD8 = 0x68,
    HGEK_NUMPAD9 = 0x69,
    HGEK_MULTIPLY = 0x6A,
    HGEK_DIVIDE = 0x6F,
    HGEK_ADD = 0x6B,
    HGEK_SUBTRACT = 0x6D,
    HGEK_DECIMAL = 0x6E,
    HGEK_F1 = 0x70,
    HGEK_F2 = 0x71,
    HGEK_F3 = 0x72,
    HGEK_F4 = 0x73,
    HGEK_F5 = 0x74,
    HGEK_F6 = 0x75,
    HGEK_F7 = 0x76,
    HGEK_F8 = 0x77,
    HGEK_F9 = 0x78,
    HGEK_F10 = 0x79,
    HGEK_F11 = 0x7A,
    HGEK_F12 = 0x7B
} hgeKeyCode_t;
