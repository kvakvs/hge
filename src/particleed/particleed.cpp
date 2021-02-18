/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Particle systems editor
//

#include "particleed.h"


HGE *hge = nullptr;


hgeFont *fnt;
hgeGUI *gui;
HTEXTURE tex_gui, texParticles;

hgeSprite *spr_left_pane1, *spr_left_pane2, *sprRightPane1, *sprRightPane2;
hgeSprite *spr_cursor, *sprColor, *sprBBox;
hgeAnimation *sprParticles;

PEditorState state;

float psx = 400, psy = 300;

void init_editor();

void done_editor();

void create_gui();


bool frame_func() {
  float px, py;
  const auto dt = hge->Timer_GetDelta();

  // Update

  hge->Input_GetMousePos(&state.mx, &state.my);
  if (hge->Input_GetKeyState(HGEK_RBUTTON)) {
    psx = state.mx;
    psy = state.my;
  } else {
    psx = 400;
    psy = 300;
  }

  if (state.bIFace) {
    if (psx > 631) {
      psx = 631;
    }
    if (psx < 168) {
      psx = 168;
    }
  }

  state.ps->GetPosition(&px, &py);
  state.ps->MoveTo(px + (psx - px) * 10 * dt, py + (psy - py) * 10 * dt);
  state.ps->Update(dt);

  if (handle_keys(hge->Input_GetKey())) {
    return true;
  }

  if (state.bIFace) {
    if (do_commands(gui->Update(dt))) {
      return true;
    }
  }

  GET_TEXT_CTRL(CMD_NPARTICLES)->printf("%d", state.ps->GetParticlesAlive());
  GET_TEXT_CTRL(CMD_FPS)->printf("%d", hge->Timer_GetFPS());

  return false;
}

bool render_func() {
  hgeRect bbox;

  // Render

  hge->Gfx_Clear(0);
  hge->Gfx_BeginScene();

  if (state.sprBG) {
    state.sprBG->Render(400, 300);
  }

  state.ps->Render();

  if (state.bBBox) {
    state.ps->GetBoundingBox(&bbox);
    if (!bbox.IsClean()) {
      sprBBox->RenderStretch(bbox.x1, bbox.y1, bbox.x2, bbox.y2);
    }
  }

  if (state.bIFace) {
    spr_left_pane1->Render(0, 0);
    spr_left_pane2->Render(0, 512);
    sprRightPane1->Render(632, 0);
    sprRightPane2->Render(632, 512);

    gui->Render();
    sprParticles->SetColor(state.ps->info.colColorStart.GetHWColor().argb | 0xFF000000);
    sprParticles->Render(26, 189);
    sprColor->Render(642, 221);
  }

  if (state.bHelp) {
    fnt->SetColor(0xFFFFFFFF);
    fnt->Render(189, 18, HGETEXT_LEFT, "Left mouse button - fire particle system\n"
                                       "Right mouse button - move the system with mouse\n\n"
                                       "Keys 1 to 9 - select preset\nPresets are saved and loaded automatically\n\n"
                                       "TAB - Hide editor panels\n"
                                       "Esc - Exit\n\n"
                                       "Edit PARTICLEED.INI file to change backdrop or fullscreen/windowed mode");
  }

  if (hge->Input_IsMouseOver() && !hge->Input_GetKeyState(HGEK_RBUTTON)) {
    spr_cursor->Render(state.mx, state.my);
  }

  hge->Gfx_EndScene();

  return false;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
  hge = hgeCreate(HGE_VERSION);

  hge->System_SetState(HGE_INIFILE, "particleed.ini");
  hge->System_SetState(HGE_LOGFILE, "particleed.log");
  hge->System_SetState(HGE_FRAMEFUNC, frame_func);
  hge->System_SetState(HGE_RENDERFUNC, render_func);
  hge->System_SetState(HGE_TITLE, "HGE Particle Systems Editor");
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
               MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
  }

  hge->System_Shutdown();
  hge->Release();
  return 0;
}

void init_editor() {
  hgeParticleSystemInfo psi;

  state.texBG = 0;
  state.sprBG = nullptr;

  const char *bgName = hge->Ini_GetString("HGE", "Background", nullptr);
  if (bgName) {
    state.texBG = hge->Texture_Load(bgName);
    const auto bgw = hge->Texture_GetWidth(state.texBG, true);
    const auto bgh = hge->Texture_GetHeight(state.texBG, true);
    state.sprBG = new hgeSprite(state.texBG, 0, 0, static_cast<float>(bgw),
                                static_cast<float>(bgh));
    state.sprBG->SetHotSpot(static_cast<float>(bgw) / 2,
                            static_cast<float>(bgh) / 2);
  }

  hge->Resource_AttachPack("particleed.paq");

  state.bIFace = true;
  state.bHelp = false;
  state.bBBox = false;
  state.nPreset = 0;

  texParticles = hge->Texture_Load("particles.png");
  sprParticles = new hgeAnimation(texParticles, 16, 1.0f, 0, 0, 32, 32);
  sprParticles->SetHotSpot(16, 16);
  memset(&psi, 0, sizeof(hgeParticleSystemInfo));
  psi.sprite = sprParticles;
  state.ps = new hgeParticleSystem(&psi);
  state.ps->TrackBoundingBox(true);
  state.ps->MoveTo(psx, psy);

  fnt = new hgeFont("font3.fnt");
  tex_gui = hge->Texture_Load("pgui.png");

  spr_cursor = new hgeSprite(tex_gui, 487, 181, 19, 26);
  sprColor = new hgeSprite(tex_gui, 466, 208, 14, 80);
  sprBBox = new hgeSprite(tex_gui, 337, 209, 128, 128);

  spr_left_pane1 = new hgeSprite(tex_gui, 0, 0, 168, 512);
  spr_left_pane2 = new hgeSprite(tex_gui, 336, 0, 168, 88);
  sprRightPane1 = new hgeSprite(tex_gui, 168, 0, 168, 512);
  sprRightPane2 = new hgeSprite(tex_gui, 336, 88, 168, 88);

  gui = new hgeGUI();
  create_gui();

  cmdLoadPreset(state.nPreset);
}

void done_editor() {
  cmdSavePreset(state.nPreset);

  if (state.sprBG) {
    delete state.sprBG;
  }
  if (state.texBG) {
    hge->Texture_Free(state.texBG);
  }

  delete gui;
  delete spr_left_pane1;
  delete spr_left_pane2;
  delete sprRightPane1;
  delete sprRightPane2;
  delete sprBBox;
  delete sprColor;
  delete spr_cursor;
  delete fnt;
  delete state.ps;
  delete sprParticles;

  hge->Texture_Free(texParticles);
  hge->Texture_Free(tex_gui);

  hge->Resource_RemoveAllPacks();
}

void create_gui() {

  // Presets & stuff

  gui->AddCtrl(new hgeGUIButton(CMD_EXIT, 718, 484, 73, 17, tex_gui, 336, 191));
  hgeGUIButton *button = new hgeGUIButton(CMD_HELP, 641, 484, 73, 17, tex_gui, 336, 191);
  button->SetMode(true);
  gui->AddCtrl(button);

  button = new hgeGUIButton(CMD_PRESET1, 641, 389, 16, 15, tex_gui, 384, 176);
  button->SetMode(true);
  button->SetState(true);
  gui->AddCtrl(button);
  button = new hgeGUIButton(CMD_PRESET2, 657, 389, 16, 15, tex_gui, 384, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  button = new hgeGUIButton(CMD_PRESET3, 673, 389, 16, 15, tex_gui, 384, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  button = new hgeGUIButton(CMD_PRESET4, 689, 389, 16, 15, tex_gui, 384, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  button = new hgeGUIButton(CMD_PRESET5, 705, 389, 16, 15, tex_gui, 384, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  button = new hgeGUIButton(CMD_PRESET6, 721, 389, 16, 15, tex_gui, 384, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  button = new hgeGUIButton(CMD_PRESET7, 737, 389, 16, 15, tex_gui, 384, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  button = new hgeGUIButton(CMD_PRESET8, 753, 389, 16, 15, tex_gui, 384, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  button = new hgeGUIButton(CMD_PRESET9, 769, 389, 16, 15, tex_gui, 384, 176);
  button->SetMode(true);
  gui->AddCtrl(button);

  button = new hgeGUIButton(CMD_BOUNDINGBOX, 641, 417, 8, 8, tex_gui, 368, 176);
  button->SetMode(true);
  gui->AddCtrl(button);

  hgeGUIText *text = new hgeGUIText(CMD_FPS, 641, 456, 28, 16, fnt);
  text->SetMode(HGETEXT_RIGHT);
  text->SetColor(0xFFD4C25A);
  gui->AddCtrl(text);
  text = new hgeGUIText(CMD_NPARTICLES, 641, 437, 28, 16, fnt);
  text->SetMode(HGETEXT_RIGHT);
  text->SetColor(0xFFD4C25A);
  gui->AddCtrl(text);

  // System parameters

  hgeGUISlider *slider = new hgeGUISlider(CMD_SYS_LIFETIME, 10, 44, 149, 6, tex_gui, 417, 177, 6,
                                          6);
  slider->SetMode(0, 10, HGESLIDER_BAR);
  slider->SetValue(5);
  gui->AddCtrl(slider);
  text = new hgeGUIText(CMD_SYS_TLIFETIME, 113, 30, 28, 12, fnt);
  text->SetMode(HGETEXT_RIGHT);
  text->SetText("0");
  gui->AddCtrl(text);
  button = new hgeGUIButton(CMD_SYS_LIFECONT, 9, 55, 8, 8, tex_gui, 368, 176);
  button->SetMode(true);
  gui->AddCtrl(button);

  slider = new hgeGUISlider(CMD_SYS_EMISSION, 10, 91, 149, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 1000, HGESLIDER_BAR);
  slider->SetValue(500);
  gui->AddCtrl(slider);
  text = new hgeGUIText(CMD_SYS_TEMISSION, 103, 77, 28, 12, fnt);
  text->SetMode(HGETEXT_RIGHT);
  text->SetText("0");
  gui->AddCtrl(text);

  button = new hgeGUIButton(CMD_SYS_PARLIFETIMELOCK, 144, 111, 16, 11, tex_gui, 336, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  slider = new hgeGUISlider(CMD_SYS_PARLIFETIMEMIN, 33, 126, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 5, HGESLIDER_BAR);
  slider->SetValue(2.5);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_SYS_PARLIFETIMEMAX, 33, 138, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 5, HGESLIDER_BAR);
  slider->SetValue(2.5);
  gui->AddCtrl(slider);

  button = new hgeGUIButton(CMD_SYS_BLENDADDITIVE, 72, 174, 8, 8, tex_gui, 368, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  button = new hgeGUIButton(CMD_SYS_BLENDBLEND, 72, 188, 8, 8, tex_gui, 368, 176);
  button->SetMode(true);
  button->SetState(true);
  gui->AddCtrl(button);
  button = new hgeGUIButton(CMD_SYS_TEXTURE, 10, 173, 32, 32, tex_gui, 336, 338);
  button->SetMode(true);
  gui->AddCtrl(button);

  // Particle movement

  slider = new hgeGUISlider(CMD_PM_DIRECTION, 9, 267, 149, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 2 * M_PI, HGESLIDER_BAR);
  slider->SetValue(M_PI);
  gui->AddCtrl(slider);
  text = new hgeGUIText(CMD_PM_TDIRECTION, 112, 253, 28, 12, fnt);
  text->SetMode(HGETEXT_RIGHT);
  text->SetText("0");
  gui->AddCtrl(text);
  button = new hgeGUIButton(CMD_PM_RELATIVE, 8, 278, 8, 8, tex_gui, 368, 176);
  button->SetMode(true);
  gui->AddCtrl(button);

  slider = new hgeGUISlider(CMD_PM_SPREAD, 9, 314, 149, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 2 * M_PI, HGESLIDER_BAR);
  slider->SetValue(M_PI);
  gui->AddCtrl(slider);
  text = new hgeGUIText(CMD_PM_TSPREAD, 112, 300, 28, 12, fnt);
  text->SetMode(HGETEXT_RIGHT);
  text->SetText("0");
  gui->AddCtrl(text);

  button = new hgeGUIButton(CMD_PM_STARTSPEEDLOCK, 143, 334, 16, 11, tex_gui, 336, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  slider = new hgeGUISlider(CMD_PM_STARTSPEEDMIN, 32, 349, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(-300, 300, HGESLIDER_BARRELATIVE);
  slider->SetValue(0);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PM_STARTSPEEDMAX, 32, 361, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(-300, 300, HGESLIDER_BARRELATIVE);
  slider->SetValue(0);
  gui->AddCtrl(slider);

  button = new hgeGUIButton(CMD_PM_GRAVITYLOCK, 143, 381, 16, 11, tex_gui, 336, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  slider = new hgeGUISlider(CMD_PM_GRAVITYMIN, 32, 396, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(-900, 900, HGESLIDER_BARRELATIVE);
  slider->SetValue(0);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PM_GRAVITYMAX, 32, 408, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(-900, 900, HGESLIDER_BARRELATIVE);
  slider->SetValue(0);
  gui->AddCtrl(slider);

  button = new hgeGUIButton(CMD_PM_RADIALLOCK, 143, 428, 16, 11, tex_gui, 336, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  slider = new hgeGUISlider(CMD_PM_RADIALMIN, 32, 443, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(-900, 900, HGESLIDER_BARRELATIVE);
  slider->SetValue(0);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PM_RADIALMAX, 32, 455, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(-900, 900, HGESLIDER_BARRELATIVE);
  slider->SetValue(0);
  gui->AddCtrl(slider);

  button = new hgeGUIButton(CMD_PM_TANGENTIALLOCK, 143, 475, 16, 11, tex_gui, 336, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  slider = new hgeGUISlider(CMD_PM_TANGENTIALMIN, 32, 490, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(-900, 900, HGESLIDER_BARRELATIVE);
  slider->SetValue(0);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PM_TANGENTIALMAX, 32, 502, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(-900, 900, HGESLIDER_BARRELATIVE);
  slider->SetValue(0);
  gui->AddCtrl(slider);

  // Particle appearance

  button = new hgeGUIButton(CMD_PA_SIZELOCK, 775, 29, 16, 11, tex_gui, 336, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  slider = new hgeGUISlider(CMD_PA_SIZESTART, 664, 44, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(1, 100, HGESLIDER_BAR);
  slider->SetValue(32);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PA_SIZEEND, 664, 56, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(1, 100, HGESLIDER_BAR);
  slider->SetValue(32);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PA_SIZEVAR, 664, 68, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 1, HGESLIDER_BAR);
  slider->SetValue(0.5);
  gui->AddCtrl(slider);

  button = new hgeGUIButton(CMD_PA_SPINLOCK, 775, 88, 16, 11, tex_gui, 336, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  slider = new hgeGUISlider(CMD_PA_SPINSTART, 664, 103, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(-50, 50, HGESLIDER_BARRELATIVE);
  slider->SetValue(0);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PA_SPINEND, 664, 115, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(-50, 50, HGESLIDER_BARRELATIVE);
  slider->SetValue(0);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PA_SPINVAR, 664, 127, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 1, HGESLIDER_BAR);
  slider->SetValue(0.5);
  gui->AddCtrl(slider);

  button = new hgeGUIButton(CMD_PA_ALPHALOCK, 775, 147, 16, 11, tex_gui, 336, 176);
  button->SetMode(true);
  gui->AddCtrl(button);
  slider = new hgeGUISlider(CMD_PA_ALPHASTART, 664, 162, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 1, HGESLIDER_BAR);
  slider->SetValue(0.5);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PA_ALPHAEND, 664, 174, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 1, HGESLIDER_BAR);
  slider->SetValue(0.5);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PA_ALPHAVAR, 664, 186, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 1, HGESLIDER_BAR);
  slider->SetValue(0.5);
  gui->AddCtrl(slider);

  slider = new hgeGUISlider(CMD_PA_RSTART, 664, 221, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 1, HGESLIDER_BAR);
  slider->SetValue(0.5);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PA_GSTART, 664, 233, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 1, HGESLIDER_BAR);
  slider->SetValue(0.5);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PA_BSTART, 664, 245, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 1, HGESLIDER_BAR);
  slider->SetValue(0.5);
  gui->AddCtrl(slider);

  slider = new hgeGUISlider(CMD_PA_REND, 664, 271, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 1, HGESLIDER_BAR);
  slider->SetValue(0.5);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PA_GEND, 664, 283, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 1, HGESLIDER_BAR);
  slider->SetValue(0.5);
  gui->AddCtrl(slider);
  slider = new hgeGUISlider(CMD_PA_BEND, 664, 295, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 1, HGESLIDER_BAR);
  slider->SetValue(0.5);
  gui->AddCtrl(slider);

  slider = new hgeGUISlider(CMD_PA_RGBVAR, 664, 321, 126, 6, tex_gui, 417, 177, 6, 6);
  slider->SetMode(0, 1, HGESLIDER_BAR);
  slider->SetValue(0.5);
  gui->AddCtrl(slider);
}
