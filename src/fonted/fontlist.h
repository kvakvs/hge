/*
** Haaf's Game Engine 1.1
** Copyright (C) 2003, Relish Games
** hge.relishgames.com
**
** Bitmap Font Builder
*/


#pragma once


#include <windows.h>
#include <stdint.h>


struct CFontListItem {
    char family[LF_FACESIZE];
    CFontListItem* next;
};

class CFontList {
    friend int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX* lpelfe, NEWTEXTMETRICEX* lpntme,
                                          unsigned int FontType, LPARAM lParam);

public:
    CFontList();
    ~CFontList();

    void BuildList();

    int GetNumFonts() {
        return fonts_;
    }

    char* GetFontByIdx(int n);

private:
    void ClearList();
    void FindSortAdd(char* family);

    int fonts_;
    CFontListItem* p_fonts_;
};

extern CFontList* FontList;
