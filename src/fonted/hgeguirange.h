/*
** Haaf's Game Engine 1.6
** Copyright (C) 2006, Relish Games
** hge.relishgames.com
**
** Bitmap Font Builder
*/


#ifndef HGEGUIRANGE_H
#define HGEGUIRANGE_H

#include "..\..\include\hgegui.h"
#include "..\..\include\hgesprite.h"


/*
** hgeGUIRange
*/
class hgeGUIRange : public hgeGUIObject
{
public:
	hgeGUIRange(int id, float x, float y, float w, float h, int nrows, int ncols, DWORD color);
	~hgeGUIRange();

	void			SetRange(int first, int last);
	void			GetRange(int *first, int *last);

	virtual void	Render();
	virtual bool	MouseMove(float x, float y);
	virtual bool	MouseLButton(bool bDown);

private:
	int				calc_point(float x, float y);

	hgeSprite*		spr;
	int				nrows, ncols;
	int				nfirst, nlast;
	float			mx, my;
	bool			bPressed;
};


#endif HGEGUIRANGE_H