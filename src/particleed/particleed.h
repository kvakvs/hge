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

#pragma once


#include <hge.h>
#include <hgeanim.h>
#include <hgefont.h>
#include <hgeparticle.h>
#include <hgegui.h>
#include <hgeguictrls.h>


#define PARTICLE_SIZE                        32

#define BUTTON_GET_STATE(id)                ((hgeGUIButton*)gui->GetCtrl(id))->GetState()
#define BUTTON_SET_STATE(id, b)        ((hgeGUIButton*)gui->GetCtrl(id))->SetState(b)
#define SLIDER_GET_VALUE(id)                ((hgeGUISlider*)gui->GetCtrl(id))->GetValue()
#define SLIDER_SET_VALUE(id, f)        ((hgeGUISlider*)gui->GetCtrl(id))->SetValue(f)
#define GET_TEXT_CTRL(id)                        ((hgeGUIText*)gui->GetCtrl(id))


extern HGE *hge;

struct PEditorState {
    bool bHelp;
    bool bBBox;
    bool bIFace;
    int nPreset;
    float mx, my;
    HTEXTURE texBG;
    hgeSprite *sprBG;
    hgeParticleSystem *ps;
};

extern hgeFont *fnt;
extern hgeGUI *gui;
extern HTEXTURE tex_gui, texParticle;

extern hgeSprite *spr_left_pane1, *spr_left_pane2, *sprRightPane1, *sprRightPane2;
extern hgeSprite *spr_cursor, *sprColor, *sprBBox;
extern hgeAnimation *sprParticles;

extern PEditorState state;

bool handle_keys(int key);

bool do_commands(int id);

void cmdChangePreset(int n);

void cmdSavePreset(int n);

void cmdLoadPreset(int n);

void cmdChangeColor(int id);

enum {
    CMD_EXIT = 1,
    CMD_HELP = 2,
    CMD_PRESET1 = 3,
    CMD_PRESET2 = 4,
    CMD_PRESET3 = 5,
    CMD_PRESET4 = 6,
    CMD_PRESET5 = 7,
    CMD_PRESET6 = 8,
    CMD_PRESET7 = 9,
    CMD_PRESET8 = 10,
    CMD_PRESET9 = 11,
    CMD_BOUNDINGBOX = 12,
    CMD_NPARTICLES = 13,
    CMD_FPS = 14,
    CMD_SYS_LIFETIME = 15,
    CMD_SYS_TLIFETIME = 16,
    CMD_SYS_LIFECONT = 17,
    CMD_SYS_EMISSION = 18,
    CMD_SYS_TEMISSION = 19,
    CMD_SYS_PARLIFETIMEMIN = 20,
    CMD_SYS_PARLIFETIMEMAX = 21,
    CMD_SYS_PARLIFETIMELOCK = 22,
    CMD_SYS_BLENDADDITIVE = 23,
    CMD_SYS_BLENDBLEND = 24,
    CMD_SYS_TEXTURE = 25,
    CMD_PM_DIRECTION = 26,
    CMD_PM_TDIRECTION = 27,
    CMD_PM_RELATIVE = 28,
    CMD_PM_SPREAD = 29,
    CMD_PM_TSPREAD = 30,
    CMD_PM_STARTSPEEDMIN = 31,
    CMD_PM_STARTSPEEDMAX = 32,
    CMD_PM_STARTSPEEDLOCK = 33,
    CMD_PM_GRAVITYMIN = 34,
    CMD_PM_GRAVITYMAX = 35,
    CMD_PM_GRAVITYLOCK = 36,
    CMD_PM_RADIALMIN = 37,
    CMD_PM_RADIALMAX = 38,
    CMD_PM_RADIALLOCK = 39,
    CMD_PM_TANGENTIALMIN = 40,
    CMD_PM_TANGENTIALMAX = 41,
    CMD_PM_TANGENTIALLOCK = 42,
    CMD_PA_SIZESTART = 43,
    CMD_PA_SIZEEND = 44,
    CMD_PA_SIZEVAR = 45,
    CMD_PA_SIZELOCK = 46,
    CMD_PA_SPINSTART = 47,
    CMD_PA_SPINEND = 48,
    CMD_PA_SPINVAR = 49,
    CMD_PA_SPINLOCK = 50,
    CMD_PA_ALPHASTART = 51,
    CMD_PA_ALPHAEND = 52,
    CMD_PA_ALPHAVAR = 53,
    CMD_PA_ALPHALOCK = 54,
    CMD_PA_RSTART = 55,
    CMD_PA_GSTART = 56,
    CMD_PA_BSTART = 57,
    CMD_PA_REND = 58,
    CMD_PA_GEND = 59,
    CMD_PA_BEND = 60,
    CMD_PA_RGBVAR = 61,
};
