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

#include "..\..\include\hge.h"
HGE *hge = 0;


struct filelist
{
	char		filename[256];
	filelist*	next;
};

filelist *files=0;


bool convert(char *filename);
char *_skip_token(char *szStr);


int main(int argc, char* argv[])
{
	HANDLE				hSearch;
	WIN32_FIND_DATA		SearchData;
	int					nfiles=0;
	bool				done=false;
	char				*buf, filename[256];
	filelist			*newFile, *nextFile;

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
		hSearch=FindFirstFile(argv[1], &SearchData);
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
				strcpy(filename, argv[1]);
				buf=strrchr(filename, '\\');
				if(!buf) buf=filename; else buf++;
				strcpy(buf,SearchData.cFileName);
				newFile=new filelist;
				strcpy(newFile->filename,filename);
				newFile->next=0;
				if(nextFile) nextFile->next=newFile;
				else files=newFile;
				nextFile=newFile;
			}

			done=!FindNextFile(hSearch, &SearchData);
		}

		hge=hgeCreate(HGE_VERSION);
		hge->System_SetState(HGE_USESOUND, false);
		hge->System_SetState(HGE_WINDOWED, true);
		hge->System_SetState(HGE_SCREENWIDTH, 640);
		hge->System_SetState(HGE_SCREENHEIGHT, 480);

		if(!hge->System_Initiate())
		{
			hge->Release();
			printf("\nCan't initiate HGE.\n\n",nfiles);
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

bool convert(char *filename)
{
	static char signature1[]="[HGEFONT]";
	static char signature2[]="[hgefont]";
	static char tempfile[]="tmpfont.tmp";
	
	FILE *hf;
	char *desc, *pdesc, strbuf[256], texname[256], *pbuf;
	int intbuf, i, height, texx, texy, tex_width;
	long size;
	HTEXTURE htex;

	printf("%s - ",filename);
	hf=fopen(filename, "r");
	if(hf==NULL) { printf("Can't open file\n"); return false; }
	fseek(hf, 0, SEEK_END);
	size=ftell(hf);
	rewind(hf);
	desc=(char *)malloc(size);
	if(desc==NULL) { printf("Can't allocate buffer\n"); fclose(hf); return false; }
	fread(desc, 1, size, hf);
	fclose(hf);
	if(!strncmp(desc,signature1,strlen(signature1)) ||
	   !strncmp(desc,signature2,strlen(signature2)) )
	{ printf("Already converted\n"); return false; }

	pdesc=desc;
	hf=fopen(tempfile, "w");
	if(hf==NULL) { printf("Can't create temporary file\n"); free(desc); return false; }

	fprintf(hf,"[HGEFONT]\n\n");

	sscanf(pdesc, " %s", strbuf);
	fprintf(hf,"Bitmap=%s\n\n",strbuf);
	pdesc=_skip_token(pdesc);

	strcpy(texname,filename);
	pbuf=strrchr(texname,'\\');
	if(!pbuf) pbuf=texname;	else pbuf++;
	strcpy(pbuf, strbuf);

	htex=hge->Texture_Load(texname);
	if(!htex) { printf("Can't load bitmap\n"); fclose(hf); free(desc); return false; }
	tex_width=hge->Texture_GetWidth(htex);
	printf("%s %dx%d - ",texname, tex_width, hge->Texture_GetWidth(htex));
	hge->Texture_Free(htex);
	texx=texy=0;

	sscanf(pdesc, " %d", &height);
	//fprintf(hf,"Height=%d\n",height);
	pdesc=_skip_token(pdesc);

	for(i=0; i<256; i++)
	{
		intbuf=0;
		sscanf(pdesc, " %d", &intbuf);
		if(!intbuf) break;
		pdesc=_skip_token(pdesc);
		if(intbuf<0) i+=abs(intbuf)-1;
		else
		{
			if(texx+intbuf > tex_width) {texy+=height;texx=0;}
			if(i>=32 && i<=126) fprintf(hf,"Char=\"%c\",%d,%d,%d,%d,0,0\n", (char)i, texx, texy, intbuf, height);
			else fprintf(hf,"Char=%2X,%d,%d,%d,%d\n", i, texx, texy, intbuf, height);
			texx+=intbuf;
		}
	}

	fclose(hf);
	free(desc);

	if(!DeleteFile(filename)) { printf("Can't replace file\n"); return false; }
	if(!MoveFile(tempfile, filename)) { printf("Sorry! Due to system failure the file seems lost\n"); return false; }
	printf("Ok\n");

	return true;
}


char *_skip_token(char *szStr)
{
	while(*szStr && (*szStr==' ' || *szStr=='\t' || *szStr=='\n' || *szStr=='\r')) szStr++;
	while(*szStr && (*szStr!=' ' && *szStr!='\t' && *szStr!='\n' && *szStr!='\r')) szStr++;
	return szStr;
}
