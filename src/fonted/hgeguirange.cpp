/*
** Haaf's Game Engine 1.6
** Copyright (C) 2006, Relish Games
** hge.relishgames.com
**
** Bitmap Font Builder
*/


#include "hgeguirange.h"


hgeGUIRange::hgeGUIRange(int _id, float x, float y, float w, float h, int _nrows, int _ncols, DWORD color)
{
	id=_id;
	bStatic=false;
	bVisible=true;
	bEnabled=true;
	rect.Set(x, y, x+w, y+h);

	spr=new hgeSprite(0, 0, 0, 1, 1);
	spr->SetColor(color);
	nrows=_nrows;
	ncols=_ncols;
	nfirst=nlast=0;

	mx=0; my=0;
	bPressed=false;
}

hgeGUIRange::~hgeGUIRange()
{
	if(spr) delete spr;
}

void hgeGUIRange::SetRange(int first, int last)
{
	if(first<0) first=0;
	if(last>=nrows*ncols) last=nrows*ncols-1;
	nfirst=first;
	nlast=last;
}

void hgeGUIRange::GetRange(int *first, int *last)
{
	if(nlast>=nfirst) { *first=nfirst; *last=nlast; }
	else { *first=nlast; *last=nfirst; }
}

void hgeGUIRange::Render()
{
	float cw=(rect.x2-rect.x1)/ncols;
	float ch=(rect.y2-rect.y1)/nrows;
	float xf,yf, xl, yl;
	int first, last;

	if(nlast>=nfirst)
	{
		first=nfirst;
		last=nlast;
	}
	else
	{
		first=nlast;
		last=nfirst;
	}

	xf=rect.x1 + cw*(first%ncols);
	yf=rect.y1 + ch*(first/ncols);

	xl=rect.x1 + cw*(last%ncols);
	yl=rect.y1 + ch*(last/ncols);

	if(yf == yl) spr->RenderStretch(xf, yf, xl+cw, yf+ch);
	else
	{
		spr->RenderStretch(xf, yf, rect.x2, yf+ch);
		spr->RenderStretch(rect.x1, yl, xl+cw, yl+ch);
		if(yl-yf>1) spr->RenderStretch(rect.x1, yf+ch, rect.x2, yl);
	}
	
	//spr->RenderStretch(x1,y1,x2,y2);
}

bool hgeGUIRange::MouseMove(float x, float y)
{
	mx=x; my=y;

	if(bPressed)
	{
		nlast=calc_point(mx, my);
		return true;
	}

	return false;
}

bool hgeGUIRange::MouseLButton(bool bDown)
{
	bPressed=bDown;

	if(bPressed)
	{
		nfirst=calc_point(mx, my);
		nlast=nfirst;
		return true;
	}
	return false;
}

int hgeGUIRange::calc_point(float x, float y)
{
	float cw=rect.x2-rect.x1;
	float ch=rect.y2-rect.y1;
	
	if(x<0) x=0;
	if(x>cw-1) x=cw-1;

	if(y<0) y=0;
	if(y>ch-1) y=ch-1;

	cw/=ncols;
	ch/=nrows;

	int xcell=int(x)/int(cw);
	int ycell=int(y)/int(ch);

	return ycell*ncols + xcell;
}