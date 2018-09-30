#include <stdio.h>

#include "../../include/hge.h"

extern HGE* hge;

bool Write32BitPNGWithPitch(FILE* fp, void* pBits, bool bNeedAlpha, int nWidth, int nHeight,
                            int nPitch);
