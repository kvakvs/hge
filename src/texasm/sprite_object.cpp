/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#include "sprite_object.h"

CSpriteObject::CSpriteObject(hgeSprite *_spr, const char *_name, const int _resgroup,
                             const bool _owned)
        : CGfxObject(_name, _resgroup) {
  owned = _owned;
  spr = _spr;
}

CSpriteObject::~CSpriteObject() {
  if (owned) {
    hge->Texture_Free(CSpriteObject::GetTexture());
    delete spr;
  }
}


void CSpriteObject::GetSourcePos(int *_x, int *_y) {
  float x, y, w, h;

  spr->GetTextureRect(&x, &y, &w, &h);

  *_x = int(x);
  *_y = int(y);
}


bool CSpriteObject::SaveDescription(FILE *fp, char *texname) {
  fprintf(fp, "Sprite %s\n", name.c_str());
  fprintf(fp, "{\n");

  if (GetTexture() && texname) {
    fprintf(fp, " texture = %s\n", texname);
  }

  fprintf(fp, " rect = %d,%d,%d,%d\n", x, y, GetWidth(), GetHeight());

  float HotX, HotY;
  spr->GetHotSpot(&HotX, &HotY);
  if (HotX || HotY) {
    fprintf(fp, " hotspot = %d,%d\n", int(HotX), int(HotY));
  }

  const int blend_mode = spr->GetBlendMode();
  if (blend_mode != BLEND_DEFAULT) {
    fprintf(fp, " blendmode = ");

    if (blend_mode & BLEND_COLORADD) {
      fprintf(fp, "COLORADD,");
    } else {
      fprintf(fp, "COLORMUL,");
    }

    if (blend_mode & BLEND_ALPHABLEND) {
      fprintf(fp, "ALPHABLEND,");
    } else {
      fprintf(fp, "ALPHAADD,");
    }

    if (blend_mode & BLEND_ZWRITE) {
      fprintf(fp, "ZWRITE\n");
    } else {
      fprintf(fp, "NOZWRITE\n");
    }
  }

  hgeColor32 color = spr->GetColor();
  if (color != 0xFFFFFFFF) {
    fprintf(fp, " color = %08X\n", color.argb);
  }

  const auto z_order = spr->GetZ();
  if (z_order != 0.5f) {
    fprintf(fp, " zorder = %03f\n", z_order);
  }

  bool FlipX, FlipY;
  spr->GetFlip(&FlipX, &FlipY);
  if (FlipX || FlipY) {
    fprintf(fp, " flip = %s,%s\n", FlipX ? "true" : "false", FlipY ? "true" : "false");
  }

  fprintf(fp, " resgroup = %d\n", resgroup);

  fprintf(fp, "}\n");

  return true;
}
