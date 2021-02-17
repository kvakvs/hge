/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once

#include "gfx_object.h"
#include <hgesprite.h>


class CSpriteObject : public CGfxObject {
public:
    CSpriteObject(hgeSprite *_spr, const char *_name, int _resgroup, bool _owned);

    ~CSpriteObject();

    int GetWidth() const override {
      return spr ? int(spr->GetWidth()) : 0;
    }

    int GetHeight() const override {
      return spr ? int(spr->GetHeight()) : 0;
    }

    HTEXTURE GetTexture() override {
      return spr ? spr->GetTexture() : 0;
    }

    void GetSourcePos(int *_x, int *_y) override;

    bool SaveDescription(FILE *fp, char *texname) override;

private:
    hgeSprite *spr;
    bool owned;
};
