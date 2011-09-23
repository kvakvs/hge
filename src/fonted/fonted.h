/*
** Haaf's Game Engine 1.1
** Copyright (C) 2003, Relish Games
** hge.relishgames.com
**
** Bitmap Font Builder
*/


#ifndef FONTED_H
#define FONTED_H


#include "..\..\include\hge.h"
#include "..\..\include\hgefont.h"
#include "..\..\include\hgegui.h"
#include "..\..\include\hgeguictrls.h"

#include "fontlist.h"
#include "hgeguirange.h"


#define ButtonGetState(id)		((hgeGUIButton*)gui->GetCtrl(id))->GetState()
#define ButtonSetState(id,b)	((hgeGUIButton*)gui->GetCtrl(id))->SetState(b)
#define SliderGetValue(id)		((hgeGUISlider*)gui->GetCtrl(id))->GetValue()
#define SliderSetValue(id,f)	((hgeGUISlider*)gui->GetCtrl(id))->SetValue(f)
#define GetTextCtrl(id)			((hgeGUIText*)gui->GetCtrl(id))


#define MAX_TEXTURE_SIZE 1024


#define CHR_UPPER		1  // 0x41-0x5A
#define CHR_LOWER		2  // 0x61-0x7A
#define CHR_NUMBERS		4  // 0x30-0x39
#define CHR_PUNCT		8  // 0x21-0x2F, 0x3A-0x40, 0x5B-0x60, 0x7B-0x7E 
#define CHR_UPPER_RUS	16 // 0xC0-0xDF
#define CHR_LOWER_RUS	32 // 0xE0-0xFF

#define CHR_ALL			64 // 0x21-0xFF


struct CSymbolRange	
{
	unsigned short	First;
	unsigned short	Last;
};

struct CHAR_DESC
{
	int x, y, w, h;
	int a, c;
};

extern CHAR_DESC vChars[256];


bool cmdSaveFont();
void cmdGenerateFont();

extern HGE *hge;

struct FEditorState
{
	bool				bHelp;
	bool				bBBox;
	bool				bDrag;
	float				nDragXOffset, nDragYOffset;
	float				nDragOldX, nDragOldY;
	float				mx, my;

	char				*FontFamily;
	int					nSize;
	int					nPadTop, nPadBtm, nPadLft, nPadRgt;
	bool				bBold;
	bool				bItalic;
	bool				bAntialias;
	CSymbolRange		sr;
};

extern hgeFont		*fnt;
extern hgeGUI		*gui;
extern HTEXTURE		texGui, texFont;

extern hgeSprite	*sprLeftPane1, *sprLeftPane2;
extern hgeSprite	*sprCursor;
extern hgeSprite	*sprFont;

extern FEditorState state;


extern float		psx, psy;
extern float		fw2, fh2;

bool HandleKeys(int key);
bool DoCommands(int id);
bool SavePNG(HTEXTURE tex, char *filename);
HTEXTURE FontGenerate(char *szFontName,
					  int nSize,
					  int nPaddingTop, int nPaddingBtm, int nPaddingLft, int nPaddingRgt,
					  bool bItalic,
					  bool bBold,
					  bool bAntialias,
					  CSymbolRange *pRanges,
					  int nRangeCount);

#define CMD_EXIT				1
#define CMD_HELP				2
#define CMD_SAVE				3
#define CMD_BOLD				4
#define CMD_ITALIC				5
#define CMD_ANTIALIAS			6
#define CMD_BOUNDINGBOX			7
#define CMD_CHARRANGE			8
#define CMD_FONTSIZE			9
#define CMD_TFONTSIZE			10
#define CMD_FAMILYLIST			11
#define CMD_FAMILYSLIDER		12


#endif
