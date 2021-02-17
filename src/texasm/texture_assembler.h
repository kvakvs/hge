/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once


#include "..\..\include\hgeresource.h"
#include "gfx_object.h"
#include "optimized_texture.h"


class CTextureAssembler {
public:
    ~CTextureAssembler() {
      ClearResources();
    }

    void AccumulateRMResources(hgeResourceManager *rm, int resgroup = 0, char *mask_set = nullptr,
                               bool bMaskInclusive = false);

    void AccumulateFileResources(char *wildcard, int resgroup = 0, char *mask_set = nullptr,
                                 bool bMaskInclusive = false);

    void ClearResources();

    void SetMaxTextureSize(const int w, const int h) {
      texture.SetMaxSize(w, h);
    }

    void SetMargin(const int margin) {
      texture.SetMargin(margin);
    }

    bool GenerateTextures(char *wildcard);

private:
    static bool CheckMask(const std::string &name, char *mask_set, bool bMaskInclusive);

    CGfxObject *FindObj(const GfxObjList& objlist, const char *name) const;

    GfxObjList obj_list;
    COptimizedTexture texture;
};
