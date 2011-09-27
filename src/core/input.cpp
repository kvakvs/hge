/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: input
*/


#include "hge_impl.h"


hgeString KeyNames[] =
{ TXT("?"), TXT("Left Mouse Button"), TXT("Right Mouse Button"), TXT("?"),
		TXT("Middle Mouse Button"), TXT("?"), TXT("?"), TXT("?"),
		TXT("Backspace"), TXT("Tab"), TXT("?"), TXT("?"), TXT("?"),
		TXT("Enter"), TXT("?"), TXT("?"), TXT("Shift"), TXT("Ctrl"),
		TXT("Alt"), TXT("Pause"), TXT("Caps Lock"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("Escape"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("Space"), TXT("Page Up"),
		TXT("Page Down"), TXT("End"), TXT("Home"), TXT("Left Arrow"),
		TXT("Up Arrow"), TXT("Right Arrow"), TXT("Down Arrow"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("Insert"), TXT("Delete"),
		TXT("?"), TXT("0"), TXT("1"), TXT("2"), TXT("3"), TXT("4"),
		TXT("5"), TXT("6"), TXT("7"), TXT("8"), TXT("9"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("A"), TXT("B"), TXT("C"), TXT("D"), TXT("E"), TXT("F"),
		TXT("G"), TXT("H"), TXT("I"), TXT("J"), TXT("K"), TXT("L"),
		TXT("M"), TXT("N"), TXT("O"), TXT("P"), TXT("Q"), TXT("R"),
		TXT("S"), TXT("T"), TXT("U"), TXT("V"), TXT("W"), TXT("X"),
		TXT("Y"), TXT("Z"), TXT("Left Win"), TXT("Right Win"),
		TXT("Application"), TXT("?"), TXT("?"), TXT("NumPad 0"),
		TXT("NumPad 1"), TXT("NumPad 2"), TXT("NumPad 3"), TXT("NumPad 4"),
		TXT("NumPad 5"), TXT("NumPad 6"), TXT("NumPad 7"), TXT("NumPad 8"),
		TXT("NumPad 9"), TXT("Multiply"), TXT("Add"), TXT("?"),
		TXT("Subtract"), TXT("Decimal"), TXT("Divide"), TXT("F1"),
		TXT("F2"), TXT("F3"), TXT("F4"), TXT("F5"), TXT("F6"), TXT("F7"),
		TXT("F8"), TXT("F9"), TXT("F10"), TXT("F11"), TXT("F12"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("Num Lock"), TXT("Scroll Lock"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("Semicolon"), TXT("Equals"), TXT("Comma"),
		TXT("Minus"), TXT("Period"), TXT("Slash"), TXT("Grave"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("Left bracket"), TXT("Backslash"),
		TXT("Right bracket"), TXT("Apostrophe"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"), TXT("?"),
		TXT("?") };



bool HGE_CALL HGE_Impl::Input_GetEvent(hgeInputEvent *event)
{
	CInputEventList *eptr;

	if (m_input_queue)
	{
		eptr = m_input_queue;
		memcpy(event, &eptr->event, sizeof(hgeInputEvent));
		m_input_queue = eptr->next;
		delete eptr;
		return true;
	}

	return false;
}



void HGE_CALL HGE_Impl::Input_GetMousePos(float *x, float *y)
{
	*x = m_input_xpos;
	*y = m_input_ypos;
}



void HGE_CALL HGE_Impl::Input_SetMousePos(float x, float y)
{
	POINT pt;
	pt.x = (long) x;
	pt.y = (long) y;
	ClientToScreen(m_hwnd, &pt);
	SetCursorPos(pt.x, pt.y);
}



int HGE_CALL HGE_Impl::Input_GetMouseWheel()
{
	return m_input_zpos;
}



bool HGE_CALL HGE_Impl::Input_IsMouseOver()
{
	return m_input_mouseover_flag;
}



bool HGE_CALL HGE_Impl::Input_GetKeyState(int key)
{
	return ((GetKeyState(key) & 0x8000) != 0);
}



bool HGE_CALL HGE_Impl::Input_KeyDown(int key)
{
	return (m_input_keys_table[key] & 1) != 0;
}



bool HGE_CALL HGE_Impl::Input_KeyUp(int key)
{
	return (m_input_keys_table[key] & 2) != 0;
}



hgeString HGE_CALL HGE_Impl::Input_GetKeyName(int key)
{
	return KeyNames[key];
}



int HGE_CALL HGE_Impl::Input_GetKey()
{
	return m_input_vkey;
}



int HGE_CALL HGE_Impl::Input_GetChar()
{
	return m_input_char;
}


//////// Implementation ////////


void HGE_Impl::_InputInit()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hwnd, &pt);
	m_input_xpos = (float) pt.x;
	m_input_ypos = (float) pt.y;

	memset(&m_input_keys_table, 0, sizeof(m_input_keys_table));
}



void HGE_Impl::_UpdateMouse()
{
	POINT pt;
	RECT rc;

	GetCursorPos(&pt);
	GetClientRect(m_hwnd, &rc);
	MapWindowPoints(m_hwnd, NULL, (LPPOINT) &rc, 2);

	if (m_input_captured_flag || (PtInRect(&rc, pt) && WindowFromPoint(pt) == m_hwnd))
		m_input_mouseover_flag = true;
	else
		m_input_mouseover_flag = false;
}



void HGE_Impl::_BuildEvent(int type, int key, int scan, int flags, int x, int y)
{
	CInputEventList *last, *eptr = new CInputEventList;
	unsigned char kbstate[256];
	POINT pt;

	eptr->event.type = type;
	eptr->event.chr = 0;
	pt.x = x;
	pt.y = y;

	GetKeyboardState(kbstate);
	if (type == INPUT_KEYDOWN)
	{
		if ((flags & HGEINP_REPEAT) == 0)
			m_input_keys_table[key] |= 1;
		ToAscii(key, scan, kbstate, (unsigned short *) &eptr->event.chr, 0);
	}
	if (type == INPUT_KEYUP)
	{
		m_input_keys_table[key] |= 2;
		ToAscii(key, scan, kbstate, (unsigned short *) &eptr->event.chr, 0);
	}
	if (type == INPUT_MOUSEWHEEL)
	{
		eptr->event.key = 0;
		eptr->event.wheel = key;
		ScreenToClient(m_hwnd, &pt);
	}
	else
	{
		eptr->event.key = key;
		eptr->event.wheel = 0;
	}

	if (type == INPUT_MBUTTONDOWN)
	{
		m_input_keys_table[key] |= 1;
		SetCapture(m_hwnd);
		m_input_captured_flag = true;
	}
	if (type == INPUT_MBUTTONUP)
	{
		m_input_keys_table[key] |= 2;
		ReleaseCapture();
		Input_SetMousePos(m_input_xpos, m_input_ypos);
		pt.x = (int) m_input_xpos;
		pt.y = (int) m_input_ypos;
		m_input_captured_flag = false;
	}

	if (kbstate[VK_SHIFT] & 0x80) 		flags |= HGEINP_SHIFT;
	if (kbstate[VK_CONTROL] & 0x80)		flags |= HGEINP_CTRL;
	if (kbstate[VK_MENU] & 0x80)		flags |= HGEINP_ALT;
	if (kbstate[VK_CAPITAL] & 0x1)		flags |= HGEINP_CAPSLOCK;
	if (kbstate[VK_SCROLL] & 0x1)		flags |= HGEINP_SCROLLLOCK;
	if (kbstate[VK_NUMLOCK] & 0x1)		flags |= HGEINP_NUMLOCK;
	eptr->event.flags = flags;

	if (pt.x == -1)
	{
		eptr->event.x = m_input_xpos;
		eptr->event.y = m_input_ypos;
	}
	else
	{
		if (pt.x < 0)				pt.x = 0;
		if (pt.y < 0)				pt.y = 0;
		if (pt.x >= m_screen_width)	pt.x = m_screen_width - 1;
		if (pt.y >= m_screen_height)	pt.y = m_screen_height - 1;

		eptr->event.x = (float) pt.x;
		eptr->event.y = (float) pt.y;
	}

	eptr->next = 0;

	if (!m_input_queue)
		m_input_queue = eptr;
	else
	{
		last = m_input_queue;
		while (last->next)
			last = last->next;
		last->next = eptr;
	}

	if (eptr->event.type == INPUT_KEYDOWN || eptr->event.type
			== INPUT_MBUTTONDOWN)
	{
		m_input_vkey = eptr->event.key;
		m_input_char = eptr->event.chr;
	}
	else if (eptr->event.type == INPUT_MOUSEMOVE)
	{
		m_input_xpos = eptr->event.x;
		m_input_ypos = eptr->event.y;
	}
	else if (eptr->event.type == INPUT_MOUSEWHEEL)
	{
		m_input_zpos += eptr->event.wheel;
	}
}



void HGE_Impl::_ClearQueue()
{
	CInputEventList *nexteptr, *eptr = m_input_queue;

	memset(&m_input_keys_table, 0, sizeof(m_input_keys_table));

	while (eptr)
	{
		nexteptr = eptr->next;
		delete eptr;
		eptr = nexteptr;
	}

	m_input_queue = 0;
	m_input_vkey = 0;
	m_input_char = 0;
	m_input_zpos = 0;
}
