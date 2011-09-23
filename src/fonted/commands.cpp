/*
** Haaf's Game Engine 1.6
** Copyright (C) 2003, Relish Games
** hge.relishgames.com
**
** Bitmap Font Builder
*/


#include "fonted.h"
#include <windows.h>
#include <string>

using namespace std;

bool HandleKeys(int key)
{
	switch(key)
	{
		case 0: return false;
		case HGEK_ESCAPE: return true;

		case HGEK_LBUTTON:
			if(state.mx > 168 && state.mx > psx-fw2 && state.mx < psx+fw2 &&
			   state.my > psy-fh2 && state.my < psy+fh2)
			{
				state.bDrag=true;
				state.nDragXOffset=state.mx;
				state.nDragYOffset=state.my;
				state.nDragOldX=psx;
				state.nDragOldY=psy;
			}
	}

	return false;
}

bool DoCommands(int id)
{
	hgeGUIListbox *listbox;
	int f,l;

	switch(id)
	{
		case 0:	return false;
		case CMD_EXIT: return true;
		case CMD_HELP: state.bHelp=ButtonGetState(CMD_HELP); break;
		case CMD_SAVE: cmdSaveFont(); break;

		case CMD_BOLD: state.bBold=ButtonGetState(CMD_BOLD); cmdGenerateFont(); break;
		case CMD_ITALIC: state.bItalic=ButtonGetState(CMD_ITALIC); cmdGenerateFont(); break;
		case CMD_ANTIALIAS: state.bAntialias=ButtonGetState(CMD_ANTIALIAS); cmdGenerateFont(); break;

		case CMD_BOUNDINGBOX: state.bBBox=ButtonGetState(CMD_BOUNDINGBOX); break;

		case CMD_FONTSIZE:
			state.nSize=int(SliderGetValue(CMD_FONTSIZE));
			GetTextCtrl(CMD_TFONTSIZE)->printf("%d",state.nSize);
			cmdGenerateFont();
			break;

		case CMD_FAMILYLIST:
			listbox=hgeGetListboxCtrl(gui, CMD_FAMILYLIST);
			state.FontFamily=listbox->GetItemText(listbox->GetSelectedItem());
			SliderSetValue(CMD_FAMILYSLIDER, listbox->GetTopItem());
			cmdGenerateFont();
			break;

		case CMD_FAMILYSLIDER:
			listbox=hgeGetListboxCtrl(gui, CMD_FAMILYLIST);
			listbox->SetTopItem(int(SliderGetValue(CMD_FAMILYSLIDER)));
			break;

		case CMD_CHARRANGE:
			((hgeGUIRange*)gui->GetCtrl(CMD_CHARRANGE))->GetRange(&f, &l);
			state.sr.First=f;
			state.sr.Last=l;
			cmdGenerateFont();
			break;
	}

	return false;
}

void cmdGenerateFont()
{
	if(sprFont) delete sprFont;
	if(texFont) hge->Texture_Free(texFont);
	texFont=FontGenerate(state.FontFamily, state.nSize, state.nPadTop, state.nPadBtm, state.nPadLft, state.nPadRgt, state.bBold, state.bItalic, state.bAntialias, &state.sr, 1);
	sprFont=new hgeSprite(texFont, 0.0f, 0.0f, (float)hge->Texture_GetWidth(texFont), (float)hge->Texture_GetHeight(texFont));
}

bool cmdSaveFont()
{
	FILE *fp;
	OPENFILENAME ofn;
	char szFile[512]="\0";
	char szTemp[512];

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME); 
	ofn.hwndOwner = hge->System_GetState(HGE_HWND); 
	ofn.lpstrFilter = "HGE Font Files\0*.fnt\0All Files\0*.*\0\0"; 
	ofn.lpstrFile= szFile; 
	ofn.nMaxFile = sizeof(szFile); 
	ofn.Flags = OFN_OVERWRITEPROMPT; // | OFN_NOCHANGEDIR
	ofn.lpstrDefExt="fnt";
	if(!GetSaveFileName(&ofn)) return true;

	fp=fopen(szFile,"w");
	if(!fp)
	{
		sprintf(szTemp, "Can't write to file:\n%s\nProbaby it is write protected.",szFile);
		MessageBox(NULL, szTemp, "Can't write to file", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
		return false;
	}

	strcpy(&szFile[ofn.nFileExtension],"png");
	fprintf(fp,"[HGEFONT]\n\nBitmap=%s\n\n", &szFile[ofn.nFileOffset]);

	for(int i=state.sr.First; i<=state.sr.Last; i++)
	{
		char c=i;
		if(i>=32 && i<=126)
			fprintf(fp,"Char=\"%c\",%d,%d,%d,%d,%d,%d\n",
			  c, vChars[i].x, vChars[i].y, vChars[i].w, vChars[i].h,
			  vChars[i].a, vChars[i].c);
		else
			fprintf(fp,"Char=%2X,%d,%d,%d,%d,%d,%d\n",
			  i, vChars[i].x, vChars[i].y, vChars[i].w, vChars[i].h,
			  vChars[i].a, vChars[i].c);
	}

	fclose(fp);

	return SavePNG(texFont, szFile);
}