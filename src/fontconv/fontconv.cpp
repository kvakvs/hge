/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2006, Relish Games
** hge.relishgames.com
**
** HGE Font Description files 1.XX -> 1.6 converter
*/
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <hge.h>
HGE *hge = 0;


struct filelist
{
	hgeChar	filename[256];
	filelist*	next;
};

filelist *files=0;


bool convert( hgeConstString filename);
hgeString _skip_token(hgeString szStr);


int main(int argc, char* argv[])
{
	HANDLE				hSearch;
	HGE_WINAPI_UNICODE_SUFFIX(WIN32_FIND_DATA)	SearchData;
	int					nfiles=0;
	bool				done=false;
	hgeChar				*buf, filename[256];
	filelist			*newFile, *nextFile;
	hgeChar				p[256];

	printf("\nHGE Font 1.XX -> 1.6 converter\nCopyright (C) 2003-2006, Relish Games\n\n");
	
	if(argc!=2)
	{
		printf("Usage: FONTCONV.EXE <wildcard>\n\n");
		printf("All files found by the specified wildcard will\n");
		printf("be automatically converted to newer format. Bitmap files\n");
		printf("should be available along with font description files.\n\n");
		return 0;
	}
	else
	{
		hge_sprintf( p, sizeof p, TXT("%S"), argv[1] );
		hSearch=HGE_WINAPI_UNICODE_SUFFIX(FindFirstFile)(p, &SearchData);
		nextFile=0;

		for(;;)
		{
			if(hSearch==INVALID_HANDLE_VALUE || done)
			{
				FindClose(hSearch);
				break;
			}

			if(!(SearchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				hge_sprintf( filename, sizeof p, TXT("%S"), argv[1] );
				buf=hge_strrchr(filename, TXT('\\'));
				if(!buf) buf=filename; else buf++;
				hge_strcpy(buf,SearchData.cFileName);
				newFile=new filelist;
				hge_strcpy(newFile->filename,filename);
				newFile->next=0;
				if(nextFile) nextFile->next=newFile;
				else files=newFile;
				nextFile=newFile;
			}

			done = ! HGE_WINAPI_UNICODE_SUFFIX(FindNextFile)(hSearch, &SearchData);
		}

		hge=hgeCreate(HGE_VERSION);
		hge->System_SetState(HGE_USESOUND, false);
		hge->System_SetState(HGE_WINDOWED, true);
		hge->System_SetState(HGE_SCREENWIDTH, 640);
		hge->System_SetState(HGE_SCREENHEIGHT, 480);

		if(!hge->System_Initiate())
		{
			hge->Release();
			printf( "\nCan't initiate HGE.\n\n" );
			return 0;
		}

		newFile=files;
		while(newFile)
		{
			if(convert(newFile->filename)) nfiles++;
			nextFile=newFile->next;
			delete newFile;
			newFile=nextFile;
		}

		hge->System_Shutdown();
		hge->Release();
		printf("\n%d file(s) successfully converted.\n\n",nfiles);
		return 0;
	}
}

bool convert(hgeConstString filename)
{
	static hgeChar signature1[] = TXT("[HGEFONT]");
	static hgeChar signature2[] = TXT("[hgefont]");
	static hgeChar tempfile[] = TXT("tmpfont.tmp");
	
	FILE *hf;
	hgeChar *desc, *pdesc, strbuf[256], texname[256], *pbuf;
	int intbuf, i, height, texx, texy, tex_width;
	long size;
	HTEXTURE htex;

	printf("%S - ",filename);
	hf=hge_fopen_rb(filename);
	if(hf==NULL) {
		printf("Can't open file\n"); return false;
	}
	fseek(hf, 0, SEEK_END);
	size=ftell(hf);
	rewind(hf);
	desc=(hgeChar *)malloc(size*sizeof(hgeChar));
	if(desc==NULL) {
		printf("Can't allocate buffer\n"); fclose(hf); return false;
	}
	fread(desc, 1, size, hf);
	fclose(hf);
	if(!hge_strncmp(desc,signature1,hge_strlen(signature1)) ||
	   !hge_strncmp(desc,signature2,hge_strlen(signature2)) )
	{
		printf("Already converted\n"); return false;
	}

	pdesc=desc;
	hf=hge_fopen_w(tempfile);
	if(hf==NULL) {
		printf("Can't create temporary file\n"); free(desc); return false;
	}

	hge_fprintf(hf, TXT("[HGEFONT]\n\n"));

	hge_sscanf(pdesc, TXT(" %s"), strbuf);
	hge_fprintf(hf, TXT("Bitmap=%s\n\n"), strbuf);
	pdesc=_skip_token(pdesc);

	hge_strcpy(texname,filename);
	pbuf=hge_strrchr(texname,TXT('\\'));
	if(!pbuf) pbuf=texname;	else pbuf++;
	hge_strcpy(pbuf, strbuf);

	htex=hge->Texture_Load(texname);
	if(!htex) { 
		hge_printf( TXT("Can't load bitmap\n")); fclose(hf); free(desc); return false;
	}
	tex_width=hge->Texture_GetWidth(htex);
	hge_printf( TXT("%s %dx%d - "),texname, tex_width, hge->Texture_GetWidth(htex));
	hge->Texture_Free(htex);
	texx=texy=0;

	hge_sscanf(pdesc, TXT(" %d"), &height);
	//fprintf(hf,"Height=%d\n",height);
	pdesc=_skip_token(pdesc);

	for(i=0; i<256; i++)
	{
		intbuf=0;
		hge_sscanf(pdesc, TXT(" %d"), &intbuf);
		if(!intbuf) break;
		pdesc=_skip_token(pdesc);
		if(intbuf<0) i+=abs(intbuf)-1;
		else
		{
			if(texx+intbuf > tex_width) {texy+=height;texx=0;}
			if(i>=32 && i<=126) {
				hge_fprintf(hf, TXT("Char=\"%c\",%d,%d,%d,%d,0,0\n"), (char)i, texx, texy, intbuf, height);
			}
			else {
				hge_fprintf(hf, TXT("Char=%2X,%d,%d,%d,%d\n"), i, texx, texy, intbuf, height);
			}
			texx+=intbuf;
		}
	}

	fclose(hf);
	free(desc);

	if(! HGE_WINAPI_UNICODE_SUFFIX(DeleteFile)(filename)) {
		hge_printf( TXT("Can't replace file\n") ); return false;
	}
	if(! HGE_WINAPI_UNICODE_SUFFIX(MoveFile)(tempfile, filename)) {
		hge_printf( TXT("Sorry! Due to system failure the file seems lost\n") ); return false;
	}
	printf("Ok\n");

	return true;
}


hgeString _skip_token(hgeString szStr)
{
	while(*szStr && (*szStr==' ' || *szStr=='\t' || *szStr=='\n' || *szStr=='\r')) szStr++;
	while(*szStr && (*szStr!=' ' && *szStr!='\t' && *szStr!='\n' && *szStr!='\r')) szStr++;
	return szStr;
}
