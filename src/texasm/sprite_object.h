#pragma once
#ifndef SPRITE_OBJECT_H
#define SPRITE_OBJECT_H


#include "gfx_object.h"
#include <hgesprite.h>


class CSpriteObject : public CGfxObject
{
public:
	CSpriteObject(hgeSprite *_spr, hgeString _name, int _resgroup, bool _owned);
	~CSpriteObject();

	virtual int			GetWidth() const { return spr ? int(spr->GetWidth()) : 0; }
	virtual int			GetHeight() const { return spr ? int(spr->GetHeight()) : 0; }
	virtual HTEXTURE	GetTexture() { return spr ? spr->GetTexture() : 0; }
	virtual void		GetSourcePos(int *_x, int *_y);
	virtual bool		SaveDescription(FILE *fp, hgeConstString texname);

private:
	hgeSprite	*spr;
	bool		owned;
};


#endif