/*
** Haaf's Game Engine 1.1
** Copyright (C) 2003, Relish Games
** hge.relishgames.com
**
** Bitmap Font Builder
*/


#pragma once


#include <string>
#include <hge.h>
#include <hgefont.h>
#include <hgegui.h>


#define BUTTON_GET_STATE(id)    ((hgeGUIButton*)gui->GetCtrl(id))->GetState()
#define BUTTON_SET_STATE(id, b)  ((hgeGUIButton*)gui->GetCtrl(id))->SetState(b)
#define SLIDER_GET_VALUE(id)    ((hgeGUISlider*)gui->GetCtrl(id))->GetValue()
#define SLIDER_SET_VALUE(id, f)  ((hgeGUISlider*)gui->GetCtrl(id))->SetValue(f)
#define GET_TEXT_CTRL(id)       ((hgeGUIText*)gui->GetCtrl(id))


#define MAX_TEXTURE_SIZE 1024


#define CHR_UPPER                1  // 0x41-0x5A
#define CHR_LOWER                2  // 0x61-0x7A
#define CHR_NUMBERS                4  // 0x30-0x39
#define CHR_PUNCT                8  // 0x21-0x2F, 0x3A-0x40, 0x5B-0x60, 0x7B-0x7E
#define CHR_UPPER_RUS        16 // 0xC0-0xDF
#define CHR_LOWER_RUS        32 // 0xE0-0xFF

#define CHR_ALL                        64 // 0x21-0xFF


struct CSymbolRange {
    uint16_t First;
    uint16_t Last;
};

struct CHAR_DESC {
    int x, y, w, h;
    int a, c;
};

extern CHAR_DESC vChars[256];


bool cmd_save_font();

void cmd_generate_font();

extern HGE *hge;

struct FEditorState {
    bool help_;
    bool b_box_;
    bool drag_;
    float drag_x_offset_;
    float drag_y_offset_;
    float drag_old_x_;
    float drag_old_y_;
    float mx_;
    float my_;

    std::string font_family_;
    int size_;
    int pad_top_;
    int pad_btm_;
    int pad_lft_;
    int pad_rgt_;
    bool bold_;
    bool italic_;
    bool antialias_;
    CSymbolRange sr_;
};

extern hgeFont *fnt;
extern hgeGUI *gui;
extern HTEXTURE tex_gui, tex_font;

extern hgeSprite *spr_left_pane1, *spr_left_pane2;
extern hgeSprite *spr_cursor;
extern hgeSprite *spr_font;

extern FEditorState state;


extern float psx, psy;
extern float fw2, fh2;

bool handle_keys(int key);

bool do_commands(int id);

bool save_png(HTEXTURE tex, char *filename);

HTEXTURE generate_font(const char *font_name, int size,
                       int padding_top, int padding_btm, int padding_lft,
                       int padding_rgt, bool italic, bool bold,
                       bool antialias, CSymbolRange *ranges, int range_count);

enum {
    CMD_EXIT = 1,
    CMD_HELP = 2,
    CMD_SAVE = 3,
    CMD_BOLD = 4,
    CMD_ITALIC = 5,
    CMD_ANTIALIAS = 6,
    CMD_BOUNDINGBOX = 7,
    CMD_CHARRANGE = 8,
    CMD_FONTSIZE = 9,
    CMD_TFONTSIZE = 10,
    CMD_FAMILYLIST = 11,
    CMD_FAMILYSLIDER = 12,
};
