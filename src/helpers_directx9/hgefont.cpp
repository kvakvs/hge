/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeFont helper class implementation
*/


#include "..\..\include\hgefont.h"
#include <stdlib.h>
#include <stdio.h>

const char FNTHEADERTAG[] = "[HGEFONT]";
const char FNTBITMAPTAG[] = "Bitmap";
const char FNTCHARTAG[]   = "Char";


HGE *hgeFont::hge=0;
char hgeFont::buffer[1024];


hgeFont::hgeFont(const char *szFont, bool bMipmap)
{
	void	*data;
	DWORD	size;
	char	*desc, *pdesc;
	char	linebuf[256];
	char	buf[MAX_PATH], *pbuf;
	char	chr;
	int		i, x, y, w, h, a, c;

	// Setup variables
	
	hge=hgeCreate(HGE_VERSION);

	fHeight=0.0f;
	fScale=1.0f;
	fProportion=1.0f;
	fRot=0.0f;
	fTracking=0.0f;
	fSpacing=1.0f;
	hTexture=0;

	fZ=0.5f;
	nBlend=BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE;
	dwCol=0xFFFFFFFF;

	ZeroMemory( &letters, sizeof(letters) );
	ZeroMemory( &pre, sizeof(letters) );
	ZeroMemory( &post, sizeof(letters) );
	
	// Load font description

	data=hge->Resource_Load(szFont, &size);
	if(!data) return;

	desc = new char[size+1];
	memcpy(desc,data,size);
	desc[size]=0;
	hge->Resource_Free(data);

	pdesc=_get_line(desc,linebuf);
	if(strcmp(linebuf, FNTHEADERTAG))
	{
		hge->System_Log("Font %s has incorrect format.", szFont);
		delete[] desc;	
		return;
	}

	// Parse font description

	while(pdesc = _get_line(pdesc,linebuf))
	{
		if(!strncmp(linebuf, FNTBITMAPTAG, sizeof(FNTBITMAPTAG)-1 ))
		{
			strcpy(buf,szFont);
			pbuf=strrchr(buf,'\\');
			if(!pbuf) pbuf=strrchr(buf,'/');
			if(!pbuf) pbuf=buf;
			else pbuf++;
			if(!sscanf(linebuf, "Bitmap = %s", pbuf)) continue;

			hTexture=hge->Texture_Load(buf, 0, bMipmap);
			if(!hTexture)
			{
				delete[] desc;	
				return;
			}
		}

		else if(!strncmp(linebuf, FNTCHARTAG, sizeof(FNTCHARTAG)-1 ))
		{
			pbuf=strchr(linebuf,'=');
			if(!pbuf) continue;
			pbuf++;
			while(*pbuf==' ') pbuf++;
			if(*pbuf=='\"')
			{
				pbuf++;
				i=(unsigned char)*pbuf++;
				pbuf++; // skip "
			}
			else
			{
				i=0;
				while((*pbuf>='0' && *pbuf<='9') || (*pbuf>='A' && *pbuf<='F') || (*pbuf>='a' && *pbuf<='f'))
				{
					chr=*pbuf;
					if(chr >= 'a') chr-='a'-':';
					if(chr >= 'A') chr-='A'-':';
					chr-='0';
					if(chr>0xF) chr=0xF;
					i=(i << 4) | chr;
					pbuf++;
				}
				if(i<0 || i>255) continue;
			}
			sscanf(pbuf, " , %d , %d , %d , %d , %d , %d", &x, &y, &w, &h, &a, &c);

			letters[i] = new hgeSprite(hTexture, (float)x, (float)y, (float)w, (float)h);
			pre[i]=(float)a;
			post[i]=(float)c;
			if(h>fHeight) fHeight=(float)h;
		}
	}

	delete[] desc;	
}


hgeFont::~hgeFont()
{
	for(int i=0; i<256; i++)
		if(letters[i]) delete letters[i];
	if(hTexture) hge->Texture_Free(hTexture);
	hge->Release();
}

void hgeFont::Render(float x, float y, int align, const char *string)
{
	int i;
	float	fx=x;

	align &= HGETEXT_HORZMASK;
	if(align==HGETEXT_RIGHT) fx-=GetStringWidth(string, false);
	if(align==HGETEXT_CENTER) fx-=int(GetStringWidth(string, false)/2.0f);

	while(*string)
	{
		if(*string=='\n')
		{
			y += int(fHeight*fScale*fSpacing);
			fx = x;
			if(align == HGETEXT_RIGHT)  fx -= GetStringWidth(string+1, false);
			if(align == HGETEXT_CENTER) fx -= int(GetStringWidth(string+1, false)/2.0f);
		}
		else
		{
			i=(unsigned char)*string;
			if(!letters[i]) i='?';
			if(letters[i])
			{
				fx += pre[i]*fScale*fProportion;
				letters[i]->RenderEx(fx, y, fRot, fScale*fProportion, fScale);
				fx += (letters[i]->GetWidth()+post[i]+fTracking)*fScale*fProportion;
			}
		}
		string++;
	}
}

void hgeFont::printf(float x, float y, int align, const char *format, ...)
{
	char	*pArg=(char *) &format+sizeof(format);

	_vsnprintf(buffer, sizeof(buffer)-1, format, pArg);
	buffer[sizeof(buffer)-1]=0;
	//vsprintf(buffer, format, pArg);

	Render(x,y,align,buffer);
}

void hgeFont::printfb(float x, float y, float w, float h, int align, const char *format, ...)
{
	char	chr, *pbuf, *prevword, *linestart;
	int		i,lines=0;
	float	tx, ty, hh, ww;
	char	*pArg=(char *) &format+sizeof(format);

	_vsnprintf(buffer, sizeof(buffer)-1, format, pArg);
	buffer[sizeof(buffer)-1]=0;
	//vsprintf(buffer, format, pArg);

	linestart=buffer;
	pbuf=buffer;
	prevword=0;

	for(;;)
	{
		i=0;
		while(pbuf[i] && pbuf[i]!=' ' && pbuf[i]!='\n') i++;

		chr=pbuf[i];
		pbuf[i]=0;
		ww=GetStringWidth(linestart);
		pbuf[i]=chr;

		if(ww > w)
		{
			if(pbuf==linestart)
			{
				pbuf[i]='\n';
				linestart=&pbuf[i+1];
			}
			else
			{
				*prevword='\n';
				linestart=prevword+1;
			}

			lines++;
		}

		if(pbuf[i]=='\n')
		{
			prevword=&pbuf[i];
			linestart=&pbuf[i+1];
			pbuf=&pbuf[i+1];
			lines++;
			continue;
		}

		if(!pbuf[i]) {lines++;break;}

		prevword=&pbuf[i];
		pbuf=&pbuf[i+1];
	}
	
	tx=x;
	ty=y;
	hh=fHeight*fSpacing*fScale*lines;

	switch(align & HGETEXT_HORZMASK)
	{
		case HGETEXT_LEFT: break;
		case HGETEXT_RIGHT: tx+=w; break;
		case HGETEXT_CENTER: tx+=int(w/2); break;
	}

	switch(align & HGETEXT_VERTMASK)
	{
		case HGETEXT_TOP: break;
		case HGETEXT_BOTTOM: ty+=h-hh; break;
		case HGETEXT_MIDDLE: ty+=int((h-hh)/2); break;
	}

	Render(tx,ty,align,buffer);
}

float hgeFont::GetStringWidth(const char *string, bool bMultiline) const
{
	int i;
	float linew, w = 0;

	while(*string)
	{
		linew = 0;

		while(*string && *string != '\n')
		{
			i=(unsigned char)*string;
			if(!letters[i]) i='?';
			if(letters[i])
				linew += letters[i]->GetWidth() + pre[i] + post[i] + fTracking;

			string++;
		}

		if(!bMultiline) return linew*fScale*fProportion;

		if(linew > w) w = linew;

		while (*string == '\n' || *string == '\r') string++;
	}

	return w*fScale*fProportion;
}

void hgeFont::SetColor(DWORD col)
{
	dwCol = col;

	for(int i=0; i<256; i++)
		if(letters[i])
			letters[i]->SetColor(col);
}

void hgeFont::SetZ(float z)
{
	fZ = z;

	for(int i=0; i<256; i++)
		if(letters[i])
			letters[i]->SetZ(z);
}

void hgeFont::SetBlendMode(int blend)
{
	nBlend = blend;

	for(int i=0; i<256; i++)
		if(letters[i])
			letters[i]->SetBlendMode(blend);
}

char *hgeFont::_get_line(char *file, char *line)
{
	int i=0;

	if(!file[i]) return 0;

	while(file[i] && file[i]!='\n' && file[i]!='\r')
	{
		line[i]=file[i];
		i++;
	}
	line[i]=0;

	while(file[i] && (file[i]=='\n' || file[i]=='\r')) i++;

	return file + i;
}