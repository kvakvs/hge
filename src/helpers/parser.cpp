/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Resource script parser implementation
*/

#include "parser.h"


HGE *RScriptParser::hge=0;


struct keyword
{
	char*	word;
	int		code;
};

keyword keytable[]=
{
	{ "=",			TTEQUALS		},
	{ ":",			TTBASED			},
	{ ",",			TTSEPARATOR		},
	{ "{",			TTOPENBLOCK		},
	{ "}",			TTCLOSEBLOCK	},
	{ "true",		TTBOOL			},
	{ "false",		TTBOOL			},

	{ "Include",	TTRES_INCLUDE	},
	{ "Resource",	TTRES_RESOURCE	},
	{ "Texture",	TTRES_TEXTURE	},
	{ "Sound",		TTRES_SOUND		},
	{ "Music",		TTRES_MUSIC		},
	{ "Stream",		TTRES_STREAM	},
	{ "Target",		TTRES_TARGET	},
	{ "Sprite",		TTRES_SPRITE	},
	{ "Animation",	TTRES_ANIMATION	},
	{ "Font",		TTRES_FONT		},
	{ "Particle",	TTRES_PARTICLE	},
	{ "Distortion",	TTRES_DISTORT	},
	{ "StringTable",TTRES_STRTABLE	},

	{ "filename",	TTPAR_FILENAME	},
	{ "resgroup",	TTPAR_RESGROUP	},
	{ "mipmap",		TTPAR_MIPMAP	},
	{ "amplify",	TTPAR_AMPLIFY	},
	{ "size",		TTPAR_SIZE		},
	{ "zbuffer",	TTPAR_ZBUFFER	},
	{ "texture",	TTPAR_TEXTURE	},
	{ "rect",		TTPAR_RECT		},
	{ "hotspot",	TTPAR_HOTSPOT	},
	{ "blendmode",	TTPAR_BLENDMODE	},
	{ "color",		TTPAR_COLOR		},
	{ "zorder",		TTPAR_ZORDER	},
	{ "flip",		TTPAR_FLIP		},
	{ "scale",		TTPAR_SCALE		},
	{ "proportion",	TTPAR_PROPORTION},
	{ "rotation",	TTPAR_ROTATION	},
	{ "frames",		TTPAR_FRAMES	},
	{ "fps",		TTPAR_FPS		},
	{ "mode",		TTPAR_MODE		},
	{ "tracking",	TTPAR_TRACKING	},
	{ "spacing",	TTPAR_SPACING	},
	{ "sprite",		TTPAR_SPRITE	},
	{ "mesh",		TTPAR_MESH		},

	{ "COLORMUL",	TTCON_COLORMUL	},
	{ "COLORADD",	TTCON_COLORADD	},
	{ "ALPHABLEND",	TTCON_ALPHABLND	},
	{ "ALPHAADD",	TTCON_ALPHAADD	},
	{ "ZWRITE",		TTCON_ZWRITE	},
	{ "NOZWRITE",	TTCON_NOZWRITE	},
	{ "FORWARD",	TTCON_FORWARD	},
	{ "REVERSE",	TTCON_REVERSE	},
	{ "PINGPONG",	TTCON_PINGPONG	},
	{ "NOPINGPONG",	TTCON_NOPINGPONG},
	{ "LOOP",		TTCON_LOOP		},
	{ "NOLOOP",		TTCON_NOLOOP	},
	{ "CIRCLE",		TTCON_CIRCLE	},
	{ "RECT",		TTCON_RECT		},
	{ "ALPHA",		TTCON_ALPHA		},

	{ NULL,			TTNONE			}
};

RScriptParser::RScriptParser(char *name, char *scr)
{
	hge=hgeCreate(HGE_VERSION);

	scriptname=name;
	script=scr;
	tokenvalue[0]=0;
	tokentype=TTNONE;
	line=1;
}

int RScriptParser::get_token()
{
	int i;

	// Skip whitespaces and comments

	for(;;)
	{
		while(*script==' ' || *script=='\t' || *script=='\n' || *script=='\r')
		{
			if(*script=='\n') line++;
			script++;
		}
		if(*script==';') while(*script && *script!='\n' && *script!='\r') script++;
		else break;
	}

	// End of script

	if(!*script) { tokentype=TTEND; tokenvalue[0]=0; return tokentype; }

	// Number

	if((*script>='0' && *script<='9') || *script=='.' || *script=='-')
	{
		tokentype=TTNUMBER;
		for(i=0;(*script>='0' && *script<='9') || *script=='.' || *script=='-';i++)
			 tokenvalue[i]=*script++;

		// Hexadecimal number starting with decimal digit

		if((*script>='A' && *script<='F') || (*script>='a' && *script<='f'))
		{
			tokentype=TTSTRING;
			for(; (*script>='A' && *script<='F') || (*script>='a' && *script<='f') ; i++)
				 tokenvalue[i]=*script++;
		}

		tokenvalue[i]=0;
		return tokentype;
	}

	// Quoted string

	if(*script=='"')
	{
		tokentype=TTSTRING;
		script++;
		for(i=0;*script && *script!='"' && *script!='\n' && *script!='\r';i++)
			 tokenvalue[i]=*script++;
		tokenvalue[i]=0;
		if(*script) script++;
		return tokentype;
	}

	// Keyword

	for(i=0;keytable[i].word;i++)
		if(!strtkcmp(keytable[i].word, script))
		{
			tokentype = keytable[i].code;
			strcpy(tokenvalue,keytable[i].word);
			script+=strlen(keytable[i].word);
			return tokentype;
		}

	// Unquoted string or hexadecimal number

	tokentype=TTSTRING;
	for(i=0;
		*script && *script!=' ' && *script!='\t' && *script!='\n' && *script!='\r'
		&& *script!=',' && *script!='=' && *script!='{' && *script!='}' && *script!=':';
		i++)
		tokenvalue[i]=*script++;
	tokenvalue[i]=0;
	return tokentype;
}

bool RScriptParser::strtkcmp(char *str, char *mem)
{
	int i,len=strlen(str);
	for(i=0;i<len;i++)
	{
		if(!mem[i]) return true;
		if(mem[i] != str[i]) return true;
	}
	return false;
}

DWORD RScriptParser::tkn_hex()
{
	int i;
	DWORD dw=0;
	char chr;
	for(i=0; tokenvalue[i]; i++)
	{
		chr=tokenvalue[i];
		if(chr >= 'a') chr-='a'-':';
		if(chr >= 'A') chr-='A'-':';
		chr-='0';
		if(chr>0xF) chr=0xF;
		dw=(dw << 4) | chr;
	}
	return dw;
}

void RScriptParser::ScriptPostError(char *msg1, char *msg2)
{
	hge->System_Log("%s, line %d: %s'%s'%s",
		get_name(), get_line(), msg1, tokenvalue[0] ? tkn_string():"<EOF>", msg2);
}
