/*
** Haaf's Game Engine 1.1
** Copyright (C) 2003, Relish Games
** hge.relishgames.com
**
** Bitmap Font Builder
*/


#ifndef FONTLIST_H
#define FONTLIST_H


#include <windows.h>


struct CFontListItem
{
	char		  family[LF_FACESIZE];
	CFontListItem *next;
};

class CFontList
{
	friend int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam);

public:
	CFontList();
	~CFontList();

	void	BuildList();
	int		GetNumFonts() { return nFonts; }
	char	*GetFontByIdx(int n);

private:
	void			ClearList();
	void			FindSortAdd(char *family);

	int				nFonts;
	CFontListItem	*pFonts;
};

extern CFontList	*FontList;


#endif FONTLIST_H
