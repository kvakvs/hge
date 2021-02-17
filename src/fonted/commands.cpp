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
#include <windows.h>
#include <cstdio>
#include <string>
#include "hgeguictrls.h"
#include "hgeguirange.h"

using namespace std;

bool handle_keys(const int key) {
  switch (key) {
    case 0:
      return false;
    case HGEK_ESCAPE:
      return true;

    case HGEK_LBUTTON:
      if (state.mx_ > 168 && state.mx_ > psx - fw2 && state.mx_ < psx + fw2 &&
          state.my_ > psy - fh2 && state.my_ < psy + fh2) {
        state.drag_ = true;
        state.drag_x_offset_ = state.mx_;
        state.drag_y_offset_ = state.my_;
        state.drag_old_x_ = psx;
        state.drag_old_y_ = psy;
      }
      break;
    default:
      break;
  }

  return false;
}

bool do_commands(const int id) {
  hgeGUIListbox *listbox;
  int f, l;

  switch (id) {
    case 0:
      return false;
    case CMD_EXIT:
      return true;
    case CMD_HELP:
      state.help_ = BUTTON_GET_STATE(CMD_HELP);
      break;
    case CMD_SAVE:
      cmd_save_font();
      break;

    case CMD_BOLD:
      state.bold_ = BUTTON_GET_STATE(CMD_BOLD);
      cmd_generate_font();
      break;
    case CMD_ITALIC:
      state.italic_ = BUTTON_GET_STATE(CMD_ITALIC);
      cmd_generate_font();
      break;
    case CMD_ANTIALIAS:
      state.antialias_ = BUTTON_GET_STATE(CMD_ANTIALIAS);
      cmd_generate_font();
      break;

    case CMD_BOUNDINGBOX:
      state.b_box_ = BUTTON_GET_STATE(CMD_BOUNDINGBOX);
      break;

    case CMD_FONTSIZE:
      state.size_ = int(SLIDER_GET_VALUE(CMD_FONTSIZE));
      GET_TEXT_CTRL(CMD_TFONTSIZE)->printf("%d", state.size_);
      cmd_generate_font();
      break;

    case CMD_FAMILYLIST:
      listbox = hgeGetListboxCtrl(gui, CMD_FAMILYLIST);
      state.font_family_ = listbox->GetItemText(listbox->GetSelectedItem());
      SLIDER_SET_VALUE(CMD_FAMILYSLIDER, static_cast<float>(listbox->GetTopItem()));
      cmd_generate_font();
      break;

    case CMD_FAMILYSLIDER:
      listbox = hgeGetListboxCtrl(gui, CMD_FAMILYLIST);
      listbox->SetTopItem(int(SLIDER_GET_VALUE(CMD_FAMILYSLIDER)));
      break;

    case CMD_CHARRANGE:
      dynamic_cast<hgeGUIRange *>(gui->GetCtrl(CMD_CHARRANGE))->GetRange(&f, &l);
      state.sr_.First = f;
      state.sr_.Last = l;
      cmd_generate_font();
      break;

    default:
      break;
  }

  return false;
}

void cmd_generate_font() {
  delete spr_font;

  if (tex_font) {
    hge->Texture_Free(tex_font);
  }
  tex_font = generate_font(
          state.font_family_.c_str(),
          state.size_,
          state.pad_top_,
          state.pad_btm_,
          state.pad_lft_,
          state.pad_rgt_,
          state.italic_,
          state.bold_,
          state.antialias_,
          &state.sr_,
          1);
  spr_font = new hgeSprite(tex_font, 0.0f, 0.0f,
                           static_cast<float>(hge->Texture_GetWidth(tex_font)),
                           static_cast<float>(hge->Texture_GetHeight(tex_font)));
}

bool cmd_save_font() {
  OPENFILENAME ofn;
  char szFile[512] = "\0";
  char szTemp[512];

  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hge->System_GetState(HGE_HWND);
  ofn.lpstrFilter = "HGE Font Files\0*.fnt\0All Files\0*.*\0\0";
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.Flags = OFN_OVERWRITEPROMPT; // | OFN_NOCHANGEDIR
  ofn.lpstrDefExt = "fnt";
  if (!GetSaveFileName(&ofn)) {
    return true;
  }

  FILE *fp = fopen(szFile, "w");
  if (!fp) {
    sprintf(szTemp, "Can't write to file:\n%s\nProbaby it is write protected.", szFile);
    MessageBox(nullptr, szTemp, "Can't write to file",
               MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
    return false;
  }

  strcpy(&szFile[ofn.nFileExtension], "png");
  fprintf(fp, "[HGEFONT]\n\nBitmap=%s\n\n", &szFile[ofn.nFileOffset]);

  for (int i = state.sr_.First; i <= state.sr_.Last; i++) {
    const char c = i;
    if (i >= 32 && i <= 126)
      fprintf(fp, "Char=\"%c\",%d,%d,%d,%d,%d,%d\n",
              c, vChars[i].x, vChars[i].y, vChars[i].w, vChars[i].h,
              vChars[i].a, vChars[i].c);
    else
      fprintf(fp, "Char=%2X,%d,%d,%d,%d,%d,%d\n",
              i, vChars[i].x, vChars[i].y, vChars[i].w, vChars[i].h,
              vChars[i].a, vChars[i].c);
  }

  fclose(fp);

  return save_png(tex_font, szFile);
}
