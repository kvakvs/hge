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


HGE *hgeFont::hge_=nullptr;
char hgeFont::buffer_[1024];


hgeFont::hgeFont(const char *szFont, bool bMipmap)
{
    uint32_t	size;
    char	linebuf[256];
    char	buf[MAX_PATH], *pbuf;
    int		i, x, y, w, h, a, c;

    // Setup variables

    hge_=hgeCreate(HGE_VERSION);

    height_=0.0f;
    scale_=1.0f;
    proportion_=1.0f;
    rot_=0.0f;
    tracking_=0.0f;
    spacing_=1.0f;
    texture_=0;

    z_=0.5f;
    blend_=BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE;
    col_=0xFFFFFFFF;

    ZeroMemory( &letters_, sizeof(letters_) );
    ZeroMemory( &pre_, sizeof(letters_) );
    ZeroMemory( &post_, sizeof(letters_) );

    // Load font description

    void *data = hge_->Resource_Load(szFont, &size);
    if(!data) {
        return;
    }

    char *desc = new char[size+1];
    memcpy(desc,data,size);
    desc[size]=0;
    hge_->Resource_Free(data);

    char *pdesc = _get_line(desc,linebuf);
    if(strcmp(linebuf, FNTHEADERTAG)) {
        hge_->System_Log("Font %s has incorrect format.", szFont);
        delete[] desc;
        return;
    }

    // Parse font description

    while(pdesc = _get_line(pdesc,linebuf)) {
        if(!strncmp(linebuf, FNTBITMAPTAG, sizeof(FNTBITMAPTAG)-1 )) {
            strcpy(buf,szFont);
            pbuf=strrchr(buf,'\\');
            if(!pbuf) {
                pbuf=strrchr(buf,'/');
            }
            if(!pbuf) {
                pbuf=buf;
            } else {
                pbuf++;
            }
            if(!sscanf(linebuf, "Bitmap = %s", pbuf)) {
                continue;
            }

            texture_=hge_->Texture_Load(buf, 0, bMipmap);
            if(!texture_) {
                delete[] desc;
                return;
            }
        }

        else if(!strncmp(linebuf, FNTCHARTAG, sizeof(FNTCHARTAG)-1 )) {
            pbuf=strchr(linebuf,'=');
            if(!pbuf) {
                continue;
            }
            pbuf++;
            while(*pbuf==' ') {
                pbuf++;
            }
            if(*pbuf=='\"') {
                pbuf++;
                i=(unsigned char)*pbuf++;
                pbuf++; // skip "
            } else {
                i=0;
                while((*pbuf>='0' && *pbuf<='9') || (*pbuf>='A' && *pbuf<='F') || (*pbuf>='a' && *pbuf<='f')) {
                    char chr = *pbuf;
                    if(chr >= 'a') {
                        chr-='a'-':';
                    }
                    if(chr >= 'A') {
                        chr-='A'-':';
                    }
                    chr-='0';
                    if(chr>0xF) {
                        chr=0xF;
                    }
                    i=(i << 4) | chr;
                    pbuf++;
                }
                if(i<0 || i>255) {
                    continue;
                }
            }
            sscanf(pbuf, " , %d , %d , %d , %d , %d , %d", &x, &y, &w, &h, &a, &c);

            letters_[i] = new hgeSprite(texture_, (float)x, (float)y, (float)w, (float)h);
            pre_[i]=(float)a;
            post_[i]=(float)c;
            if(h>height_) {
                height_=(float)h;
            }
        }
    }

    delete[] desc;
}


hgeFont::~hgeFont()
{
    for(int i=0; i<256; i++)
        if(letters_[i]) {
            delete letters_[i];
        }
    if(texture_) {
        hge_->Texture_Free(texture_);
    }
    hge_->Release();
}

void hgeFont::Render(float x, float y, int align, const char *string)
{
    float	fx=x;

    align &= HGETEXT_HORZMASK;
    if(align==HGETEXT_RIGHT) {
        fx-=GetStringWidth(string, false);
    }
    if(align==HGETEXT_CENTER) {
        fx-=int(GetStringWidth(string, false)/2.0f);
    }

    while(*string) {
        if(*string=='\n') {
            y += int(height_*scale_*spacing_);
            fx = x;
            if(align == HGETEXT_RIGHT) {
                fx -= GetStringWidth(string+1, false);
            }
            if(align == HGETEXT_CENTER) {
                fx -= int(GetStringWidth(string+1, false)/2.0f);
            }
        } else {
            int i = (unsigned char)*string;
            if(!letters_[i]) {
                i='?';
            }
            if(letters_[i]) {
                fx += pre_[i]*scale_*proportion_;
                letters_[i]->RenderEx(fx, y, rot_, scale_*proportion_, scale_);
                fx += (letters_[i]->GetWidth()+post_[i]+tracking_)*scale_*proportion_;
            }
        }
        string++;
    }
}

void hgeFont::printf(float x, float y, int align, const char *format, ...)
{
    char	*pArg=(char *) &format+sizeof(format);

    _vsnprintf(buffer_, sizeof(buffer_)-1, format, pArg);
    buffer_[sizeof(buffer_)-1]=0;
    //vsprintf(buffer, format, pArg);

    Render(x,y,align,buffer_);
}

void hgeFont::printfb(float x, float y, float w, float h, int align, const char *format, ...)
{
    int lines=0;
    char	*pArg=(char *) &format+sizeof(format);

    _vsnprintf(buffer_, sizeof(buffer_)-1, format, pArg);
    buffer_[sizeof(buffer_)-1]=0;
    //vsprintf(buffer, format, pArg);

    char *linestart = buffer_;
    char *pbuf = buffer_;
    char *prevword = nullptr;

    for(;;) {
        int i = 0;
        while(pbuf[i] && pbuf[i]!=' ' && pbuf[i]!='\n') {
            i++;
        }

        char chr = pbuf[i];
        pbuf[i]=0;
        float ww = GetStringWidth(linestart);
        pbuf[i]=chr;

        if(ww > w) {
            if(pbuf==linestart) {
                pbuf[i]='\n';
                linestart=&pbuf[i+1];
            } else {
                *prevword='\n';
                linestart=prevword+1;
            }

            lines++;
        }

        if(pbuf[i]=='\n') {
            prevword=&pbuf[i];
            linestart=&pbuf[i+1];
            pbuf=&pbuf[i+1];
            lines++;
            continue;
        }

        if(!pbuf[i]) {
            lines++;
            break;
        }

        prevword=&pbuf[i];
        pbuf=&pbuf[i+1];
    }

    float tx = x;
    float ty = y;
    float hh = height_*spacing_*scale_*lines;

    switch(align & HGETEXT_HORZMASK) {
    case HGETEXT_LEFT:
        break;
    case HGETEXT_RIGHT:
        tx+=w;
        break;
    case HGETEXT_CENTER:
        tx+=int(w/2);
        break;
    }

    switch(align & HGETEXT_VERTMASK) {
    case HGETEXT_TOP:
        break;
    case HGETEXT_BOTTOM:
        ty+=h-hh;
        break;
    case HGETEXT_MIDDLE:
        ty+=int((h-hh)/2);
        break;
    }

    Render(tx,ty,align,buffer_);
}

float hgeFont::GetStringWidth(const char *string, bool bMultiline) const
{
    float w = 0;

    while(*string) {
        float linew = 0;

        while(*string && *string != '\n') {
            int i = (unsigned char)*string;
            if(!letters_[i]) {
                i='?';
            }
            if(letters_[i]) {
                linew += letters_[i]->GetWidth() + pre_[i] + post_[i] + tracking_;
            }

            string++;
        }

        if(!bMultiline) {
            return linew*scale_*proportion_;
        }

        if(linew > w) {
            w = linew;
        }

        while (*string == '\n' || *string == '\r') {
            string++;
        }
    }

    return w*scale_*proportion_;
}

void hgeFont::SetColor(uint32_t col)
{
    col_ = col;

    for(int i=0; i<256; i++)
        if(letters_[i]) {
            letters_[i]->SetColor(col);
        }
}

void hgeFont::SetZ(float z)
{
    z_ = z;

    for(int i=0; i<256; i++)
        if(letters_[i]) {
            letters_[i]->SetZ(z);
        }
}

void hgeFont::SetBlendMode(int blend)
{
    blend_ = blend;

    for(int i=0; i<256; i++)
        if(letters_[i]) {
            letters_[i]->SetBlendMode(blend);
        }
}

char *hgeFont::_get_line(char *file, char *line)
{
    int i=0;

    if(!file[i]) {
        return nullptr;
    }

    while(file[i] && file[i]!='\n' && file[i]!='\r') {
        line[i]=file[i];
        i++;
    }
    line[i]=0;

    while(file[i] && (file[i]=='\n' || file[i]=='\r')) {
        i++;
    }

    return file + i;
}
