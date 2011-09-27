/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeFont helper class implementation
*/
#include <hgefont.h>
#include <stdlib.h>
#include <stdio.h>

#include <crtdbg.h> // for _ASSERTE
#include <memory> // for unique_ptr

hgeConstString FNTHEADERTAG = TXT("[HGEFONT]");
hgeConstString FNTBITMAPTAG = TXT("Bitmap");
hgeConstString FNTCHARTAG = TXT("Char");


HGE *hgeFont::m_hge = 0;

#if ! HGE_UNICODE
	hgeChar hgeFont::m_buffer[1024];
#endif // ! HGE_UNICODE

#if ! HGE_UNICODE
hgeFont::hgeFont(hgeConstString szFont, bool bMipmap)
{
	void * data;
	uint32_t size;
	//hgeChar * desc;
	hgeChar * pdesc;
	hgeChar linebuf[256];
	hgeChar buf[MAX_PATH], *pbuf;
	hgeChar chr;
	int i, x, y, w, h, a, c;

	// Setup variables

	m_hge = hgeCreate(HGE_VERSION);

	m_height = 0.0f;
	m_scale = 1.0f;
	m_proportion = 1.0f;
	m_rotation = 0.0f;
	m_tracking = 0.0f;
	m_spacing = 1.0f;
	m_texture = 0;

	m_depth = 0.5f;
	m_blending = BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE;
	m_color = 0xFFFFFFFF;

	ZeroMemory( &m_letters, sizeof(m_letters) );
	ZeroMemory( &m_pre, sizeof(m_letters) );
	ZeroMemory( &m_post, sizeof(m_letters) );

	// Load font description

	data = m_hge->Resource_Load(szFont, &size);
	if (!data)
		return;

	hgeUniquePtr<hgeChar> desc(new hgeChar[size + 1]);
	//memcpy(desc,data,size);
	hgeStrFromUtf8((const char *) data, desc.get(), size + 1);
	//desc[size]=0;

	m_hge->Resource_Free(data);

	pdesc = _get_line(desc.get(), linebuf);
	if (hge_strcmp(linebuf, FNTHEADERTAG))
	{
		m_hge->System_Log(TXT("Font %s has incorrect format."), szFont);
		//delete[] desc;	
		return;
	}

	// Parse font description

	while (pdesc = _get_line(pdesc, linebuf))
	{
		if (!hge_strncmp(linebuf, FNTBITMAPTAG, sizeof(FNTBITMAPTAG) - 1))
		{
			hge_strcpy(buf, szFont);
			pbuf = hge_strrchr(buf, TXT('\\'));
			if (!pbuf)
				pbuf = hge_strrchr(buf, TXT('/'));
			if (!pbuf)
				pbuf = buf;
			else
				pbuf++;
			if (!hge_sscanf(linebuf, TXT("Bitmap = %s"), pbuf))
				continue;

			m_texture = m_hge->Texture_Load(buf, 0, bMipmap);
			if (!m_texture)
			{
				//delete[] desc;	
				return;
			}
		}

		else if (!hge_strncmp(linebuf, FNTCHARTAG, sizeof(FNTCHARTAG) - 1))
		{
			pbuf = hge_strchr(linebuf, TXT('='));
			if (!pbuf)
				continue;
			pbuf++;
			while (*pbuf == ' ')
				pbuf++;
			if (*pbuf == '\"')
			{
				pbuf++;
				i = (hgeChar) *pbuf++;
				pbuf++; // skip "
			}
			else
			{
				i = 0;
				while ((*pbuf >= '0' && *pbuf <= '9') || (*pbuf >= 'A' && *pbuf
						<= 'F') || (*pbuf >= 'a' && *pbuf <= 'f'))
				{
					chr = *pbuf;
					if (chr >= 'a')
						chr -= 'a' - ':';
					if (chr >= 'A')
						chr -= 'A' - ':';
					chr -= '0';
					if (chr > 0xF)
						chr = 0xF;
					i = (i << 4) | chr;
					pbuf++;
				}
				if (i < 0 || i > 255)
					continue;
			}
			hge_sscanf(pbuf, TXT(" , %d , %d , %d , %d , %d , %d"), &x, &y, &w,
					&h, &a, &c);

			m_letters[i] = new hgeSprite(m_texture, (float) x, (float) y,
					(float) w, (float) h);
			m_pre[i] = (float) a;
			m_post[i] = (float) c;
			if (h > m_height)
				m_height = (float) h;
		}
	}

	//delete[] desc;	
	//_ASSERTE(false); // todo copy here BMFONT loader code
}
#endif


#if ! HGE_UNICODE
hgeFont::~hgeFont()
{
	for (int i = 0; i < 256; i++)
		if (m_letters[i])
			delete m_letters[i];
	if (m_texture)
		hge->Texture_Free(m_texture);
	hge->Release();
}
#endif


#if ! HGE_UNICODE
void hgeFont::Render(float x, float y, int align, hgeConstString _str)
{
	const hgeChar * str = _str;
	int i;
	float fx = x;

	align &= HGETEXT_HORZMASK;
	if (align == HGETEXT_RIGHT)
		fx -= GetStringWidth((hgeString) str, false);
	if (align == HGETEXT_CENTER)
		fx -= int(GetStringWidth((hgeString) str, false) / 2.0f);

	while (*str)
	{
		if (*str == '\n')
		{
			y += int(m_height * m_scale * m_spacing);
			fx = x;
			if (align == HGETEXT_RIGHT)
				fx -= GetStringWidth((hgeString) str + 1, false);
			if (align == HGETEXT_CENTER)
				fx -= int(GetStringWidth((hgeString) str + 1, false) / 2.0f);
		}
		else
		{
			i = (hgeChar) *str;
			if (!m_letters[i])
				i = '?';
			if (m_letters[i])
			{
				fx += m_pre[i] * m_scale * m_proportion;
				m_letters[i]->RenderEx(fx, y, m_rotation, m_scale * m_proportion, m_scale);
				fx += (m_letters[i]->GetWidth() + m_post[i] + m_tracking) * m_scale
						* m_proportion;
			}
		}
		str++;
	}
}
#endif


#if ! HGE_UNICODE
void hgeFont::printf(float x, float y, int align, hgeConstString format, ...)
{
	va_list pArg = (va_list) &format + sizeof(format);

	hge_vsnprintf(m_buffer, HGEFONT_BUFFER_SZ - 1, format, pArg);
	m_buffer[HGEFONT_BUFFER_SZ - 1] = 0;
	//vsprintf(buffer, format, pArg);

	Render(x, y, align, m_buffer);
}
#endif


#if ! HGE_UNICODE
void hgeFont::printfb(float x, float y, float w, float h, int align, hgeConstString format, ...)
{
	hgeChar chr, *pbuf, *prevword, *linestart;
	int i, lines = 0;
	float tx, ty, hh, ww;
	va_list pArg = (va_list) &format + sizeof(format);

	hge_vsnprintf(m_buffer, HGEFONT_BUFFER_SZ - 1, format, pArg);
	m_buffer[HGEFONT_BUFFER_SZ - 1] = 0;
	//vsprintf(buffer, format, pArg);

	linestart = m_buffer;
	pbuf = m_buffer;
	prevword = 0;

	for (;;)
	{
		i = 0;
		while (pbuf[i] && pbuf[i] != ' ' && pbuf[i] != '\n')
			i++;

		chr = pbuf[i];
		pbuf[i] = 0;
		ww = GetStringWidth(linestart);
		pbuf[i] = chr;

		if (ww > w)
		{
			if (pbuf == linestart)
			{
				pbuf[i] = '\n';
				linestart = &pbuf[i + 1];
			}
			else
			{
				*prevword = '\n';
				linestart = prevword + 1;
			}

			lines++;
		}

		if (pbuf[i] == '\n')
		{
			prevword = &pbuf[i];
			linestart = &pbuf[i + 1];
			pbuf = &pbuf[i + 1];
			lines++;
			continue;
		}

		if (!pbuf[i])
		{
			lines++;
			break;
		}

		prevword = &pbuf[i];
		pbuf = &pbuf[i + 1];
	}

	tx = x;
	ty = y;
	hh = m_height * m_spacing * m_scale * lines;

	switch (align & HGETEXT_HORZMASK)
	{
	case HGETEXT_LEFT:
		break;
	case HGETEXT_RIGHT:
		tx += w;
		break;
	case HGETEXT_CENTER:
		tx += int(w / 2);
		break;
	}

	switch (align & HGETEXT_VERTMASK)
	{
	case HGETEXT_TOP:
		break;
	case HGETEXT_BOTTOM:
		ty += h - hh;
		break;
	case HGETEXT_MIDDLE:
		ty += int((h - hh) / 2);
		break;
	}

	Render(tx, ty, align, m_buffer);
}
#endif


#if ! HGE_UNICODE
float hgeFont::GetStringWidth(hgeConstString _str, bool bMultiline) const
{
	int i;
	float linew, w = 0;
	const hgeChar * str = _str;

	while (*str)
	{
		linew = 0;

		while (*str && *str != '\n')
		{
			i = (unsigned char) *str;
			if (!m_letters[i])
				i = '?';
			if (m_letters[i])
				linew += m_letters[i]->GetWidth() + m_pre[i] + m_post[i] + m_tracking;

			str++;
		}

		if (!bMultiline)
			return linew * m_scale * m_proportion;

		if (linew > w)
			w = linew;

		while (*str == '\n' || *str == '\r')
			str++;
	}

	return w * m_scale * m_proportion;
}
#endif


#if ! HGE_UNICODE
void hgeFont::SetColor(uint32_t col)
{
	m_color = col;

	for (int i = 0; i < 256; i++)
		if (m_letters[i])
			m_letters[i]->SetColor(col);
}
#endif


#if ! HGE_UNICODE
void hgeFont::SetZ(float z)
{
	m_depth = z;

	for (int i = 0; i < 256; i++)
		if (m_letters[i])
			m_letters[i]->SetZ(z);
}
#endif


#if ! HGE_UNICODE
void hgeFont::SetBlendMode(int blend)
{
	m_blending = blend;

	for (int i = 0; i < 256; i++)
		if (m_letters[i])
			m_letters[i]->SetBlendMode(blend);
}
#endif


#if ! HGE_UNICODE
hgeString hgeFont::_get_line(hgeString file, hgeString line)
{
	int i = 0;

	if (!file[i])
		return 0;

	while (file[i] && file[i] != '\n' && file[i] != '\r')
	{
		line[i] = file[i];
		i++;
	}
	line[i] = 0;

	while (file[i] && (file[i] == '\n' || file[i] == '\r'))
		i++;

	return file + i;
}
#endif