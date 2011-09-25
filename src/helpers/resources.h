/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeResourceManager resources header
*/
#pragma once
#ifndef HGERESOURCES_H
#define HGERESOURCES_H


#include <hgeresource.h>
#include "parser.h"


#define RES_SCRIPT		0

#define RES_RESOURCE	1
#define RES_TEXTURE		2
#define RES_EFFECT		3
#define RES_MUSIC		4
#define RES_STREAM		5
#define RES_TARGET		6
#define RES_SPRITE		7
#define RES_ANIMATION	8
#define RES_FONT		9
#define RES_PARTICLE	10
#define RES_DISTORT		11
#define RES_STRTABLE	12


void		AddRes(hgeResourceManager *rm, int type, ResDesc *resource);
ResDesc*	FindRes(hgeResourceManager *rm, int type, hgeConstString name);


struct RScript : public ResDesc
{
	static  void  Parse(hgeResourceManager *rm, RScriptParser *sp, hgeConstString name, hgeConstString basename);
	virtual uint32_t Get(hgeResourceManager *rm) {return 0;}
	virtual void  Free() {}
};

struct RResource : public ResDesc
{
	hgeChar filename[HGE_MAX_RESOURCE_CHARS];

	static  void  Parse(hgeResourceManager *rm, RScriptParser *sp, hgeConstString name, hgeConstString basename);
	virtual uint32_t Get(hgeResourceManager *rm);
	virtual void  Free();
};

struct RTexture : public ResDesc
{
	hgeChar		  filename[HGE_MAX_RESOURCE_CHARS];
	bool		  mipmap;

	static  void  Parse(hgeResourceManager *rm, RScriptParser *sp, hgeConstString name, hgeConstString basename);
	virtual uint32_t Get(hgeResourceManager *rm);
	virtual void  Free();
};

struct REffect : public ResDesc
{
	hgeChar		  filename[HGE_MAX_RESOURCE_CHARS];

	static  void  Parse(hgeResourceManager *rm, RScriptParser *sp, hgeConstString name, hgeConstString basename);
	virtual uint32_t Get(hgeResourceManager *rm);
	virtual void  Free();
};

struct RMusic : public ResDesc
{
	hgeChar		  filename[HGE_MAX_RESOURCE_CHARS];
	int			  amplify;

	static  void  Parse(hgeResourceManager *rm, RScriptParser *sp, hgeConstString name, hgeConstString basename);
	virtual uint32_t Get(hgeResourceManager *rm);
	virtual void  Free();
};

struct RStream : public ResDesc
{
	hgeChar		  filename[HGE_MAX_RESOURCE_CHARS];

	static  void  Parse(hgeResourceManager *rm, RScriptParser *sp, hgeConstString name, hgeConstString basename);
	virtual uint32_t Get(hgeResourceManager *rm);
	virtual void  Free();
};

struct RTarget : public ResDesc
{
	int			width;
	int			height;
	bool		zbuffer;

	static  void  Parse(hgeResourceManager *rm, RScriptParser *sp, hgeConstString name, hgeConstString basename);
	virtual uint32_t Get(hgeResourceManager *rm);
	virtual void  Free();
};

struct RSprite : public ResDesc
{
	hgeChar		texname[HGE_MAX_RESOURCE_CHARS];
	float		tx, ty, w, h;
	float		hotx, hoty;
	int			blend;
	uint32_t	color;
	float		z;
	bool		bXFlip, bYFlip;
//	float		x,y;
//	float		scale;
//	float		rotation;
//	int			collision;

	static  void  Parse(hgeResourceManager *rm, RScriptParser *sp, hgeConstString name, hgeConstString basename);
	virtual uint32_t Get(hgeResourceManager *rm);
	virtual void  Free();
};

struct RAnimation : public RSprite
{
	int			frames;
	float		fps;
	int			mode;

	static  void  Parse(hgeResourceManager *rm, RScriptParser *sp, hgeConstString name, hgeConstString basename);
	virtual uint32_t Get(hgeResourceManager *rm);
	virtual void  Free();
};

struct RFont : public ResDesc
{
	hgeChar		filename[HGE_MAX_RESOURCE_CHARS];
	bool		mipmap;
	int			blend;
	uint32_t	color;
	float		z;
	float		scale;
	float		proportion;
	float		tracking;
	float		spacing;
	float		rotation;

	static  void  Parse(hgeResourceManager *rm, RScriptParser *sp, hgeConstString name, hgeConstString basename);
	virtual uint32_t Get(hgeResourceManager *rm);
	virtual void  Free();
};

struct RParticle : public ResDesc
{
	hgeChar		filename[HGE_MAX_RESOURCE_CHARS];
	hgeChar		spritename[HGE_MAX_RESOURCE_CHARS];

	static  void  Parse(hgeResourceManager *rm, RScriptParser *sp, hgeConstString name, hgeConstString basename);
	virtual uint32_t Get(hgeResourceManager *rm);
	virtual void  Free();
};

struct RDistort : public ResDesc
{
	hgeChar		texname[HGE_MAX_RESOURCE_CHARS];
	float		tx, ty, w, h;
	int			cols, rows;
	int			blend;
	uint32_t	color;
	float		z;

	static  void  Parse(hgeResourceManager *rm, RScriptParser *sp, hgeConstString name, hgeConstString basename);
	virtual uint32_t Get(hgeResourceManager *rm);
	virtual void  Free();
};


struct RStringTable : public ResDesc
{
	hgeChar		  filename[HGE_MAX_RESOURCE_CHARS];

	static  void  Parse(hgeResourceManager *rm, RScriptParser *sp, hgeConstString name, hgeConstString basename);
	virtual uint32_t Get(hgeResourceManager *rm);
	virtual void  Free();
};

#endif
