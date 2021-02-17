/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#include <cstdio>

#include <hge.h>

extern HGE *hge;

bool write32_bit_png_with_pitch(FILE *fp, void *pBits, bool bNeedAlpha, int nWidth, int nHeight,
                                int nPitch);
