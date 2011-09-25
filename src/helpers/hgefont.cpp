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

#include <crtdbg.h> // for _ASSERTE
#include <memory> // for unique_ptr

hgeConstString FNTHEADERTAG = TXT("[HGEFONT]");
hgeConstString FNTBITMAPTAG = TXT("Bitmap");
hgeConstString FNTCHARTAG   = TXT("Char");


HGE *hgeFont::hge=0;
hgeChar hgeFont::m_buffer[1024];


hgeFont::hgeFont(hgeConstString szFont, bool bMipmap)
{
	void		* data;
	uint32_t	size;
	//hgeChar		* desc;
	hgeChar		* pdesc;
	hgeChar		linebuf[256];
	hgeChar		buf[MAX_PATH], *pbuf;
	hgeChar		chr;
	int			i, x, y, w, h, a, c;

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

	std::unique_ptr <hgeChar> desc( new hgeChar[size+1] );
	//memcpy(desc,data,size);
	hgeStrFromUtf8( (const char *)data, desc.get(), size+1 );
	//desc[size]=0;

	hge->Resource_Free(data);

	pdesc=_get_line( desc.get(), linebuf );
	if( hge_strcmp( linebuf, FNTHEADERTAG ))
	{
		hge->System_Log( TXT("Font %s has incorrect format."), szFont );
		//delete[] desc;	
		return;
	}

	// Parse font description

	while(pdesc = _get_line(pdesc,linebuf))
	{
		if(!hge_strncmp(linebuf, FNTBITMAPTAG, sizeof(FNTBITMAPTAG)-1 ))
		{
			hge_strcpy(buf,szFont);
			pbuf=hge_strrchr(buf, TXT('\\'));
			if(!pbuf) pbuf=hge_strrchr(buf, TXT('/'));
			if(!pbuf) pbuf=buf;
			else pbuf++;
			if(! hge_sscanf(linebuf, TXT("Bitmap = %s"), pbuf)) continue;

			hTexture=hge->Texture_Load(buf, 0, bMipmap);
			if(!hTexture)
			{
				//delete[] desc;	
				return;
			}
		}

		else if(!hge_strncmp(linebuf, FNTCHARTAG, sizeof(FNTCHARTAG)-1 ))
		{
			pbuf=hge_strchr(linebuf,TXT('='));
			if(!pbuf) continue;
			pbuf++;
			while(*pbuf==' ') pbuf++;
			if(*pbuf=='\"')
			{
				pbuf++;
				i=(hgeChar)*pbuf++;
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
			hge_sscanf(pbuf, TXT(" , %d , %d , %d , %d , %d , %d"), &x, &y, &w, &h, &a, &c);

			letters[i] = new hgeSprite(hTexture, (float)x, (float)y, (float)w, (float)h);
			pre[i]=(float)a;
			post[i]=(float)c;
			if(h>fHeight) fHeight=(float)h;
		}
	}

	//delete[] desc;	
	//_ASSERTE(false); // todo copy here BMFONT loader code
}


hgeFont::~hgeFont()
{
	for(int i=0; i<256; i++)
		if(letters[i]) delete letters[i];
	if(hTexture) hge->Texture_Free(hTexture);
	hge->Release();
}

void hgeFont::Render(float x, float y, int align, hgeConstString _str)
{
	const hgeChar * str = _str;
	int i;
	float	fx=x;

	align &= HGETEXT_HORZMASK;
	if(align==HGETEXT_RIGHT) fx-=GetStringWidth((hgeString)str, false);
	if(align==HGETEXT_CENTER) fx-=int(GetStringWidth((hgeString)str, false)/2.0f);

	while(*str)
	{
		if(*str=='\n')
		{
			y += int(fHeight*fScale*fSpacing);
			fx = x;
			if(align == HGETEXT_RIGHT)  fx -= GetStringWidth((hgeString)str+1, false);
			if(align == HGETEXT_CENTER) fx -= int(GetStringWidth((hgeString)str+1, false)/2.0f);
		}
		else
		{
			i=(hgeChar)*str;
			if(!letters[i]) i='?';
			if(letters[i])
			{
				fx += pre[i]*fScale*fProportion;
				letters[i]->RenderEx(fx, y, fRot, fScale*fProportion, fScale);
				fx += (letters[i]->GetWidth()+post[i]+fTracking)*fScale*fProportion;
			}
		}
		str++;
	}
}

void hgeFont::printf(float x, float y, int align, hgeConstString format, ...)
{
	va_list pArg = (va_list) &format+sizeof(format);

	hge_vsnprintf( m_buffer, HGEFONT_BUFFER_SZ-1, format, pArg );
	m_buffer[HGEFONT_BUFFER_SZ-1]=0;
	//vsprintf(buffer, format, pArg);

	Render(x,y,align,m_buffer);
}

void hgeFont::printfb(float x, float y, float w, float h, int align, hgeConstString format, ...)
{
	hgeChar chr, *pbuf, *prevword, *linestart;
	int		i,lines=0;
	float	tx, ty, hh, ww;
	va_list pArg = (va_list) &format+sizeof(format);

	hge_vsnprintf(m_buffer, HGEFONT_BUFFER_SZ-1, format, pArg);
	m_buffer[HGEFONT_BUFFER_SZ-1]=0;
	//vsprintf(buffer, format, pArg);

	linestart=m_buffer;
	pbuf=m_buffer;
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

	Render(tx,ty,align,m_buffer);
}

float hgeFont::GetStringWidth(hgeConstString _str, bool bMultiline) const
{
	int i;
	float linew, w = 0;
	const hgeChar * str = _str;

	while(*str)
	{
		linew = 0;

		while(*str && *str != '\n')
		{
			i=(unsigned char)*str;
			if(!letters[i]) i='?';
			if(letters[i])
				linew += letters[i]->GetWidth() + pre[i] + post[i] + fTracking;

			str++;
		}

		if(!bMultiline) return linew*fScale*fProportion;

		if(linew > w) w = linew;

		while (*str == '\n' || *str == '\r') str++;
	}

	return w*fScale*fProportion;
}

void hgeFont::SetColor(uint32_t col)
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

hgeString hgeFont::_get_line(hgeString file, hgeString line)
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