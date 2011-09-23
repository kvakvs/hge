/*
** Haaf's Game Engine 1.81
** Copyright (C) 2003-2008, Relish Games
** hge.relishgames.com
**
** PNG Images Optimizer
*/


#include "pngopt.h"

HGE *hge = 0;


struct color
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};


struct filelist
{
	char		filename[256];
	filelist*	next;
};

filelist *files=0;


extern bool Write32BitPNGWithPitch(FILE* fp, void* pBits, bool bNeedAlpha, int nWidth, int nHeight, int nPitch);
bool convert(char *filename);


int main(int argc, char* argv[])
{
	HANDLE				hSearch;
	WIN32_FIND_DATA		SearchData;
	int					nfiles=0;
	bool				done=false;
	char				*buf, filename[256];
	filelist			*newFile, *nextFile;

	printf("\nPNG Optimizer v0.2\nCopyright (C) 2003-2008, Relish Games\n\n");
	
	if(argc!=2)
	{
		printf("Usage: PNGOPT.EXE <wildcard>\n\n");
		printf("The image data of all files found by the specified wildcard\n");
		printf("will be expanded beneath the alpha channel boundary.\n");
		printf("So images will render without artifacts when scaled, stretched,\n");
		printf("rotated or rendered into not integer coordinates.\n\n");
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
		hge->System_SetState(HGE_SCREENWIDTH, 320);
		hge->System_SetState(HGE_SCREENHEIGHT, 200);
		hge->System_SetState(HGE_SHOWSPLASH, false);

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
		printf("\n%d image(s) successfully optimized.\n\n",nfiles);
		return 0;
	}
}

bool convert(char *filename)
{
	HTEXTURE tex;
	int width, height, pitch;
	color *buf;
	
	int i,j,k,l;
	int r,g,b;
	int count;

	FILE *fp=0;
	
	printf("%s - ", filename);
	
	tex = hge->Texture_Load(filename);
	if(!tex) { printf("Can't load texture.\n"); return false; }	

	width  = hge->Texture_GetWidth(tex, true);
	height = hge->Texture_GetHeight(tex, true);
	pitch  = hge->Texture_GetWidth(tex, false);

	buf=(color *)hge->Texture_Lock(tex, false);
	if(!buf) { printf("Can't lock texture.\n"); return false; }	

	for(i=0; i<height; i++)
		for(j=0; j<width; j++)
			if(!buf[i*pitch+j].a)
			{
				count = 0;
				r=g=b = 0;

				for(k=-1; k<=1; k++)
					for(l=-1; l<=1; l++)
						if(i+k >= 0 && i+k < height &&
						   j+l >= 0 && j+l < width &&
						   buf[(i+k)*pitch + (j+l)].a)
						{
							r += buf[(i+k)*pitch + (j+l)].r;
							g += buf[(i+k)*pitch + (j+l)].g;
							b += buf[(i+k)*pitch + (j+l)].b;
							count++;
						}

				if(count)
				{
					buf[i*pitch+j].r = unsigned char(r / count);
					buf[i*pitch+j].g = unsigned char(g / count);
					buf[i*pitch+j].b = unsigned char(b / count);
				}
			}


	fp=fopen(filename,"wb");
	if(!fp)
	{
		hge->Texture_Unlock(tex);
		printf("Can't write to file.\n");
		return false;
	}

	if(!Write32BitPNGWithPitch(fp, buf, true, width, height, pitch))
	{
		hge->Texture_Unlock(tex);
		fclose(fp);
		printf("Error writing data.\n");
		return false;
	}

	fclose(fp);
	hge->Texture_Unlock(tex);

	printf("Ok\n");
	return true;
}
