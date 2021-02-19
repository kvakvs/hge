/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Core functions implementation: input
//

#include "hge_impl.h"

namespace hgeImpl {

    static char const *g_key_names[] = {
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


    bool HGE_CALL HGE_Impl::Input_GetEvent(hgeInputEvent *event) {
      if (ev_queue_.empty()) { return false; }
      memcpy(event, &ev_queue_.front(), sizeof(hgeInputEvent));
      ev_queue_.pop();
      return true;
    }

    void HGE_CALL HGE_Impl::Input_GetMousePos(float *x, float *y) {
      *x = xpos_;
      *y = ypos_;
    }


    void HGE_CALL HGE_Impl::Input_SetMousePos(const float x, const float y) {
      POINT pt;
      pt.x = static_cast<long>(x);
      pt.y = static_cast<long>(y);
      ClientToScreen(hwnd_, &pt);
      SetCursorPos(pt.x, pt.y);
    }

    int HGE_CALL HGE_Impl::Input_GetMouseWheel() {
      return zpos_;
    }

    bool HGE_CALL HGE_Impl::Input_IsMouseOver() {
      return mouse_over_;
    }

    bool HGE_CALL HGE_Impl::Input_GetKeyState(const int key) {
      return ((GetKeyState(key) & 0x8000) != 0);
    }

    bool HGE_CALL HGE_Impl::Input_KeyDown(const int key) {
      return key_table_[key].pressed;
    }

    bool HGE_CALL HGE_Impl::Input_KeyUp(const int key) {
      return key_table_[key].released;
    }

    const char *HGE_CALL HGE_Impl::Input_GetKeyName(const int key) {
      return g_key_names[key];
    }

    int HGE_CALL HGE_Impl::Input_GetKey() {
      return v_key_;
    }

    int HGE_CALL HGE_Impl::Input_GetChar() {
      return input_char_;
    }


//////// Implementation ////////


    void HGE_Impl::input_init() {
      POINT pt;
      GetCursorPos(&pt);
      ScreenToClient(hwnd_, &pt);
      xpos_ = static_cast<float>(pt.x);
      ypos_ = static_cast<float>(pt.y);

      memset(&key_table_, 0, sizeof(key_table_));
    }

    void HGE_Impl::update_mouse() {
      POINT pt;
      RECT rc;

      GetCursorPos(&pt);
      GetClientRect(hwnd_, &rc);
      MapWindowPoints(hwnd_, nullptr, reinterpret_cast<LPPOINT>(&rc), 2);

      if (is_captured_ || (PtInRect(&rc, pt) && WindowFromPoint(pt) == hwnd_)) {
        mouse_over_ = true;
      } else {
        mouse_over_ = false;
      }
    }

    void HGE_Impl::build_event(const int type, const int key, const int scan,
                               int flags, const int x, const int y) {
      unsigned char kbstate[256];
      POINT pt;
      hgeInputEvent new_event(type, 0);
      pt.x = x;
      pt.y = y;

      GetKeyboardState(kbstate);
      if (type == INPUT_KEYDOWN) {
        if ((flags & HGEINP_REPEAT) == 0) {
          key_table_[key].pressed = true;
        }
        ToAscii(key, scan, kbstate,
                reinterpret_cast<uint16_t *>(&new_event.chr), 0);
      }
      if (type == INPUT_KEYUP) {
        key_table_[key].released = true;
        ToAscii(key, scan, kbstate,
                reinterpret_cast<uint16_t *>(&new_event.chr), 0);
      }
      if (type == INPUT_MOUSEWHEEL) {
        new_event.key = 0;
        new_event.wheel = key;
        ScreenToClient(hwnd_, &pt);
      } else {
        new_event.key = key;
        new_event.wheel = 0;
      }

      if (type == INPUT_MBUTTONDOWN) {
        key_table_[key].pressed = true;
        SetCapture(hwnd_);
        is_captured_ = true;
      }
      if (type == INPUT_MBUTTONUP) {
        key_table_[key].released = true;
        ReleaseCapture();
        Input_SetMousePos(xpos_, ypos_);
        pt.x = static_cast<int>(xpos_);
        pt.y = static_cast<int>(ypos_);
        is_captured_ = false;
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
      new_event.flags = flags;

      if (pt.x == -1) {
        new_event.x = xpos_;
        new_event.y = ypos_;
      } else {
        if (pt.x < 0) {
          pt.x = 0;
        }
        if (pt.y < 0) {
          pt.y = 0;
        }
        if (pt.x >= screen_width_) {
          pt.x = screen_width_ - 1;
        }
        if (pt.y >= screen_height_) {
          pt.y = screen_height_ - 1;
        }

        new_event.x = static_cast<float>(pt.x);
        new_event.y = static_cast<float>(pt.y);
      }

      if (new_event.type == INPUT_KEYDOWN
          || new_event.type == INPUT_MBUTTONDOWN) {
        v_key_ = new_event.key;
        input_char_ = new_event.chr;
      } else if (new_event.type == INPUT_MOUSEMOVE) {
        xpos_ = new_event.x;
        ypos_ = new_event.y;
      } else if (new_event.type == INPUT_MOUSEWHEEL) {
        zpos_ += new_event.wheel;
      }

      ev_queue_.push(new_event); // TODO: Can use C++11 emplace
    }

    void HGE_Impl::clear_queue() {
      while (!ev_queue_.empty()) { ev_queue_.pop(); }

      memset(&key_table_, 0, sizeof(key_table_));

      v_key_ = 0;
      input_char_ = 0;
      zpos_ = 0;
    }

} // namespace
