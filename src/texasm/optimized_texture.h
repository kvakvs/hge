/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once


#include "texasm.h"
#include "gfx_object.h"
#include "RectPlacement.h"


bool write32_bit_png_with_pitch(FILE *fp, void *pBits, bool bNeedAlpha, int nWidth, int nHeight,
                                int nPitch);


struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};


class COptimizedTexture {
public:
    COptimizedTexture();

    ~COptimizedTexture() {
      Reset();
    }

    void Reset();

    void SetMaxSize(const int w, const int h) {
      maxw = w;
      maxh = h;
    }

    void SetMargin(const int margin) {
      placer.SetMargin(margin);
    }

    bool PlaceObject(CGfxObject *obj);

    void AddNoTextureObject(CGfxObject *obj);

    int GetNumPlaced() {
      return obj_list.size();
    }

    bool Create();

    bool CopyData();

    bool OptimizeAlpha() const;

    bool Save(char *filename) const;

    bool SaveDescriptions(char *resfile, char *texfile, char *texname);

private:
    int maxw;
    int maxh;

    GfxObjList obj_list;
    CRectPlacement placer;
    int texw;
    int texh;

    HTEXTURE tex;
    CColor *tex_data;
    int pitch;
};
