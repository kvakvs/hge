/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Resource script parser header
*/

#ifndef HGEPARSER_H
#define HGEPARSER_H


#include "..\..\include\hge.h"
#include <string.h>
#include <stdlib.h>


enum
{
 TTNONE, TTEND, TTNUMBER, TTSTRING,

 TTBOOL, TTEQUALS, TTBASED, TTSEPARATOR, TTOPENBLOCK, TTCLOSEBLOCK,
 
 TTRES__FIRST,
 TTRES_INCLUDE, TTRES_RESOURCE, TTRES_TEXTURE, TTRES_SOUND, TTRES_MUSIC,
 TTRES_STREAM, TTRES_TARGET, TTRES_SPRITE, TTRES_ANIMATION, TTRES_FONT,
 TTRES_PARTICLE, TTRES_DISTORT, TTRES_STRTABLE,
 TTRES__LAST,

 TTPAR__FIRST,
 TTPAR_FILENAME, TTPAR_RESGROUP, TTPAR_MIPMAP, TTPAR_AMPLIFY, TTPAR_SIZE, TTPAR_ZBUFFER,
 TTPAR_TEXTURE, TTPAR_RECT, TTPAR_HOTSPOT, TTPAR_BLENDMODE, TTPAR_COLOR,
 TTPAR_ZORDER, TTPAR_FLIP, TTPAR_SCALE, TTPAR_PROPORTION, TTPAR_ROTATION, TTPAR_FRAMES,
 TTPAR_FPS, TTPAR_MODE, TTPAR_TRACKING, TTPAR_SPACING, TTPAR_SPRITE, TTPAR_MESH,
 TTPAR__LAST,

 TTCON__FIRST,
 TTCON_COLORMUL, TTCON_COLORADD, TTCON_ALPHABLND, TTCON_ALPHAADD, TTCON_ZWRITE,
 TTCON_NOZWRITE, TTCON_FORWARD, TTCON_REVERSE, TTCON_PINGPONG, TTCON_NOPINGPONG,
 TTCON_LOOP, TTCON_NOLOOP, TTCON_CIRCLE, TTCON_RECT, TTCON_ALPHA,
 TTCON__LAST
};


class RScriptParser
{
public:
	RScriptParser(char *name, char *scr);
	~RScriptParser() { hge->Release(); }

	int		get_token();
	void	put_back()	 { script-=strlen(tokenvalue); }
	int		get_line()	 { return line;}
	char*	get_name()	 { return scriptname;}

	char*	tkn_string() { return tokenvalue; }
	int		tkn_int()    { return atoi(tokenvalue); }
	float	tkn_float()  { return (float)atof(tokenvalue); }
	bool	tkn_bool()   { return (tokenvalue[0]=='t' || tokenvalue[0]=='T') ? true : false; }
	DWORD	tkn_hex();

	void	ScriptPostError(char *msg1, char *msg2);

	int		tokentype;
	char	tokenvalue[128];
	char*	script;
	char*	scriptname;
	int		line;

private:
	bool	strtkcmp(char *str, char *mem);

	static HGE *hge;
};


#endif
