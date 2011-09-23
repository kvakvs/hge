/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeAnimation helper class implementation
*/


#include "..\..\include\hgeanim.h"


hgeAnimation::hgeAnimation(HTEXTURE tex, int nframes, float FPS, float x, float y, float w, float h)
	: hgeSprite(tex, x, y, w, h)
{
	orig_width = hge->Texture_GetWidth(tex, true);

	fSinceLastFrame=-1.0f;
	fSpeed=1.0f/FPS;
	bPlaying=false;
	nFrames=nframes;

	Mode=HGEANIM_FWD | HGEANIM_LOOP;
	nDelta=1;
	SetFrame(0);
}

hgeAnimation::hgeAnimation(const hgeAnimation & anim)
: hgeSprite(anim)
{ 
	// Copy hgeAnimation parameters: 
	this->orig_width	  = anim.orig_width;
	this->bPlaying        = anim.bPlaying; 
	this->fSpeed          = anim.fSpeed; 
	this->fSinceLastFrame = anim.fSinceLastFrame;
	this->Mode            = anim.Mode;
	this->nDelta          = anim.nDelta;
	this->nFrames         = anim.nFrames;
	this->nCurFrame       = anim.nCurFrame;
}

void hgeAnimation::SetMode(int mode)
{
	Mode=mode;

	if(mode & HGEANIM_REV)
	{
		nDelta = -1;
		SetFrame(nFrames-1);
	}
	else
	{
		nDelta = 1;
		SetFrame(0);
	}
}


void hgeAnimation::Play()
{
	bPlaying=true;
	fSinceLastFrame=-1.0f;
	if(Mode & HGEANIM_REV)
	{
		nDelta = -1;
		SetFrame(nFrames-1);
	}
	else
	{
		nDelta = 1;
		SetFrame(0);
	}
}


void hgeAnimation::Update(float fDeltaTime)
{
	if(!bPlaying) return;

	if(fSinceLastFrame == -1.0f)
		fSinceLastFrame=0.0f;
	else
		fSinceLastFrame += fDeltaTime;

	while(fSinceLastFrame >= fSpeed)
	{
		fSinceLastFrame -= fSpeed;

		if(nCurFrame + nDelta == nFrames)
		{
			switch(Mode)
			{
				case HGEANIM_FWD:
				case HGEANIM_REV | HGEANIM_PINGPONG:
					bPlaying = false;
					break;

				case HGEANIM_FWD | HGEANIM_PINGPONG:
				case HGEANIM_FWD | HGEANIM_PINGPONG | HGEANIM_LOOP:
				case HGEANIM_REV | HGEANIM_PINGPONG | HGEANIM_LOOP:
					nDelta = -nDelta;
					break;
			}
		}
		else if(nCurFrame + nDelta < 0)
		{
			switch(Mode)
			{
				case HGEANIM_REV:
				case HGEANIM_FWD | HGEANIM_PINGPONG:
					bPlaying = false;
					break;

				case HGEANIM_REV | HGEANIM_PINGPONG:
				case HGEANIM_REV | HGEANIM_PINGPONG | HGEANIM_LOOP:
				case HGEANIM_FWD | HGEANIM_PINGPONG | HGEANIM_LOOP:
					nDelta = -nDelta;
					break;
			}
		}

		if(bPlaying) SetFrame(nCurFrame+nDelta);
	}
}

void hgeAnimation::SetFrame(int n)
{
	float tx1, ty1, tx2, ty2;
	bool bX, bY, bHS;
	int ncols = int(orig_width) / int(width);


	n = n % nFrames;
	if(n < 0) n = nFrames + n;
	nCurFrame = n;

	// calculate texture coords for frame n
	ty1 = ty;
	tx1 = tx + n*width;

	if(tx1 > orig_width-width)
	{
		n -= int(orig_width-tx) / int(width);
		tx1 = width * (n%ncols);
		ty1 += height * (1 + n/ncols);
	}

	tx2 = tx1 + width;
	ty2 = ty1 + height;

	tx1 /= tex_width;
	ty1 /= tex_height;
	tx2 /= tex_width;
	ty2 /= tex_height;

	quad.v[0].tx=tx1; quad.v[0].ty=ty1;
	quad.v[1].tx=tx2; quad.v[1].ty=ty1;
	quad.v[2].tx=tx2; quad.v[2].ty=ty2;
	quad.v[3].tx=tx1; quad.v[3].ty=ty2;

	bX=bXFlip; bY=bYFlip; bHS=bHSFlip;
	bXFlip=false; bYFlip=false;
	SetFlip(bX,bY,bHS);
}

