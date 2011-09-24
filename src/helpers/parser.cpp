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
	const hgeString		word;
	int					code;
};

keyword keytable[]=
{
	{ TXT("="),			TTEQUALS		},
	{ TXT(":"),			TTBASED			},
	{ TXT(","),			TTSEPARATOR		},
	{ TXT("{"),			TTOPENBLOCK		},
	{ TXT("}"),			TTCLOSEBLOCK	},
	{ TXT("true"),		TTBOOL			},
	{ TXT("false"),		TTBOOL			},

	{ TXT("Include"),	TTRES_INCLUDE	},
	{ TXT("Resource"),	TTRES_RESOURCE	},
	{ TXT("Texture"),	TTRES_TEXTURE	},
	{ TXT("Sound"),		TTRES_SOUND		},
	{ TXT("Music"),		TTRES_MUSIC		},
	{ TXT("Stream"),	TTRES_STREAM	},
	{ TXT("Target"),	TTRES_TARGET	},
	{ TXT("Sprite"),	TTRES_SPRITE	},
	{ TXT("Animation"),	TTRES_ANIMATION	},
	{ TXT("Font"),		TTRES_FONT		},
	{ TXT("Particle"),	TTRES_PARTICLE	},
	{ TXT("Distortion"),TTRES_DISTORT	},
	{ TXT("StringTable"),TTRES_STRTABLE	},

	{ TXT("filename"),	TTPAR_FILENAME	},
	{ TXT("resgroup"),	TTPAR_RESGROUP	},
	{ TXT("mipmap"),	TTPAR_MIPMAP	},
	{ TXT("amplify"),	TTPAR_AMPLIFY	},
	{ TXT("size"),		TTPAR_SIZE		},
	{ TXT("zbuffer"),	TTPAR_ZBUFFER	},
	{ TXT("texture"),	TTPAR_TEXTURE	},
	{ TXT("rect"),		TTPAR_RECT		},
	{ TXT("hotspot"),	TTPAR_HOTSPOT	},
	{ TXT("blendmode"),	TTPAR_BLENDMODE	},
	{ TXT("color"),		TTPAR_COLOR		},
	{ TXT("zorder"),	TTPAR_ZORDER	},
	{ TXT("flip"),		TTPAR_FLIP		},
	{ TXT("scale"),		TTPAR_SCALE		},
	{ TXT("proportion"),TTPAR_PROPORTION},
	{ TXT("rotation"),	TTPAR_ROTATION	},
	{ TXT("frames"),	TTPAR_FRAMES	},
	{ TXT("fps"),		TTPAR_FPS		},
	{ TXT("mode"),		TTPAR_MODE		},
	{ TXT("tracking"),	TTPAR_TRACKING	},
	{ TXT("spacing"),	TTPAR_SPACING	},
	{ TXT("sprite"),	TTPAR_SPRITE	},
	{ TXT("mesh"),		TTPAR_MESH		},

	{ TXT("COLORMUL"),	TTCON_COLORMUL	},
	{ TXT("COLORADD"),	TTCON_COLORADD	},
	{ TXT("ALPHABLEND"),TTCON_ALPHABLND	},
	{ TXT("ALPHAADD"),	TTCON_ALPHAADD	},
	{ TXT("ZWRITE"),	TTCON_ZWRITE	},
	{ TXT("NOZWRITE"),	TTCON_NOZWRITE	},
	{ TXT("FORWARD"),	TTCON_FORWARD	},
	{ TXT("REVERSE"),	TTCON_REVERSE	},
	{ TXT("PINGPONG"),	TTCON_PINGPONG	},
	{ TXT("NOPINGPONG"),TTCON_NOPINGPONG},
	{ TXT("LOOP"),		TTCON_LOOP		},
	{ TXT("NOLOOP"),	TTCON_NOLOOP	},
	{ TXT("CIRCLE"),	TTCON_CIRCLE	},
	{ TXT("RECT"),		TTCON_RECT		},
	{ TXT("ALPHA"),		TTCON_ALPHA		},

	{ NULL,			TTNONE			}
};

RScriptParser::RScriptParser(hgeString name, hgeString scr)
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
			hge_strcpy(tokenvalue,keytable[i].word);
			script += hge_strlen(keytable[i].word);
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

bool RScriptParser::strtkcmp(hgeString str, hgeString mem)
{
	int i,len = hge_strlen(str);
	for(i=0;i<len;i++)
	{
		if(!mem[i]) return true;
		if(mem[i] != str[i]) return true;
	}
	return false;
}

uint32_t RScriptParser::tkn_hex()
{
	int i;
	uint32_t dw=0;
	hgeChar chr;
	for(i=0; tokenvalue[i]; i++)
	{
		chr=tokenvalue[i];
		if(chr >= TXT('a')) chr-=TXT('a')-TXT(':');
		if(chr >= TXT('A')) chr-=TXT('A')-TXT(':');
		chr-=TXT('0');
		if(chr>0xF) chr=0xF;
		dw=(dw << 4) | chr;
	}
	return dw;
}

void RScriptParser::ScriptPostError(hgeString msg1, hgeString msg2)
{
	hge->System_Log( TXT("%s, line %d: %s'%s'%s"),
		get_name(),
		get_line(),
		msg1,
		tokenvalue[0] ? tkn_string() : TXT("<EOF>"),
		msg2);
}
