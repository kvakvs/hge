/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeResourceManager helper class header
*/


#ifndef HGERESOURCE_H
#define HGERESOURCE_H


#include "hge.h"
#include "hgesprite.h"
#include "hgeanim.h"
#include "hgefont.h"
#include "hgeparticle.h"
#include "hgedistort.h"
#include "hgestrings.h"


#define RESTYPES 13
#define MAXRESCHARS 128


class hgeResourceManager;

struct ResDesc
{
	char		name[MAXRESCHARS];
	int			resgroup;
	DWORD		handle;
	ResDesc*	next;

	ResDesc()	{ hge=hgeCreate(HGE_VERSION); }
	~ResDesc()	{ hge->Release(); }

	virtual DWORD Get(hgeResourceManager *rm) = 0;
	virtual void  Free() = 0;

protected:
	static HGE	*hge;
};

/*
** HGE Resource manager class
*/
class hgeResourceManager
{
public:
	hgeResourceManager(const char *scriptname=0);
	~hgeResourceManager();

	void				ChangeScript(const char *scriptname=0);
	bool				Precache(int groupid=0);
	void				Purge(int groupid=0);

	void*				GetResource(const char *name, int resgroup=0);
	HTEXTURE			GetTexture(const char *name, int resgroup=0);
	HEFFECT				GetEffect(const char *name, int resgroup=0);
	HMUSIC				GetMusic(const char *name, int resgroup=0);
	HSTREAM				GetStream(const char *name, int resgroup=0);
	HTARGET				GetTarget(const char *name);

	hgeSprite*			GetSprite(const char *name);
	hgeAnimation*		GetAnimation(const char *name);
	hgeFont*			GetFont(const char *name);
	hgeParticleSystem*	GetParticleSystem(const char *name);
	hgeDistortionMesh*	GetDistortionMesh(const char *name);
	hgeStringTable*		GetStringTable(const char *name, int resgroup=0);

	ResDesc*			res[RESTYPES];

private:
	hgeResourceManager(const hgeResourceManager &);
	hgeResourceManager&	operator= (const hgeResourceManager&);
	void				_remove_all();
	void				_parse_script(const char *scriptname=0);

	static HGE			*hge;
};


#endif
