/*
** Haaf's Game Engine 1.6
** Copyright (C) 2003, Relish Games
** hge.relishgames.com
**
** Bitmap Font Builder
*/


#include "fontlist.h"


CFontList::CFontList()
{
	nFonts=0;
	pFonts=0;
}

CFontList::~CFontList()
{
	ClearList();
}

void CFontList::BuildList()
{
	HDC hdc = CreateCompatibleDC(0);
	LOGFONT lf;
	lf.lfCharSet=DEFAULT_CHARSET;
	lf.lfFaceName[0]='\0';
	lf.lfPitchAndFamily=0;

    EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)this, 0);
	DeleteDC(hdc);
}

void CFontList::ClearList()
{
	CFontListItem *pItem=pFonts, *pNext;

	while(pItem)
	{
		pNext=pItem->next;
		delete pItem;
		pItem=pNext;
	}
	
	nFonts=0;
}

char *CFontList::GetFontByIdx(int n)
{
	int i;
	CFontListItem *pItem=pFonts;

	if(n<0 || n>=GetNumFonts()) return 0;

	for(i=0;i<n;i++) pItem=pItem->next;

	return pItem->family;
}

void CFontList::FindSortAdd(char *family)
{
	int cmp;
	CFontListItem *pItem=pFonts, *pPrev=0, *pNew;

	while(pItem)
	{
		cmp=strcmp(pItem->family, family);
		if(!cmp) return;
		if(cmp>0)
		{
			pNew = new CFontListItem;
			strcpy(pNew->family, family);
			pNew->next=pItem;
			if(pPrev) pPrev->next=pNew;
			else pFonts=pNew;
			nFonts++;
			return;
		}
		pPrev=pItem;
		pItem=pItem->next;
	}

	pNew = new CFontListItem;
	strcpy(pNew->family, family);
	pNew->next=pItem;
	if(pPrev) pPrev->next=pNew;
	else pFonts=pNew;
	nFonts++;
}

int CALLBACK EnumFontFamExProc(
  ENUMLOGFONTEX *lpelfe,    // logical-font data
  NEWTEXTMETRICEX *lpntme,  // physical-font data
  DWORD FontType,           // type of font
  LPARAM lParam             // application-defined data
)
{
	((CFontList *)lParam)->FindSortAdd(lpelfe->elfLogFont.lfFaceName);
	return 1;
}

