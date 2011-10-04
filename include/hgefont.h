/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeFont helper class header
*/
#pragma once

#ifndef HGEFONT_H
#define HGEFONT_H


#include <hge.h>
#include <hgesprite.h>


#define HGETEXT_LEFT		0
#define HGETEXT_RIGHT		1
#define HGETEXT_CENTER		2
#define HGETEXT_HORZMASK	0x03

#define HGETEXT_TOP			0
#define HGETEXT_BOTTOM		4
#define HGETEXT_MIDDLE		8
#define HGETEXT_VERTMASK	0x0C


/*
** HGE Font interface (to use with BMFont font)
** Supports Unicode
*/
#if HGE_UNICODE
class hgeFont
{
public:
	hgeFont(hgeConstString filename, bool bMipmap=false);

	void	Render(float x, float y, int align, hgeConstString string) {}
	void	printf(float x, float y, int align, hgeConstString format, ...) {}
	void	printfb(float x, float y, float w, float h, int align, hgeConstString format, ...) {}

	void	SetColor(uint32_t col)		{ m_color = col; }
	void	SetZ(float z)				{ m_depth = z; }
	void	SetBlendMode(int blend)		{ m_blending = blend; }
	void	SetScale(float scale)		{ m_scale = scale; }
	void	SetProportion(float prop)	{ }
	void	SetRotation(float rot)		{ }
	void	SetTracking(float tracking) { }
	void	SetSpacing(float spacing)	{ }

	uint32_t	GetColor() const		{ return m_color; }
	float	GetZ() const				{ return m_depth; }
	int		GetBlendMode() const		{ return m_blending; }
	float	GetScale() const			{ return m_scale; }
	float	GetProportion() const		{ return 0.0f; }
	float	GetRotation() const			{ return 0.0f; }
	float	GetTracking() const			{ return 0.0f; }
	float	GetSpacing() const			{ return 0.0f; }

	hgeSprite *	GetSprite(hgeChar chr) const { return NULL; }
	float	GetPreWidth(hgeChar chr) const { return 0.0f; }
	float	GetPostWidth(hgeChar chr) const { return 0.0f; }
	float	GetHeight() const { return 0.0f; }
	float	GetStringWidth(hgeConstString string, bool bMultiline=true) const { return 0.0f; }

protected:
	static HGE	* m_hge;

	uint32_t	m_color;
	float		m_depth;
	int			m_blending;
	float		m_scale;

private:
	hgeFont();
	hgeFont(const hgeFont &fnt);
	hgeFont & operator = (const hgeFont &fnt);
};

#else // not HGE_UNICODE 

/*
** Legacy HGE Font class
*/
class hgeFont
{
public:
	hgeFont(hgeConstString filename, bool bMipmap=false);
	~hgeFont();

	void		Render(float x, float y, int align, hgeConstString string);
	void		printf(float x, float y, int align, hgeConstString format, ...);
	void		printfb(float x, float y, float w, float h, int align, hgeConstString format, ...);

	void		SetColor(uint32_t col);
	void		SetZ(float z);
	void		SetBlendMode(int blend);
	void		SetScale(float scale) {m_scale=scale;}
	void		SetProportion(float prop) { m_proportion=prop; }
	void		SetRotation(float rot) {m_rotation=rot;}
	void		SetTracking(float tracking) {m_tracking=tracking;}
	void		SetSpacing(float spacing) {m_spacing=spacing;}

	uint32_t	GetColor() const {return m_color;}
	float		GetZ() const {return m_depth;}
	int			GetBlendMode() const {return m_blending;}
	float		GetScale() const {return m_scale;}
	float		GetProportion() const { return m_proportion; }
	float		GetRotation() const {return m_rotation;}
	float		GetTracking() const {return m_tracking;}
	float		GetSpacing() const {return m_spacing;}

	hgeSprite*	GetSprite(hgeChar chr) const { return m_letters[(hgeChar)chr]; }
	float		GetPreWidth(hgeChar chr) const { return m_pre[(hgeChar)chr]; }
	float		GetPostWidth(hgeChar chr) const { return m_post[(hgeChar)chr]; }
	float		GetHeight() const { return m_height; }
	float		GetStringWidth(hgeConstString string, bool bMultiline=true) const;

private:
	hgeFont();
	hgeFont(const hgeFont &fnt);
	hgeFont & operator = (const hgeFont &fnt);

	hgeString _get_line( hgeString file, hgeString line );

	const static size_t HGEFONT_BUFFER_SZ = 1024;
	static hgeChar m_buffer[HGEFONT_BUFFER_SZ];

	static HGE	* m_hge;

	uint32_t	m_color;
	float		m_depth;
	int			m_blending;
	float		m_scale;
	HTEXTURE	m_texture;
	hgeSprite*	m_letters[256];
	float		m_pre[256];
	float		m_post[256];
	float		m_height;
	float		m_proportion;
	float		m_rotation;
	float		m_tracking;
	float		m_spacing;
};

#endif // HGE_UNICODE 
#endif // HGEFONT_H
