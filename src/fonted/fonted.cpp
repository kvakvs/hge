/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Bitmap Font Builder
//


#include "fonted.h"
#include <cstdio>
#include "fontlist.h"
#include "hgeguictrls.h"
#include "hgeguirange.h"


HGE *hge = nullptr;


hgeFont *fnt;
hgeGUI *gui;
HTEXTURE tex_gui;
HTEXTURE tex_font = 0;

hgeSprite *spr_font = nullptr;
hgeSprite *spr_black;
hgeSprite *spr_left_pane1;
hgeSprite *spr_left_pane2;
hgeSprite *spr_cursor;

CFontList *font_list = nullptr;
FEditorState state;

float psx = 484;
float psy = 300;
float fw2;
float fh2;

void init_editor();

void done_editor();

void create_gui();

//HTEXTURE generate_font();


bool frame_func() {
  const auto dt = hge->Timer_GetDelta();

  // Update

  fw2 = spr_font->GetWidth() / 2;
  fh2 = spr_font->GetHeight() / 2;

  hge->Input_GetMousePos(&state.mx_, &state.my_);
  if (hge->Input_GetKeyState(HGEK_LBUTTON)) {
    if (state.drag_) {
      psx = state.drag_old_x_ + (state.mx_ - state.drag_x_offset_);
      psy = state.drag_old_y_ + (state.my_ - state.drag_y_offset_);
    }
  } else {
    state.drag_ = false;
  }

  if (handle_keys(hge->Input_GetKey())) {
    return true;
  }
  if (do_commands(gui->Update(dt))) {
    return true;
  }

  return false;
}

bool render_func() {
  int i;
  char szTemp[128];

  // Render

  hge->Gfx_BeginScene();
  hge->Gfx_Clear(hgeColor32(0xFF404040));

  spr_black->SetTextureRect(0, 0, spr_font->GetWidth(), spr_font->GetHeight());
  spr_black->Render(psx - fw2, psy - fh2);
  spr_font->Render(psx - fw2, psy - fh2);

  float u0, v0, u1, v1;

  if (state.b_box_)
    for (i = state.sr_.First; i <= state.sr_.Last; i++) {
      u0 = static_cast<float>(vChars[i].x) + psx - fw2;
      u1 = u0 + vChars[i].w;
      v0 = static_cast<float>(vChars[i].y) + psy - fh2;
      v1 = v0 + vChars[i].h;

      hge->Gfx_RenderLine(u0 + 0.5f, v0 + 0.5f, u1, v0 + 0.5f, hgeColor32(0xFF95883F));
      hge->Gfx_RenderLine(u1, v0 + 0.5f, u1, v1, hgeColor32(0xFF95883F));
      hge->Gfx_RenderLine(u1, v1, u0 + 0.5f, v1, hgeColor32(0xFF95883F));
      hge->Gfx_RenderLine(u0 + 0.5f, v1, u0 + 0.5f, v0 + 0.5f, hgeColor32(0xFF95883F));
    }

  sprintf(szTemp, "Texture size: %dx%d",
          static_cast<int>(spr_font->GetWidth()),
          static_cast<int>(spr_font->GetHeight()));
  fnt->SetColor(0xFF808080);
  fnt->Render(176, 580, HGETEXT_LEFT, szTemp);

  for (i = state.sr_.First; i <= state.sr_.Last; i++) {
    u0 = static_cast<float>(vChars[i].x) + psx - fw2;
    u1 = u0 + vChars[i].w;
    v0 = static_cast<float>(vChars[i].y) + psy - fh2;
    v1 = v0 + vChars[i].h;

    if (state.mx_ >= u0 && state.mx_ < u1 &&
        state.my_ >= v0 && state.my_ < v1) {
      hge->Gfx_RenderLine(u0 + 0.5f, v0 + 0.5f, u1, v0 + 0.5f, 0xFFFF0000);
      hge->Gfx_RenderLine(u1, v0 + 0.5f, u1, v1, 0xFFFF0000);
      hge->Gfx_RenderLine(u1, v1, u0 + 0.5f, v1, 0xFFFF0000);
      hge->Gfx_RenderLine(u0 + 0.5f, v1, u0 + 0.5f, v0 + 0.5f, 0xFFFF0000);

      if (i >= 32 && i <= 126)
        sprintf(szTemp, "\"%c\" = x:%d y:%d w:%d h:%d  a:%d c:%d",
                static_cast<char>(i), vChars[i].x, vChars[i].y,
                vChars[i].w, vChars[i].h,
                vChars[i].a, vChars[i].c);
      else
        sprintf(szTemp, "0x%02X = x:%d y:%d w:%d h:%d  a:%d c:%d",
                i, vChars[i].x, vChars[i].y, vChars[i].w, vChars[i].h,
                vChars[i].a, vChars[i].c);
      fnt->Render(790, 580, HGETEXT_RIGHT, szTemp);

    }
  }

  spr_left_pane1->Render(0, 0);
  spr_left_pane2->Render(0, 512);

  gui->Render();

  if (state.help_) {
    fnt->SetColor(0xFFFFFFFF);
    fnt->Render(189, 18, HGETEXT_LEFT, "Left mouse button - drag font texture\n"
                                       "Typefaces listbox - use Up/Down arrows, Mouse wheel\n"
                                       "Characters range - click and drag\n"
                                       "Esc - Exit\n\n"
                                       "A saved font includes two files: .FNT and .PNG\n"
                                       "You could apply any additional effects to the saved PNG in your graphics editor\n"
                                       "Edit FONTED.INI file to run in fullscreen");
  }
  if (hge->Input_IsMouseOver()) {
    spr_cursor->Render(state.mx_, state.my_);
  }

  hge->Gfx_EndScene();

  return false;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
  hge = hgeCreate(HGE_VERSION);

  hge->System_SetState(HGE_INIFILE, "fonted.ini");
  hge->System_SetState(HGE_LOGFILE, "fonted.log");
  hge->System_SetState(HGE_FRAMEFUNC, frame_func);
  hge->System_SetState(HGE_RENDERFUNC, render_func);
  hge->System_SetState(HGE_TITLE, "HGE Bitmap Font Builder");
  hge->System_SetState(HGE_SCREENWIDTH, 800);
  hge->System_SetState(HGE_SCREENHEIGHT, 600);
  hge->System_SetState(HGE_SCREENBPP, 32);
  hge->System_SetState(HGE_USESOUND, false);

  if (hge->Ini_GetInt("HGE", "FullScreen", 0)) {
    hge->System_SetState(HGE_WINDOWED, false);
  } else {
    hge->System_SetState(HGE_WINDOWED, true);
  }

  if (hge->System_Initiate()) {
    init_editor();
    hge->System_Start();
    done_editor();
  } else {
    MessageBox(nullptr, hge->System_GetErrorMessage(), "Error",
               MB_OK | MB_ICONERROR | MB_TASKMODAL);
  }

  hge->System_Shutdown();
  hge->Release();
  return 0;
}

void init_editor() {
  hge->Resource_AttachPack("fonted.paq");

  fnt = new hgeFont("font3.fnt");

  state.help_ = false;
  state.b_box_ = false;
  state.drag_ = false;

  font_list = new CFontList();
  font_list->BuildList();

  state.font_family_ = font_list->GetFontByIdx(0);
  state.size_ = 20;
  state.pad_top_ = hge->Ini_GetInt("HGE", "PaddingTop", 0);
  state.pad_btm_ = hge->Ini_GetInt("HGE", "PaddingBottom", 0);
  state.pad_lft_ = hge->Ini_GetInt("HGE", "PaddingLeft", 0);
  state.pad_rgt_ = hge->Ini_GetInt("HGE", "PaddingRight", 0);
  state.bold_ = false;
  state.italic_ = false;
  state.antialias_ = true;
  state.sr_.First = 32;
  state.sr_.Last = 126;

  cmd_generate_font();

  tex_gui = hge->Texture_Load("fgui.png");

  spr_cursor = new hgeSprite(tex_gui, 487, 181, 19, 26);
  spr_black = new hgeSprite(0, 0, 0, 100, 100);
  spr_black->SetColor(0xFF000000);

  spr_left_pane1 = new hgeSprite(tex_gui, 0, 0, 168, 512);
  spr_left_pane2 = new hgeSprite(tex_gui, 336, 0, 168, 88);

  gui = new hgeGUI();
  create_gui();
}

void done_editor() {
  delete gui;
  delete spr_left_pane1;
  delete spr_left_pane2;
  delete spr_cursor;
  delete fnt;
  delete spr_font;
  delete spr_black;

  delete font_list;

  hge->Texture_Free(tex_font);
  hge->Texture_Free(tex_gui);

  hge->Resource_RemoveAllPacks();
}

void create_gui() {
  gui->AddCtrl(new hgeGUIButton(CMD_SAVE, 9, 485, 47, 17, tex_gui, 336, 338));
  gui->AddCtrl(new hgeGUIButton(CMD_EXIT, 111, 485, 47, 17, tex_gui, 336, 338));
  auto *button = new hgeGUIButton(CMD_HELP, 60, 485, 47, 17, tex_gui, 336, 338);
  button->SetMode(true);
  gui->AddCtrl(button);

  button = new hgeGUIButton(CMD_BOLD, 9, 180, 8, 8, tex_gui, 368, 176);
  button->SetMode(true);
  button->SetState(state.bold_);
  gui->AddCtrl(button);
  button = new hgeGUIButton(CMD_ITALIC, 52, 180, 8, 8, tex_gui, 368, 176);
  button->SetMode(true);
  button->SetState(state.italic_);
  gui->AddCtrl(button);
  button = new hgeGUIButton(CMD_ANTIALIAS, 97, 180, 8, 8, tex_gui, 368, 176);
  button->SetMode(true);
  button->SetState(state.antialias_);
  gui->AddCtrl(button);

  button = new hgeGUIButton(CMD_BOUNDINGBOX, 9, 461, 8, 8, tex_gui, 368, 176);
  button->SetMode(true);
  button->SetState(state.b_box_);
  gui->AddCtrl(button);

  auto *listbox = new hgeGUIListbox(
          CMD_FAMILYLIST, 10, 44, 139, 128, fnt, 0xFF7697A4, 0xFFBBCBD2,
          0x40D4C25A);
  for (int i = 0; i < font_list->GetNumFonts(); i++) {
    listbox->AddItem(font_list->GetFontByIdx(i));
  }
  gui->AddCtrl(listbox);

  auto *slider = new hgeGUISlider(CMD_FAMILYSLIDER, 152, 44, 6, 128, tex_gui, 417, 177, 6,
                                  6, true);
  slider->SetMode(0, static_cast<float>(listbox->GetNumItems()) - listbox->GetNumRows(),
                  HGESLIDER_BAR);
  slider->SetValue(0);
  gui->AddCtrl(slider);

  auto range = new hgeGUIRange(CMD_CHARRANGE, 14, 266, 144, 144, 16, 16, 0x4D99FCD2);
  range->SetRange(state.sr_.First, state.sr_.Last);
  gui->AddCtrl(range);

  slider = new hgeGUISlider(CMD_FONTSIZE, 10, 219, 148, 6, tex_gui, 417, 177, 6, 6, false);
  slider->SetMode(5, 80, HGESLIDER_BAR);
  slider->SetValue(static_cast<float>(state.size_));
  gui->AddCtrl(slider);

  auto text = new hgeGUIText(CMD_TFONTSIZE, 116, 205, 28, 12, fnt);
  text->SetMode(HGETEXT_RIGHT);
  text->printf("%d", state.size_);
  gui->AddCtrl(text);
}
