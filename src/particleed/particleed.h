/*
** Haaf's Game Engine 1.8
** Copyright (C) 2007, Relish Games
** hge.relishgames.com
**
** Particle systems editor
*/


#ifndef PARTICLEED_H
#define PARTICLEED_H


#include "..\..\include\hge.h"
#include "..\..\include\hgeanim.h"
#include "..\..\include\hgefont.h"
#include "..\..\include\hgeparticle.h"
#include "..\..\include\hgegui.h"
#include "..\..\include\hgeguictrls.h"


#define PARTICLE_SIZE			32

#define ButtonGetState(id)		((hgeGUIButton*)gui->GetCtrl(id))->GetState()
#define ButtonSetState(id,b)	((hgeGUIButton*)gui->GetCtrl(id))->SetState(b)
#define SliderGetValue(id)		((hgeGUISlider*)gui->GetCtrl(id))->GetValue()
#define SliderSetValue(id,f)	((hgeGUISlider*)gui->GetCtrl(id))->SetValue(f)
#define GetTextCtrl(id)			((hgeGUIText*)gui->GetCtrl(id))


extern HGE *hge;

struct PEditorState
{
	bool				bHelp;
	bool				bBBox;
	bool				bIFace;
	int					nPreset;
	float				mx, my;
	HTEXTURE			texBG;
	hgeSprite			*sprBG;
	hgeParticleSystem	*ps;
};

extern hgeFont		*fnt;
extern hgeGUI		*gui;
extern HTEXTURE		texGui, texParticle;

extern hgeSprite	*sprLeftPane1, *sprLeftPane2, *sprRightPane1, *sprRightPane2;
extern hgeSprite	*sprCursor, *sprColor, *sprBBox;
extern hgeAnimation	*sprParticles;

extern PEditorState state;

bool HandleKeys(int key);
bool DoCommands(int id);

void cmdChangePreset(int n);
void cmdSavePreset(int n);
void cmdLoadPreset(int n);
void cmdChangeColor(int id);


#define CMD_EXIT				1
#define CMD_HELP				2
#define CMD_PRESET1				3
#define CMD_PRESET2				4
#define CMD_PRESET3				5
#define CMD_PRESET4				6
#define CMD_PRESET5				7
#define CMD_PRESET6				8
#define CMD_PRESET7				9
#define CMD_PRESET8				10
#define CMD_PRESET9				11
#define CMD_BOUNDINGBOX			12
#define CMD_NPARTICLES			13
#define CMD_FPS					14

#define CMD_SYS_LIFETIME		15
#define CMD_SYS_TLIFETIME		16
#define CMD_SYS_LIFECONT		17
#define CMD_SYS_EMISSION		18
#define CMD_SYS_TEMISSION		19
#define CMD_SYS_PARLIFETIMEMIN	20
#define CMD_SYS_PARLIFETIMEMAX	21
#define CMD_SYS_PARLIFETIMELOCK	22
#define CMD_SYS_BLENDADDITIVE	23
#define CMD_SYS_BLENDBLEND		24
#define CMD_SYS_TEXTURE			25

#define CMD_PM_DIRECTION		26
#define CMD_PM_TDIRECTION		27
#define CMD_PM_RELATIVE			28
#define CMD_PM_SPREAD			29
#define CMD_PM_TSPREAD			30

#define CMD_PM_STARTSPEEDMIN	31
#define CMD_PM_STARTSPEEDMAX	32
#define CMD_PM_STARTSPEEDLOCK	33

#define CMD_PM_GRAVITYMIN		34
#define CMD_PM_GRAVITYMAX		35
#define CMD_PM_GRAVITYLOCK		36

#define CMD_PM_RADIALMIN		37
#define CMD_PM_RADIALMAX		38
#define CMD_PM_RADIALLOCK		39

#define CMD_PM_TANGENTIALMIN	40
#define CMD_PM_TANGENTIALMAX	41
#define CMD_PM_TANGENTIALLOCK	42

#define CMD_PA_SIZESTART		43
#define CMD_PA_SIZEEND			44
#define CMD_PA_SIZEVAR			45
#define CMD_PA_SIZELOCK			46

#define CMD_PA_SPINSTART		47
#define CMD_PA_SPINEND			48
#define CMD_PA_SPINVAR			49
#define CMD_PA_SPINLOCK			50

#define CMD_PA_ALPHASTART		51
#define CMD_PA_ALPHAEND			52
#define CMD_PA_ALPHAVAR			53
#define CMD_PA_ALPHALOCK		54

#define CMD_PA_RSTART			55
#define CMD_PA_GSTART			56
#define CMD_PA_BSTART			57
#define CMD_PA_REND				58
#define CMD_PA_GEND				59
#define CMD_PA_BEND				60
#define CMD_PA_RGBVAR			61


#endif
