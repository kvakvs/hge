/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003, Relish Games
** hge.relishgames.com
**
** Bitmap Font Builder
*/


#include "fonted.h"
#include <stdio.h>


HGE *hge=0;


hgeFont			*fnt;
hgeGUI			*gui;
HTEXTURE		texGui;
HTEXTURE		texFont=0;

hgeSprite		*sprFont=0, *sprBlack;
hgeSprite		*sprLeftPane1, *sprLeftPane2;
hgeSprite		*sprCursor;

CFontList		*FontList=0;
FEditorState	state;

float			psx=484, psy=300;
float			fw2, fh2;

void			InitEditor();
void			DoneEditor();
void			CreateGUI();

HTEXTURE		FontGenerate();


bool FrameFunc()
{
	float		dt=hge->Timer_GetDelta();

	// Update
	
	fw2=sprFont->GetWidth()/2;
	fh2=sprFont->GetHeight()/2;

	hge->Input_GetMousePos(&state.mx, &state.my);
	if(hge->Input_GetKeyState(HGEK_LBUTTON))
	{
		if(state.bDrag)
		{
			psx=state.nDragOldX+(state.mx-state.nDragXOffset);
			psy=state.nDragOldY+(state.my-state.nDragYOffset);
		}
	}
	else state.bDrag=false;

	if(HandleKeys(hge->Input_GetKey())) return true;
	if(DoCommands(gui->Update(dt))) return true;

	return false;
}

bool RenderFunc()
{
	int i;
	char		szTemp[128];

	// Render
	
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0xFF404040);

	sprBlack->SetTextureRect(0,0,sprFont->GetWidth(),sprFont->GetHeight());
	sprBlack->Render(psx-fw2, psy-fh2);
	sprFont->Render(psx-fw2, psy-fh2);

	float u0,v0,u1,v1;

	if(state.bBBox)
		for(i=state.sr.First;i<=state.sr.Last;i++)
		{
			u0=(float)vChars[i].x+psx-fw2;
			u1=u0+vChars[i].w;
			v0=(float)vChars[i].y+psy-fh2;
			v1=v0+vChars[i].h;

			hge->Gfx_RenderLine(u0+0.5f, v0+0.5f, u1,      v0+0.5f, 0xFF95883F);
			hge->Gfx_RenderLine(u1,      v0+0.5f, u1,      v1,      0xFF95883F);
			hge->Gfx_RenderLine(u1,      v1,      u0+0.5f, v1,      0xFF95883F);
			hge->Gfx_RenderLine(u0+0.5f, v1,      u0+0.5f, v0+0.5f, 0xFF95883F);
		}

	sprintf(szTemp,"Texture size: %dx%d",(int)sprFont->GetWidth(),(int)sprFont->GetHeight());
	fnt->SetColor(0xFF808080);
	fnt->Render(176, 580, HGETEXT_LEFT, szTemp);

	for(i=state.sr.First;i<=state.sr.Last;i++)
	{
		u0=(float)vChars[i].x+psx-fw2;
		u1=u0+vChars[i].w;
		v0=(float)vChars[i].y+psy-fh2;
		v1=v0+vChars[i].h;

		if(state.mx>=u0 && state.mx<u1 &&
			state.my>=v0 && state.my<v1)
		{
			hge->Gfx_RenderLine(u0+0.5f, v0+0.5f, u1,      v0+0.5f, 0xFFFF0000);
			hge->Gfx_RenderLine(u1,      v0+0.5f, u1,      v1,      0xFFFF0000);
			hge->Gfx_RenderLine(u1,      v1,      u0+0.5f, v1,      0xFFFF0000);
			hge->Gfx_RenderLine(u0+0.5f, v1,      u0+0.5f, v0+0.5f, 0xFFFF0000);

			if(i>=32 && i<=126)
				sprintf(szTemp,"\"%c\" = x:%d y:%d w:%d h:%d  a:%d c:%d",
					(char)i, vChars[i].x, vChars[i].y, vChars[i].w, vChars[i].h,
					vChars[i].a, vChars[i].c);
			else
				sprintf(szTemp,"0x%02X = x:%d y:%d w:%d h:%d  a:%d c:%d",
					i, vChars[i].x, vChars[i].y, vChars[i].w, vChars[i].h,
					vChars[i].a, vChars[i].c);
			fnt->Render(790, 580, HGETEXT_RIGHT, szTemp);

		}
	}
		
	sprLeftPane1->Render(0,0);
	sprLeftPane2->Render(0,512);
	
	gui->Render();

	if(state.bHelp)
	{
		fnt->SetColor(0xFFFFFFFF);
		fnt->Render(189, 18, HGETEXT_LEFT, "Left mouse button - drag font texture\n"
			"Typefaces listbox - use Up/Down arrows, Mouse wheel\n"
			"Characters range - click and drag\n"
			"Esc - Exit\n\n"
			"A saved font includes two files: .FNT and .PNG\n"
			"You could apply any additional effects to the saved PNG in your graphics editor\n"
			"Edit FONTED.INI file to run in fullscreen");
	}
	if(hge->Input_IsMouseOver()) sprCursor->Render(state.mx, state.my);

	hge->Gfx_EndScene();

	return false;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	hge = hgeCreate(HGE_VERSION);

	hge->System_SetState(HGE_INIFILE, "fonted.ini");
	hge->System_SetState(HGE_LOGFILE, "fonted.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "HGE Bitmap Font Builder");
	hge->System_SetState(HGE_SCREENWIDTH, 800);
	hge->System_SetState(HGE_SCREENHEIGHT, 600);
	hge->System_SetState(HGE_SCREENBPP, 32);
	hge->System_SetState(HGE_USESOUND, false);

	if(hge->Ini_GetInt("HGE", "FullScreen",0))	hge->System_SetState(HGE_WINDOWED, false);
	else hge->System_SetState(HGE_WINDOWED, true);

	if(hge->System_Initiate())
	{
		InitEditor();
		hge->System_Start();
		DoneEditor();
	}
	else MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_TASKMODAL);

	hge->System_Shutdown();
	hge->Release();
	return 0;
}

void InitEditor()
{
	hge->Resource_AttachPack("fonted.paq");

	fnt=new hgeFont("font3.fnt");

	state.bHelp=false;
	state.bBBox=false;
	state.bDrag=false;

	FontList=new CFontList();
	FontList->BuildList();

	state.FontFamily=FontList->GetFontByIdx(0);
	state.nSize=20;
	state.nPadTop=hge->Ini_GetInt("HGE", "PaddingTop",0);
	state.nPadBtm=hge->Ini_GetInt("HGE", "PaddingBottom",0);
	state.nPadLft=hge->Ini_GetInt("HGE", "PaddingLeft",0);
	state.nPadRgt=hge->Ini_GetInt("HGE", "PaddingRight",0);
	state.bBold=false;
	state.bItalic=false;
	state.bAntialias=true;
	state.sr.First=32;
	state.sr.Last=126;

	cmdGenerateFont();

	texGui=hge->Texture_Load("fgui.png");

	sprCursor=new hgeSprite(texGui, 487, 181, 19, 26);
	sprBlack=new hgeSprite(0,0,0,100,100);
	sprBlack->SetColor(0xFF000000);

	sprLeftPane1=new hgeSprite(texGui, 0, 0, 168, 512);
	sprLeftPane2=new hgeSprite(texGui, 336, 0, 168, 88);

	gui=new hgeGUI();
	CreateGUI();
}

void DoneEditor()
{
	delete gui;
	delete sprLeftPane1;
	delete sprLeftPane2;
	delete sprCursor;
	delete fnt;
	delete sprFont;
	delete sprBlack;

	delete FontList;

	hge->Texture_Free(texFont);
	hge->Texture_Free(texGui);

	hge->Resource_RemoveAllPacks();
}

void CreateGUI()
{
	hgeGUIListbox *listbox;
	hgeGUIButton  *button;
	hgeGUISlider  *slider;
	hgeGUIText	  *text;
	hgeGUIRange	  *range;
	int i;

	gui->AddCtrl(new hgeGUIButton(CMD_SAVE, 9, 485, 47, 17, texGui, 336, 338));
	gui->AddCtrl(new hgeGUIButton(CMD_EXIT, 111, 485, 47, 17, texGui, 336, 338));
	button=new hgeGUIButton(CMD_HELP, 60, 485, 47, 17, texGui, 336, 338);
	button->SetMode(true);
	gui->AddCtrl(button);

	button=new hgeGUIButton(CMD_BOLD, 9, 180, 8, 8, texGui, 368, 176);
	button->SetMode(true);
	button->SetState(state.bBold);
	gui->AddCtrl(button);
	button=new hgeGUIButton(CMD_ITALIC, 52, 180, 8, 8, texGui, 368, 176);
	button->SetMode(true);
	button->SetState(state.bItalic);
	gui->AddCtrl(button);
	button=new hgeGUIButton(CMD_ANTIALIAS, 97, 180, 8, 8, texGui, 368, 176);
	button->SetMode(true);
	button->SetState(state.bAntialias);
	gui->AddCtrl(button);	
	
	button=new hgeGUIButton(CMD_BOUNDINGBOX, 9, 461, 8, 8, texGui, 368, 176);
	button->SetMode(true);
	button->SetState(state.bBBox);
	gui->AddCtrl(button);

	listbox=new hgeGUIListbox(CMD_FAMILYLIST, 10, 44, 139, 128, fnt, 0xFF7697A4, 0xFFBBCBD2, 0x40D4C25A);
	for(i=0; i<FontList->GetNumFonts(); i++) listbox->AddItem(FontList->GetFontByIdx(i));
	gui->AddCtrl(listbox);

	slider=new hgeGUISlider(CMD_FAMILYSLIDER, 152, 44, 6, 128, texGui, 417, 177, 6, 6, true);
	slider->SetMode(0, (float)listbox->GetNumItems()-listbox->GetNumRows(), HGESLIDER_BAR);
	slider->SetValue(0);
	gui->AddCtrl(slider);

	range=new hgeGUIRange(CMD_CHARRANGE, 14, 266, 144, 144, 16, 16, 0x4D99FCD2);
	range->SetRange(state.sr.First, state.sr.Last);
	gui->AddCtrl(range);

	slider=new hgeGUISlider(CMD_FONTSIZE, 10, 219, 148, 6, texGui, 417, 177, 6, 6, false);
	slider->SetMode(5, 80, HGESLIDER_BAR);
	slider->SetValue((float)state.nSize);
	gui->AddCtrl(slider);
	text=new hgeGUIText(CMD_TFONTSIZE, 116, 205, 28, 12, fnt);
	text->SetMode(HGETEXT_RIGHT);
	text->printf("%d",state.nSize);
	gui->AddCtrl(text);
}

