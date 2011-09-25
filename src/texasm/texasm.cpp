/*
** Haaf's Game Engine 1.81
** Copyright (C) 2003-2008, Relish Games
** hge.relishgames.com
**
** Texture Assembler
*/

#include "texasm.h"
#include "texture_assembler.h"

#include <hgeresource.h>

#include <direct.h>


HGE *hge = 0;


void SysLog( hgeConstString format, ...);
bool ParseTask( hgeString task, int *resgroup, hgeString mask, bool *inclusive);


int main(int argc, char * argv[])
{
	hgeResourceManager *rm;
	CTextureAssembler  *ta;
	hgeChar	   cwd[MAX_PATH];


	hge=hgeCreate(HGE_VERSION);
	hge->System_SetState(HGE_SHOWSPLASH, false);
	hge->System_SetState(HGE_USESOUND, false);
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_SCREENWIDTH, 320);
	hge->System_SetState(HGE_SCREENHEIGHT, 200);
	hge->System_SetState(HGE_LOGFILE, TXT("texasm.log"));

	// try to initiate HGE

	if(!hge->System_Initiate())
	{
		SysLog( TXT("Can't initiate HGE.\n") );
		hge->System_Shutdown();
		hge->Release();
		return 1;
	}

	SysLog( TXT("\nTexture Assembler v0.1\nCopyright (C) 2003-2008, Relish Games\n\n") );
	
	// check for args count and display help

	if(argc != 3 && argc != 4)
	{
		SysLog(TXT("Usage: TEXASM.EXE <res-file>|<wildcard> <output-wildcard> [<task>]\n\n\n"));

		SysLog(TXT("<res-file> - resource script to process.\n"));
		SysLog(TXT("To be able to locate the resources, texasm.exe must\n"));
		SysLog(TXT("be run from the folder of the main project executable.\n\n"));

		SysLog(TXT("<wildcard> - a wildcard to search image files for processing.\n"));
		SysLog(TXT("Either <res-file> or <wildcard> should be specified, not both.\n"));
		SysLog(TXT("All image formats supported by HGE can be handled.\n\n"));

		SysLog(TXT("<output-wildcard> - wildcard to store the result to.\n"));
		SysLog(TXT("For example, \"opt_textures\\TitleTex\" will generate the following files:\n"));
		SysLog(TXT("  opt_textures\\TitleTex1.png, opt_textures\\TitleTex1.res\n"));
		SysLog(TXT("  opt_textures\\TitleTex2.png, opt_textures\\TitleTex2.res\n"));
		SysLog(TXT("  ...\n\n"));

		SysLog(TXT("<task> - optional resource filter.\n"));
		SysLog(TXT("If omitted, all resources found in the script/folder will be processed.\n"));
		SysLog(TXT("The default resgroup assigned to folder resources is 0.\n"));
		SysLog(TXT("Example tasks:\n\n"));
		SysLog(TXT("  2\n"));
		SysLog(TXT("  Process all resources from resgroup 2\n\n"));
		SysLog(TXT("  7+sprFlower/sprLeaf/sprTrunk\n"));
		SysLog(TXT("  Process resources from resgroup 7 whose\n  names start with sprFlower, sprLeaf or sprTrunk\n\n"));
		SysLog(TXT("  3-gui\\menu\\title\n"));
		SysLog(TXT("  Process resources from resgroup 3 whose\n  names DON'T start with gui, menu or title\n\n"));
		SysLog(TXT("  +mars/earth/mercury\n"));
		SysLog(TXT("  Process all resources whose names start\n  with mars, earth or mercury\n\n"));
		SysLog(TXT("  -gui\\menu\\title\n"));
		SysLog(TXT("  Process all resources whose names DON'T\n  start with gui, menu or title\n\n"));

		SysLog(TXT("\nCurrently only Sprite resources and only 32bit PNG output are supported.\n\n"));

		hge->System_Shutdown();
		hge->Release();
		return 0;
	}

	// test if we should process a resource script or a folder

	bool bScript;
	hgeChar p[256];

	//hge_sprintf(p, sizeof p, TXT("%S"), argv[1]);
	hgeStrFromUtf8( argv[1], p, sizeof p );
	void *script = hge->Resource_Load(p);
	if(script)
		bScript = true;
	else
		bScript = false;

	hge->Resource_Free(script);

	// parse task argument if present

	int resgroup = 0;
	bool inclusive = true;
	hgeChar mask[256] = TXT("");

	if(argc == 4)
	{
		//hge_sprintf(p, sizeof p, TXT("%S"), argv[3]);
		hgeStrFromUtf8( argv[3], p, sizeof p );
		if(!ParseTask(p, &resgroup, mask, &inclusive))
		{
			SysLog(TXT("Invalid task: %s\n"), p);
			hge->System_Shutdown();
			hge->Release();
			return 1;		
		}
	}


	// create resource manager & texture assember

	rm = new hgeResourceManager();
	ta = new CTextureAssembler();

	// (possible enhancement: pass maximum texture size and margin from command line)

	ta->SetMaxTextureSize(1024, 1024);
	ta->SetMargin(1);

	// make the search/output path consistent with texture
	// loading, i.e. relative to the main executable

	hge_getcwd(cwd, sizeof(cwd));
	hge_chdir(hge->Resource_MakePath());

	// Add a task into texture assembler. There may be more than one
	// call to AccumulateRMResources() and/or AccumulateFileResources().

	if(bScript) {
		//hge_sprintf(p, sizeof p, TXT("%S"), argv[1]);
		hgeStrFromUtf8( argv[1], p, sizeof p );
		rm->ChangeScript(p);
		ta->AccumulateRMResources(rm, resgroup, mask, inclusive);
	} else {
		//hge_sprintf(p, sizeof p, TXT("%S"), argv[1]);
		hgeStrFromUtf8( argv[1], p, sizeof p );
		ta->AccumulateFileResources(p, resgroup, mask, inclusive);
	}

	// Generate and save PNG and RES files. The processing includes:
	// - pack sprites as tight as possible onto minimal number of textures
	// - ensure no distortion when scaling or rotating sprites:
	//   a) all sprites have empty 1-pixel border outside
	//   b) texture data is corrected along the alpha channel border
	// - all extra info is removed from the saved PNGs

	//hge_sprintf(p, sizeof p, TXT("%S"), argv[2]);
	hgeStrFromUtf8( argv[2], p, sizeof p );
	bool success = ta->GenerateTextures(p);

	// restore current working directory

	hge_chdir(cwd);

	delete ta;
	delete rm;

	if(success)
		SysLog(TXT("\nSuccess.\n"));
	else
		SysLog(TXT("\nAborted.\n"));

	hge->System_Shutdown();
	hge->Release();
	return (int)success;
}


void SysLog( hgeConstString format, ... )
{
	hgeChar buf[256];

	va_list ap;

	va_start(ap, format);
	hge_vsprintf(buf, format, ap);
	va_end(ap);

	hge_printf(buf);
	hge->System_Log(buf);
}


bool ParseTask( hgeString task, int *resgroup, hgeString mask, bool *inclusive)
//bool ParseTask(char *task, int *resgroup, char *mask, bool *inclusive)
{
	*resgroup = 0;
	*inclusive = true;
	mask[0] = 0;

	hgeString p = task;

	// parse resgroup

	while(isdigit(*p))
	{
		*resgroup *= 10;
		*resgroup += *p - '0';
		p++;
	}

	if(!*p) return true;

	// parse inclusiveness

	if(*p == '-')
		*inclusive = false;
	else
	{
		if(*p != '+') return false;
	}

	p++;
	if(!*p) return false;

	// parse mask

	hge_strcpy(mask, p);
	p = mask;

	while(*p)
	{
		if(*p == '\\' || *p == '/') *p = 0;
		p++;
	}

	p++;
	*p = 0;

	return true;
}
