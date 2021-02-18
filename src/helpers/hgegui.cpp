/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// hgeGUI helper class implementation
//
#include <hgegui.h>


HGE *hgeGUI::hge_ = nullptr;
HGE *hgeGUIObject::hge_ = nullptr;


hgeGUI::hgeGUI() {
  hge_ = hgeCreate(HGE_VERSION);

  ctrls_ = nullptr;
  ctrl_lock_ = nullptr;
  ctrl_focus_ = nullptr;
  ctrl_over_ = nullptr;
  navmode_ = HGEGUI_NONAVKEYS;
  l_pressed_ = l_released_ = false;
  r_pressed_ = r_released_ = false;
  wheel_ = 0;
  mx_ = my_ = 0.0f;
  enter_leave_ = 0;
  spr_cursor_ = nullptr;
}

hgeGUI::~hgeGUI() {
  hgeGUIObject *ctrl = ctrls_;

  while (ctrl) {
    hgeGUIObject *nextctrl = ctrl->next;
    delete ctrl;
    ctrl = nextctrl;
  }

  hge_->Release();
}

void hgeGUI::AddCtrl(hgeGUIObject *ctrl) {
  hgeGUIObject *last = ctrls_;

  ctrl->gui = this;

  if (!ctrls_) {
    ctrls_ = ctrl;
    ctrl->prev = nullptr;
    ctrl->next = nullptr;
  } else {
    while (last->next) {
      last = last->next;
    }
    last->next = ctrl;
    ctrl->prev = last;
    ctrl->next = nullptr;
  }
}

void hgeGUI::DelCtrl(const int id) {
  hgeGUIObject *ctrl = ctrls_;

  while (ctrl) {
    if (ctrl->id == id) {
      if (ctrl->prev) {
        ctrl->prev->next = ctrl->next;
      } else {
        ctrls_ = ctrl->next;
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

hgeGUIObject *hgeGUI::GetCtrl(const int id) const {
  hgeGUIObject *ctrl = ctrls_;

  while (ctrl) {
    if (ctrl->id == id) {
      return ctrl;
    }
    ctrl = ctrl->next;
  }

  return nullptr;
}

void hgeGUI::MoveCtrl(const int id, const float x, const float y) {
  hgeGUIObject *ctrl = GetCtrl(id);
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
  navmode_ = mode;
}

void hgeGUI::SetCursor(hgeSprite *spr) {
  spr_cursor_ = spr;
}


void hgeGUI::SetColor(const hgeColor32 color) {
  auto ctrl = ctrls_;

  while (ctrl) {
    ctrl->SetColor(color);
    ctrl = ctrl->next;
  }
}


void hgeGUI::Reset() {
  auto ctrl = ctrls_;

  while (ctrl) {
    ctrl->Reset();
    ctrl = ctrl->next;
  }

  ctrl_lock_ = nullptr;
  ctrl_over_ = nullptr;
  ctrl_focus_ = nullptr;
}


void hgeGUI::Move(const float dx, const float dy) {
  auto ctrl = ctrls_;

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

  if (ctrlNewFocus == ctrl_focus_) {
    return;
  }
  if (!ctrlNewFocus) {
    if (ctrl_focus_) {
      ctrl_focus_->Focus(false);
    }
    ctrl_focus_ = nullptr;
  } else if (!ctrlNewFocus->bStatic && ctrlNewFocus->bVisible && ctrlNewFocus->bEnabled) {
    if (ctrl_focus_) {
      ctrl_focus_->Focus(false);
    }
    if (ctrlNewFocus) {
      ctrlNewFocus->Focus(true);
    }
    ctrl_focus_ = ctrlNewFocus;
  }
}

int hgeGUI::GetFocus() const {
  if (ctrl_focus_) {
    return ctrl_focus_->id;
  }
  return 0;
}

void hgeGUI::Enter() {
  hgeGUIObject *ctrl = ctrls_;

  while (ctrl) {
    ctrl->Enter();
    ctrl = ctrl->next;
  }

  enter_leave_ = 2;
}

void hgeGUI::Leave() {
  hgeGUIObject *ctrl = ctrls_;

  while (ctrl) {
    ctrl->Leave();
    ctrl = ctrl->next;
  }

  ctrl_focus_ = nullptr;
  ctrl_over_ = nullptr;
  ctrl_lock_ = nullptr;
  enter_leave_ = 1;
}

void hgeGUI::Render() {
  hgeGUIObject *ctrl = ctrls_;

  while (ctrl) {
    if (ctrl->bVisible) {
      ctrl->Render();
    }
    ctrl = ctrl->next;
  }

  if (hge_->Input_IsMouseOver() && spr_cursor_) {
    spr_cursor_->Render(mx_, my_);
  }
}

int hgeGUI::Update(const float dt) {

  // Update the mouse variables

  hge_->Input_GetMousePos(&mx_, &my_);
  l_pressed_ = hge_->Input_KeyDown(HGEK_LBUTTON);
  l_released_ = hge_->Input_KeyUp(HGEK_LBUTTON);
  r_pressed_ = hge_->Input_KeyDown(HGEK_RBUTTON);
  r_released_ = hge_->Input_KeyUp(HGEK_RBUTTON);
  wheel_ = hge_->Input_GetMouseWheel();

  // Update all controls

  hgeGUIObject *ctrl = ctrls_;
  while (ctrl) {
    ctrl->Update(dt);
    ctrl = ctrl->next;
  }

  // Handle Enter/Leave

  if (enter_leave_) {
    ctrl = ctrls_;
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
    if (enter_leave_ == 1) {
      return -1;
    }
    enter_leave_ = 0;
  }

  // Handle keys

  const int key = hge_->Input_GetKey();
  if (((navmode_ & HGEGUI_LEFTRIGHT) && key == HGEK_LEFT) ||
      ((navmode_ & HGEGUI_UPDOWN) && key == HGEK_UP)) {
    ctrl = ctrl_focus_;
    if (!ctrl) {
      ctrl = ctrls_;
      if (!ctrl) {
        return 0;
      }
    }
    do {
      ctrl = ctrl->prev;
      if (!ctrl && ((navmode_ & HGEGUI_CYCLED) || !ctrl_focus_)) {
        ctrl = ctrls_;
        while (ctrl->next) {
          ctrl = ctrl->next;
        }
      }
      if (!ctrl || ctrl == ctrl_focus_) {
        break;
      }
    } while (ctrl->bStatic || !ctrl->bVisible || !ctrl->bEnabled);

    if (ctrl && ctrl != ctrl_focus_) {
      if (ctrl_focus_) {
        ctrl_focus_->Focus(false);
      }
      if (ctrl) {
        ctrl->Focus(true);
      }
      ctrl_focus_ = ctrl;
    }
  } else if (((navmode_ & HGEGUI_LEFTRIGHT) && key == HGEK_RIGHT) ||
             ((navmode_ & HGEGUI_UPDOWN) && key == HGEK_DOWN)) {
    ctrl = ctrl_focus_;
    if (!ctrl) {
      ctrl = ctrls_;
      if (!ctrl) {
        return 0;
      }
      while (ctrl->next) {
        ctrl = ctrl->next;
      }
    }
    do {
      ctrl = ctrl->next;
      if (!ctrl && ((navmode_ & HGEGUI_CYCLED) || !ctrl_focus_)) {
        ctrl = ctrls_;
      }
      if (!ctrl || ctrl == ctrl_focus_) {
        break;
      }
    } while (ctrl->bStatic || !ctrl->bVisible || !ctrl->bEnabled);

    if (ctrl && ctrl != ctrl_focus_) {
      if (ctrl_focus_) {
        ctrl_focus_->Focus(false);
      }
      if (ctrl) {
        ctrl->Focus(true);
      }
      ctrl_focus_ = ctrl;
    }
  } else if (ctrl_focus_ && key && key != HGEK_LBUTTON && key != HGEK_RBUTTON) {
    if (ctrl_focus_->KeyClick(key, hge_->Input_GetChar())) {
      return ctrl_focus_->id;
    }
  }

  // Handle mouse

  const auto b_l_down = hge_->Input_GetKeyState(HGEK_LBUTTON);
  const auto b_r_down = hge_->Input_GetKeyState(HGEK_RBUTTON);

  if (ctrl_lock_) {
    ctrl = ctrl_lock_;
    if (!b_l_down && !b_r_down) {
      ctrl_lock_ = nullptr;
    }
    if (ProcessCtrl(ctrl)) {
      return ctrl->id;
    }
  } else {
    // Find last (topmost) control

    ctrl = ctrls_;
    if (ctrl)
      while (ctrl->next) {
        ctrl = ctrl->next;
      }

    while (ctrl) {
      if (ctrl->rect.TestPoint(mx_, my_) && ctrl->bEnabled) {
        if (ctrl_over_ != ctrl) {
          if (ctrl_over_) {
            ctrl_over_->MouseOver(false);
          }
          ctrl->MouseOver(true);
          ctrl_over_ = ctrl;
        }

        if (ProcessCtrl(ctrl)) {
          return ctrl->id;
        }
        return 0;
      }
      ctrl = ctrl->prev;
    }

    if (ctrl_over_) {
      ctrl_over_->MouseOver(false);
      ctrl_over_ = nullptr;
    }

  }

  return 0;
}

bool hgeGUI::ProcessCtrl(hgeGUIObject *ctrl) {
  bool bResult = false;

  if (l_pressed_) {
    ctrl_lock_ = ctrl;
    SetFocus(ctrl->id);
    bResult = bResult || ctrl->MouseLButton(true);
  }
  if (r_pressed_) {
    ctrl_lock_ = ctrl;
    SetFocus(ctrl->id);
    bResult = bResult || ctrl->MouseRButton(true);
  }
  if (l_released_) {
    bResult = bResult || ctrl->MouseLButton(false);
  }
  if (r_released_) {
    bResult = bResult || ctrl->MouseRButton(false);
  }
  if (wheel_) {
    bResult = bResult || ctrl->MouseWheel(wheel_);
  }
  bResult = bResult || ctrl->MouseMove(mx_ - ctrl->rect.x1, my_ - ctrl->rect.y1);

  return bResult;
}
