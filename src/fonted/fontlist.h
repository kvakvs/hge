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

#pragma once


#include <windows.h>
#include <cstdint>


struct CFontListItem {
    char family[LF_FACESIZE];
    CFontListItem *next;
};

class CFontList {
    friend int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme,
                                          unsigned int FontType, LPARAM lParam);

public:
    CFontList();

    ~CFontList();

    void BuildList();

    int GetNumFonts() const {
      return fonts_;
    }

    char *GetFontByIdx(int n);

private:
    void ClearList();

    void FindSortAdd(char *family);

    int fonts_;
    CFontListItem *p_fonts_;
};

extern CFontList *font_list;
