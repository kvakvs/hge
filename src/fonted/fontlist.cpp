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

#include "fontlist.h"


CFontList::CFontList() {
  fonts_ = 0;
  p_fonts_ = nullptr;
}

CFontList::~CFontList() {
  ClearList();
}


int CALLBACK EnumFontFamExProc(
        ENUMLOGFONTEX *lpelfe, // logical-font data
        NEWTEXTMETRICEX *lpntme, // physical-font data
        unsigned int FontType, // type of font
        const LPARAM l_param // application-defined data
) {
  reinterpret_cast<CFontList *>(l_param)->FindSortAdd(lpelfe->elfLogFont.lfFaceName);
  return 1;
}


void CFontList::BuildList() {
  const auto hdc = CreateCompatibleDC(nullptr);
  LOGFONT lf;
  lf.lfCharSet = DEFAULT_CHARSET;
  lf.lfFaceName[0] = '\0';
  lf.lfPitchAndFamily = 0;

  EnumFontFamiliesEx(hdc, &lf, reinterpret_cast<FONTENUMPROC>(EnumFontFamExProc),
                     reinterpret_cast<LPARAM>(this), 0);
  DeleteDC(hdc);
}

void CFontList::ClearList() {
  CFontListItem *pItem = p_fonts_;

  while (pItem) {
    CFontListItem *pNext = pItem->next;
    delete pItem;
    pItem = pNext;
  }

  fonts_ = 0;
}

char *CFontList::GetFontByIdx(const int n) {
  CFontListItem *pItem = p_fonts_;

  if (n < 0 || n >= GetNumFonts()) {
    return nullptr;
  }

  for (int i = 0; i < n; i++) {
    pItem = pItem->next;
  }

  return pItem->family;
}

void CFontList::FindSortAdd(char *family) {
  CFontListItem *pItem = p_fonts_, *pPrev = nullptr, *pNew;

  while (pItem) {
    const int cmp = strcmp(pItem->family, family);
    if (!cmp) {
      return;
    }
    if (cmp > 0) {
      pNew = new CFontListItem;
      strcpy(pNew->family, family);
      pNew->next = pItem;
      if (pPrev) {
        pPrev->next = pNew;
      } else {
        p_fonts_ = pNew;
      }
      fonts_++;
      return;
    }
    pPrev = pItem;
    pItem = pItem->next;
  }

  pNew = new CFontListItem;
  strcpy(pNew->family, family);
  pNew->next = pItem;
  if (pPrev) {
    pPrev->next = pNew;
  } else {
    p_fonts_ = pNew;
  }
  fonts_++;
}
