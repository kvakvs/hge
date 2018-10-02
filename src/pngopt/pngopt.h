#include <stdio.h>

#include "../../include/hge.h"

extern HGE* hge;

bool write32_bit_png_with_pitch(FILE* fp, void* pBits, bool bNeedAlpha, int nWidth, int nHeight,
                            int nPitch);
