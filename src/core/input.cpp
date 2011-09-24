/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: input
*/


#include "hge_impl.h"


hgeString KeyNames[] =
{
 TXT("?"),
 TXT("Left Mouse Button"), TXT("Right Mouse Button"), TXT("?"), TXT("Middle Mouse Button"),
 TXT("?"), TXT("?"), TXT("?"), TXT("Backspace"), TXT("Tab"), TXT("?"), TXT("?"), TXT("?"), TXT("Enter"), TXT("?"), TXT("?"),
 TXT("Shift"), TXT("Ctrl"), TXT("Alt"), TXT("Pause"), TXT("Caps Lock"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("Escape"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("Space"), TXT("Page Up"), TXT("Page Down"), TXT("End"), TXT("Home"),
 TXT("Left Arrow"), TXT("Up Arrow"), TXT("Right Arrow"), TXT("Down Arrow"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("Insert"), TXT("Delete"), TXT("?"),
 TXT("0"), TXT("1"), TXT("2"), TXT("3"), TXT("4"), TXT("5"), TXT("6"), TXT("7"), TXT("8"), TXT("9"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("A"), TXT("B"), TXT("C"), TXT("D"), TXT("E"), TXT("F"), TXT("G"), TXT("H"), TXT("I"), TXT("J"), TXT("K"), TXT("L"), TXT("M"),
 TXT("N"), TXT("O"), TXT("P"), TXT("Q"), TXT("R"), TXT("S"), TXT("T"), TXT("U"), TXT("V"), TXT("W"), TXT("X"), TXT("Y"), TXT("Z"),
 TXT("Left Win"), TXT("Right Win"), TXT("Application"), TXT("?"), TXT("?"),
 TXT("NumPad 0"), TXT("NumPad 1"), TXT("NumPad 2"), TXT("NumPad 3"), TXT("NumPad 4"),
 TXT("NumPad 5"), TXT("NumPad 6"), TXT("NumPad 7"), TXT("NumPad 8"), TXT("NumPad 9"),
 TXT("Multiply"), TXT("Add"), TXT("?"), TXT("Subtract"), TXT("Decimal"), TXT("Divide"),
 TXT("F1"), TXT("F2"), TXT("F3"), TXT("F4"), TXT("F5"), TXT("F6"), TXT("F7"), TXT("F8"), TXT("F9"), TXT("F10"), TXT("F11"), TXT("F12"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("Num Lock"), TXT("Scroll Lock"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("Semicolon"), TXT("Equals"), TXT("Comma"), TXT("Minus"), TXT("Period"), TXT("Slash"), TXT("Grave"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("Left bracket"), TXT("Backslash"), TXT("Right bracket"), TXT("Apostrophe"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
 TXT("?"), TXT("?"), TXT("?")
};


bool HGE_CALL HGE_Impl::Input_GetEvent(hgeInputEvent *event)
{
	CInputEventList *eptr;

	if(queue)
	{
		eptr=queue;
		memcpy(event, &eptr->event, sizeof(hgeInputEvent));
		queue=eptr->next;
		delete eptr;
		return true;
	}
	
	return false;
}

void HGE_CALL HGE_Impl::Input_GetMousePos(float *x, float *y)
{
	*x=Xpos; *y=Ypos;
}


void HGE_CALL HGE_Impl::Input_SetMousePos(float x, float y)
{
	POINT pt;
	pt.x=(long)x; pt.y=(long)y;
	ClientToScreen(hwnd, &pt);
	SetCursorPos(pt.x,pt.y);
}

int HGE_CALL HGE_Impl::Input_GetMouseWheel()
{
	return Zpos;
}

bool HGE_CALL HGE_Impl::Input_IsMouseOver()
{
	return bMouseOver;
}

bool HGE_CALL HGE_Impl::Input_GetKeyState(int key)
{
	return ((GetKeyState(key) & 0x8000) != 0);
}

bool HGE_CALL HGE_Impl::Input_KeyDown(int key)
{
	return (keyz[key] & 1) != 0;
}

bool HGE_CALL HGE_Impl::Input_KeyUp(int key)
{
	return (keyz[key] & 2) != 0;
}

hgeString HGE_CALL HGE_Impl::Input_GetKeyName(int key)
{
	return KeyNames[key];
}

int HGE_CALL HGE_Impl::Input_GetKey()
{
	return VKey;
}

int HGE_CALL HGE_Impl::Input_GetChar()
{
	return Char;
}


//////// Implementation ////////


void HGE_Impl::_InputInit()
{
	POINT	pt;
	GetCursorPos(&pt);
	ScreenToClient(hwnd, &pt);
	Xpos = (float)pt.x;
	Ypos = (float)pt.y;

	memset(&keyz, 0, sizeof(keyz));
}

void HGE_Impl::_UpdateMouse()
{
	POINT	pt;
	RECT	rc;

	GetCursorPos(&pt);
	GetClientRect(hwnd, &rc);
	MapWindowPoints(hwnd, NULL, (LPPOINT)&rc, 2);

	if(bCaptured || (PtInRect(&rc, pt) && WindowFromPoint(pt)==hwnd))
		bMouseOver=true;
	else
		bMouseOver=false;
}

void HGE_Impl::_BuildEvent(int type, int key, int scan, int flags, int x, int y)
{
	CInputEventList *last, *eptr=new CInputEventList;
	unsigned char kbstate[256];
	POINT pt;

	eptr->event.type=type;
	eptr->event.chr=0;
	pt.x=x; pt.y=y;

	GetKeyboardState(kbstate);
	if(type==INPUT_KEYDOWN)
	{
		if((flags & HGEINP_REPEAT) == 0) keyz[key] |= 1;
		ToAscii(key, scan, kbstate, (unsigned short *)&eptr->event.chr, 0);
	}
	if(type==INPUT_KEYUP)
	{
		keyz[key] |= 2;
		ToAscii(key, scan, kbstate, (unsigned short *)&eptr->event.chr, 0);
	}
	if(type==INPUT_MOUSEWHEEL)
	{
		eptr->event.key=0; eptr->event.wheel=key;
		ScreenToClient(hwnd,&pt);
	}
	else { eptr->event.key=key; eptr->event.wheel=0; }

	if(type==INPUT_MBUTTONDOWN)
	{
		keyz[key] |= 1;
		SetCapture(hwnd);
		bCaptured=true;
	}
	if(type==INPUT_MBUTTONUP)
	{
		keyz[key] |= 2;
		ReleaseCapture();
		Input_SetMousePos(Xpos, Ypos);
		pt.x=(int)Xpos; pt.y=(int)Ypos;
		bCaptured=false;
	}
	
	if(kbstate[VK_SHIFT] & 0x80) flags|=HGEINP_SHIFT;
	if(kbstate[VK_CONTROL] & 0x80) flags|=HGEINP_CTRL;
	if(kbstate[VK_MENU] & 0x80) flags|=HGEINP_ALT;
	if(kbstate[VK_CAPITAL] & 0x1) flags|=HGEINP_CAPSLOCK;
	if(kbstate[VK_SCROLL] & 0x1) flags|=HGEINP_SCROLLLOCK;
	if(kbstate[VK_NUMLOCK] & 0x1) flags|=HGEINP_NUMLOCK;
	eptr->event.flags=flags;

	if(pt.x==-1) { eptr->event.x=Xpos;eptr->event.y=Ypos; }
	else
	{
		if(pt.x<0) pt.x=0;
		if(pt.y<0) pt.y=0;
		if(pt.x>=nScreenWidth) pt.x=nScreenWidth-1;
		if(pt.y>=nScreenHeight) pt.y=nScreenHeight-1;

		eptr->event.x=(float)pt.x;
		eptr->event.y=(float)pt.y;
	}

	eptr->next=0; 

	if(!queue) queue=eptr;
	else
	{
		last=queue;
		while(last->next) last=last->next;
		last->next=eptr;
	}

	if(eptr->event.type==INPUT_KEYDOWN || eptr->event.type==INPUT_MBUTTONDOWN)
	{
		VKey=eptr->event.key;Char=eptr->event.chr;
	}
	else if(eptr->event.type==INPUT_MOUSEMOVE)
	{
		Xpos=eptr->event.x;Ypos=eptr->event.y;
	}
	else if(eptr->event.type==INPUT_MOUSEWHEEL)
	{
		Zpos+=eptr->event.wheel;
	}
}

void HGE_Impl::_ClearQueue()
{
	CInputEventList *nexteptr, *eptr=queue;

	memset(&keyz, 0, sizeof(keyz));
	
	while(eptr)
	{
		nexteptr=eptr->next;
		delete eptr;
		eptr=nexteptr;
	}

	queue=0; VKey=0; Char=0; Zpos=0;
}
