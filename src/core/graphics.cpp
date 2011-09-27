/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
** edited by kvakvs@yandex.ru, see https://github.com/kvakvs/hge
**
** Core functions implementation: graphics
*/


#include "hge_impl.h"
// GAPI dependent includes and defines (DX8/DX9 switch) by kvakvs@yandex.ru
#include "hge_gapi.h"


void HGE_CALL HGE_Impl::Gfx_Clear(uint32_t color)
{
	if (m_current_target)
	{
		if (m_current_target->pDepth)
			m_d3d_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
					color, 1.0f, 0);
		else
			m_d3d_device->Clear(0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0);
	}
	else
	{
		if (m_zbuffer_flag)
			m_d3d_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
					color, 1.0f, 0);
		else
			m_d3d_device->Clear(0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0);
	}
}

void HGE_CALL HGE_Impl::Gfx_SetClipping(int x, int y, int w, int h)
{
	hgeGAPIViewport vp;
	int scr_width, scr_height;

	if (!m_current_target)
	{
		scr_width = pHGE->System_GetStateInt(HGE_SCREENWIDTH);
		scr_height = pHGE->System_GetStateInt(HGE_SCREENHEIGHT);
	}
	else
	{
		scr_width = Texture_GetWidth((HTEXTURE) m_current_target->pTex);
		scr_height = Texture_GetHeight((HTEXTURE) m_current_target->pTex);
	}

	if (!w)
	{
		vp.X = 0;
		vp.Y = 0;
		vp.Width = scr_width;
		vp.Height = scr_height;
	}
	else
	{
		if (x < 0)
		{
			w += x;
			x = 0;
		}
		if (y < 0)
		{
			h += y;
			y = 0;
		}

		if (x + w > scr_width)
			w = scr_width - x;
		if (y + h > scr_height)
			h = scr_height - y;

		vp.X = x;
		vp.Y = y;
		vp.Width = w;
		vp.Height = h;
	}

	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	_render_batch();
	m_d3d_device->SetViewport(&vp);

	D3DXMATRIX tmp;
	D3DXMatrixScaling(&m_proj_matrix, 1.0f, -1.0f, 1.0f);
	D3DXMatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
	D3DXMatrixMultiply(&m_proj_matrix, &m_proj_matrix, &tmp);
	D3DXMatrixOrthoOffCenterLH(&tmp, (float) vp.X, (float) (vp.X + vp.Width),
			-((float) (vp.Y + vp.Height)), -((float) vp.Y), vp.MinZ, vp.MaxZ);
	D3DXMatrixMultiply(&m_proj_matrix, &m_proj_matrix, &tmp);
	m_d3d_device->SetTransform(D3DTS_PROJECTION, &m_proj_matrix);
}



void HGE_CALL HGE_Impl::Gfx_SetTransform(float x, float y, float dx, float dy,
		float rot, float hscale, float vscale)
{
	D3DXMATRIX tmp;

	if (vscale == 0.0f)
		D3DXMatrixIdentity(&m_view_matrix);
	else
	{
		D3DXMatrixTranslation(&m_view_matrix, -x, -y, 0.0f);
		D3DXMatrixScaling(&tmp, hscale, vscale, 1.0f);
		D3DXMatrixMultiply(&m_view_matrix, &m_view_matrix, &tmp);
		D3DXMatrixRotationZ(&tmp, -rot);
		D3DXMatrixMultiply(&m_view_matrix, &m_view_matrix, &tmp);
		D3DXMatrixTranslation(&tmp, x + dx, y + dy, 0.0f);
		D3DXMatrixMultiply(&m_view_matrix, &m_view_matrix, &tmp);
	}

	_render_batch();
	m_d3d_device->SetTransform(D3DTS_VIEW, &m_view_matrix);
}



bool HGE_CALL HGE_Impl::Gfx_BeginScene(HTARGET targ)
{
	hgeGAPISurface * pSurf = 0, *pDepth = 0;

	D3DDISPLAYMODE Mode;
	CRenderTargetList *target = (CRenderTargetList *) targ;

	HRESULT hr = m_d3d_device->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST)
		return false;
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		if (m_windowed_flag)
		{
			if (FAILED(m_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Mode))
					|| Mode.Format == D3DFMT_UNKNOWN)
			{
				_PostError(TXT("Can't determine desktop video mode"));
				return false;
			}

			m_d3d_pp_wnd.BackBufferFormat = Mode.Format;
			if (_format_id(Mode.Format) < 4)
				m_screen_color_depth = 16;
			else
				m_screen_color_depth = 32;
		}

		if (!_GfxRestore())
			return false;
	}

	if (m_vert_array)
	{
		_PostError(TXT("Gfx_BeginScene: Scene is already being rendered"));
		return false;
	}

	if (target != m_current_target)
	{
		if (target)
		{
			target->pTex->GetSurfaceLevel(0, &pSurf);
			pDepth = target->pDepth;
		}
		else
		{
			pSurf = m_screen_surf;
			pDepth = m_depth_surf;
		}
#if HGE_DIRECTX_VER == 8
		if(FAILED(m_d3d_device->SetRenderTarget(pSurf, pDepth)))
#endif
#if HGE_DIRECTX_VER == 9
		if(FAILED(m_d3d_device->SetRenderTarget(0, pSurf)))
#endif
		{
			if (target)
				pSurf->Release();
			_PostError(TXT("Gfx_BeginScene: Can't set render target"));
			return false;
		}
		if (target)
		{
			pSurf->Release();
			if (target->pDepth)
				m_d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			else
				m_d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			_SetProjectionMatrix(target->width, target->height);
		}
		else
		{
			if (m_zbuffer_flag)
				m_d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			else
				m_d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			_SetProjectionMatrix(m_screen_width, m_screen_height);
		}

		m_d3d_device->SetTransform(D3DTS_PROJECTION, &m_proj_matrix);
		D3DXMatrixIdentity(&m_view_matrix);
		m_d3d_device->SetTransform(D3DTS_VIEW, &m_view_matrix);

		m_current_target = target;
	}

	m_d3d_device->BeginScene();
#if HGE_DIRECTX_VER == 8
	m_vertex_buf->Lock( 0, 0, (uint8_t**)&m_vert_array, 0 );
#endif
#if HGE_DIRECTX_VER == 9
	m_vertex_buf->Lock( 0, 0, (VOID**)&m_vert_array, 0 );
#endif
	return true;
}



void HGE_CALL HGE_Impl::Gfx_EndScene()
{
	_render_batch(true);
	m_d3d_device->EndScene();
	if (!m_current_target)
		m_d3d_device->Present(NULL, NULL, NULL, NULL );
}



void HGE_CALL HGE_Impl::Gfx_RenderLine(float x1, float y1, float x2, float y2,
		uint32_t color, float z)
{
	if (m_vert_array)
	{
		if (m_current_prim_type != HGEPRIM_LINES || m_prim >= VERTEX_BUFFER_SIZE
				/ HGEPRIM_LINES || m_current_texture || m_current_blend_mode != BLEND_DEFAULT)
		{
			_render_batch();

			m_current_prim_type = HGEPRIM_LINES;
			if (m_current_blend_mode != BLEND_DEFAULT)
				_SetBlendMode(BLEND_DEFAULT);
			if (m_current_texture)
			{
				m_d3d_device->SetTexture(0, 0);
				m_current_texture = 0;
			}
		}

		int i = m_prim * HGEPRIM_LINES;
		m_vert_array[i].x = x1;
		m_vert_array[i + 1].x = x2;
		m_vert_array[i].y = y1;
		m_vert_array[i + 1].y = y2;
		m_vert_array[i].z = m_vert_array[i + 1].z = z;
		m_vert_array[i].col = m_vert_array[i + 1].col = color;
		m_vert_array[i].tx = m_vert_array[i + 1].tx = m_vert_array[i].ty = m_vert_array[i
				+ 1].ty = 0.0f;

		m_prim++;
	}
}



void HGE_CALL HGE_Impl::Gfx_RenderTriple(const hgeTriple *triple)
{
	if (m_vert_array)
	{
		if (m_current_prim_type != HGEPRIM_TRIPLES
				|| m_prim >= VERTEX_BUFFER_SIZE / HGEPRIM_TRIPLES
				|| m_current_texture != triple->tex
				|| m_current_blend_mode != triple->blend)
		{
			_render_batch();

			m_current_prim_type = HGEPRIM_TRIPLES;
			if (m_current_blend_mode != triple->blend)
				_SetBlendMode(triple->blend);
			if (triple->tex != m_current_texture)
			{
				m_d3d_device->SetTexture(0, (hgeGAPITexture *) triple->tex);
				m_current_texture = triple->tex;
			}
		}

		memcpy(&m_vert_array[m_prim * HGEPRIM_TRIPLES], triple->v, sizeof(hgeVertex) * HGEPRIM_TRIPLES);
		m_prim++;
	}
}



void HGE_CALL HGE_Impl::Gfx_RenderQuad(const hgeQuad *quad)
{
	if (m_vert_array)
	{
		if (m_current_prim_type != HGEPRIM_QUADS
				|| m_prim >= VERTEX_BUFFER_SIZE / HGEPRIM_QUADS
				|| m_current_texture != quad->tex
				|| m_current_blend_mode != quad->blend)
		{
			_render_batch();

			m_current_prim_type = HGEPRIM_QUADS;
			if (m_current_blend_mode != quad->blend)
				_SetBlendMode(quad->blend);
			if (quad->tex != m_current_texture)
			{
				m_d3d_device->SetTexture(0, (hgeGAPITexture *) quad->tex);
				m_current_texture = quad->tex;
			}
		}

		memcpy(&m_vert_array[m_prim * HGEPRIM_QUADS], quad->v, sizeof(hgeVertex) * HGEPRIM_QUADS);
		m_prim++;
	}
}



hgeVertex* HGE_CALL HGE_Impl::Gfx_StartBatch(int prim_type, HTEXTURE tex,
		int blend, int *max_prim)
{
	if (m_vert_array)
	{
		_render_batch();

		m_current_prim_type = prim_type;
		if (m_current_blend_mode != blend)
			_SetBlendMode(blend);
		if (tex != m_current_texture)
		{
			m_d3d_device->SetTexture(0, (hgeGAPITexture *) tex);
			m_current_texture = tex;
		}

		*max_prim = VERTEX_BUFFER_SIZE / prim_type;
		return m_vert_array;
	}
	else
		return 0;
}



void HGE_CALL HGE_Impl::Gfx_FinishBatch(int nprim)
{
	m_prim = nprim;
}



HTARGET HGE_CALL HGE_Impl::Target_Create(int width, int height, bool zbuffer)
{
	CRenderTargetList *pTarget;
	D3DSURFACE_DESC TDesc;

	pTarget = new CRenderTargetList;
	pTarget->pTex = 0;
	pTarget->pDepth = 0;

	if (FAILED(D3DXCreateTexture(m_d3d_device, width, height, 1, D3DUSAGE_RENDERTARGET,
					m_d3d_pp->BackBufferFormat, D3DPOOL_DEFAULT, &pTarget->pTex)))
	{
		_PostError(TXT("Can't create render target texture"));
		delete pTarget;
		return 0;
	}

	pTarget->pTex->GetLevelDesc(0, &TDesc);
	pTarget->width = TDesc.Width;
	pTarget->height = TDesc.Height;

	if (zbuffer)
	{
#if HGE_DIRECTX_VER == 8
		if(FAILED(m_d3d_device->CreateDepthStencilSurface(pTarget->width, pTarget->height,
								D3DFMT_D16, D3DMULTISAMPLE_NONE, &pTarget->pDepth)))
#endif
#if HGE_DIRECTX_VER == 9
		if(FAILED(m_d3d_device->CreateDepthStencilSurface(width, height,
								D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, false, &pTarget->pDepth, NULL)))
#endif
		{
			pTarget->pTex->Release();
			_PostError(TXT("Can't create render target depth buffer"));
			delete pTarget;
			return 0;
		}
	}

	pTarget->next = m_targets_list;
	m_targets_list = pTarget;

	return (HTARGET) pTarget;
}



void HGE_CALL HGE_Impl::Target_Free(HTARGET target)
{
	CRenderTargetList *pTarget = m_targets_list, *pPrevTarget = NULL;

	while (pTarget)
	{
		if ((CRenderTargetList *) target == pTarget)
		{
			if (pPrevTarget)
				pPrevTarget->next = pTarget->next;
			else
				m_targets_list = pTarget->next;

			if (pTarget->pTex)
				pTarget->pTex->Release();
			if (pTarget->pDepth)
				pTarget->pDepth->Release();

			delete pTarget;
			return;
		}

		pPrevTarget = pTarget;
		pTarget = pTarget->next;
	}
}



HTEXTURE HGE_CALL HGE_Impl::Target_GetTexture(HTARGET target)
{
	CRenderTargetList *targ = (CRenderTargetList *) target;
	if (target)
		return (HTEXTURE) targ->pTex;
	else
		return 0;
}



HTEXTURE HGE_CALL HGE_Impl::Texture_Create(int width, int height)
{
	hgeGAPITexture * pTex;

	if (FAILED( D3DXCreateTexture( m_d3d_device, width, height,
					1, // Mip levels
					0, // Usage
					D3DFMT_A8R8G8B8, // Format
					D3DPOOL_MANAGED, // Memory pool
					&pTex ) ))
	{
		_PostError(TXT("Can't create texture"));
		return NULL;
	}

	return (HTEXTURE) pTex;
}



HTEXTURE HGE_CALL HGE_Impl::Texture_Load(hgeConstString filename,
		uint32_t size, bool bMipmap)
{
	void *data;
	uint32_t _size;
	D3DFORMAT fmt1, fmt2;
	hgeGAPITexture * pTex;
	D3DXIMAGE_INFO info;
	CTextureList *texItem;

	if (size)
	{
		data = (void *) filename;
		_size = size;
	}
	else
	{
		data = pHGE->Resource_Load(filename, &_size);
		if (!data)
			return NULL;
	}

	if (*(uint32_t*) data == 0x20534444) // Compressed DDS format magic number
	{
		fmt1 = D3DFMT_UNKNOWN;
		fmt2 = D3DFMT_A8R8G8B8;
	}
	else
	{
		fmt1 = D3DFMT_A8R8G8B8;
		fmt2 = D3DFMT_UNKNOWN;
	}

	//  if( FAILED( D3DXCreateTextureFromFileInMemory( pD3DDevice, data, _size, &pTex ) ) ) pTex=NULL;
	if (FAILED( D3DXCreateTextureFromFileInMemoryEx( m_d3d_device, data, _size,
					D3DX_DEFAULT, D3DX_DEFAULT,
					bMipmap ? 0:1,     // Mip levels
					0,                 // Usage
					fmt1,              // Format
					D3DPOOL_MANAGED,   // Memory pool
					D3DX_FILTER_NONE,  // Filter
					D3DX_DEFAULT,      // Mip filter
					0,                 // Color key
					&info, NULL,
					&pTex ) ))

		if (FAILED( D3DXCreateTextureFromFileInMemoryEx( m_d3d_device, data, _size,
						D3DX_DEFAULT, D3DX_DEFAULT,
						bMipmap ? 0:1,    // Mip levels
						0,                // Usage
						fmt2,             // Format
						D3DPOOL_MANAGED,  // Memory pool
						D3DX_FILTER_NONE, // Filter
						D3DX_DEFAULT,     // Mip filter
						0,                // Color key
						&info, NULL,
						&pTex ) ))

		{
			_PostError(TXT("Can't create texture"));
			if (!size)
				Resource_Free(data);
			return NULL;
		}

	if (!size)
		Resource_Free(data);

	texItem = new CTextureList;
	texItem->tex = (HTEXTURE) pTex;
	texItem->width = info.Width;
	texItem->height = info.Height;
	texItem->next = m_texture_list;
	m_texture_list = texItem;

	return (HTEXTURE) pTex;
}



void HGE_CALL HGE_Impl::Texture_Free(HTEXTURE tex)
{
	hgeGAPITexture * pTex = (hgeGAPITexture *) tex;
	CTextureList *texItem = m_texture_list, *texPrev = 0;

	while (texItem)
	{
		if (texItem->tex == tex)
		{
			if (texPrev)
				texPrev->next = texItem->next;
			else
				m_texture_list = texItem->next;
			delete texItem;
			break;
		}
		texPrev = texItem;
		texItem = texItem->next;
	}
	if (pTex != NULL)
		pTex->Release();
}



int HGE_CALL HGE_Impl::Texture_GetWidth(HTEXTURE tex, bool bOriginal)
{
	D3DSURFACE_DESC TDesc;
	hgeGAPITexture * pTex = (hgeGAPITexture *) tex;
	CTextureList *texItem = m_texture_list;

	if (bOriginal)
	{
		while (texItem)
		{
			if (texItem->tex == tex)
				return texItem->width;
			texItem = texItem->next;
		}
		return 0;
	}
	else
	{
		if (FAILED(pTex->GetLevelDesc(0, &TDesc)))
			return 0;
		else
			return TDesc.Width;
	}
}



int HGE_CALL HGE_Impl::Texture_GetHeight(HTEXTURE tex, bool bOriginal)
{
	D3DSURFACE_DESC TDesc;
	hgeGAPITexture * pTex = (hgeGAPITexture *) tex;
	CTextureList *texItem = m_texture_list;

	if (bOriginal)
	{
		while (texItem)
		{
			if (texItem->tex == tex)
				return texItem->height;
			texItem = texItem->next;
		}
		return 0;
	}
	else
	{
		if (FAILED(pTex->GetLevelDesc(0, &TDesc)))
			return 0;
		else
			return TDesc.Height;
	}
}



uint32_t * HGE_CALL HGE_Impl::Texture_Lock(HTEXTURE tex, bool bReadOnly,
		int left, int top, int width, int height)
{
	hgeGAPITexture * pTex = (hgeGAPITexture *) tex;
	D3DSURFACE_DESC TDesc;
	D3DLOCKED_RECT TRect;
	RECT region, *prec;
	int flags;

	pTex->GetLevelDesc(0, &TDesc);
	if (TDesc.Format != D3DFMT_A8R8G8B8 && TDesc.Format != D3DFMT_X8R8G8B8)
		return 0;

	if (width && height)
	{
		region.left = left;
		region.top = top;
		region.right = left + width;
		region.bottom = top + height;
		prec = &region;
	}
	else
		prec = 0;

	if (bReadOnly)
		flags = D3DLOCK_READONLY;
	else
		flags = 0;

	if (FAILED(pTex->LockRect(0, &TRect, prec, flags)))
	{
		_PostError(TXT("Can't lock texture"));
		return 0;
	}

	return (uint32_t *) TRect.pBits;
}



void HGE_CALL HGE_Impl::Texture_Unlock(HTEXTURE tex)
{
	hgeGAPITexture * pTex = (hgeGAPITexture *) tex;
	pTex->UnlockRect(0);
}


//////// Implementation ////////


void HGE_Impl::_render_batch(bool bEndScene)
{
	if (m_vert_array)
	{
		m_vertex_buf->Unlock();

		if (m_prim)
		{
			switch (m_current_prim_type)
			{
			case HGEPRIM_QUADS:
#if HGE_DIRECTX_VER == 8
				m_d3d_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_prim<<2, 0, m_prim<<1);
#endif
#if HGE_DIRECTX_VER == 9
				m_d3d_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_prim<<2, 0, m_prim<<1);
#endif
				break;

			case HGEPRIM_TRIPLES:
				m_d3d_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_prim);
				break;

			case HGEPRIM_LINES:
				m_d3d_device->DrawPrimitive(D3DPT_LINELIST, 0, m_prim);
				break;
			}

			m_prim = 0;
		}

		if (bEndScene)
			m_vert_array = 0;
#if HGE_DIRECTX_VER == 8
		else m_vertex_buf->Lock( 0, 0, (uint8_t**)&m_vert_array, 0 );
#endif
#if HGE_DIRECTX_VER == 9
		else m_vertex_buf->Lock( 0, 0, (VOID**)&m_vert_array, 0 );
#endif
	}
}



void HGE_Impl::_SetBlendMode(int blend)
{
	if ((blend & BLEND_ALPHABLEND) != (m_current_blend_mode & BLEND_ALPHABLEND))
	{
		if (blend & BLEND_ALPHABLEND)
			m_d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		else
			m_d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

	if ((blend & BLEND_ZWRITE) != (m_current_blend_mode & BLEND_ZWRITE))
	{
		if (blend & BLEND_ZWRITE)
			m_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		else
			m_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}

	if ((blend & BLEND_COLORADD) != (m_current_blend_mode & BLEND_COLORADD))
	{
		if (blend & BLEND_COLORADD)
			m_d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
		else
			m_d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	}

	m_current_blend_mode = blend;
}



void HGE_Impl::_SetProjectionMatrix(int width, int height)
{
    D3DXMATRIX tmp;
    D3DXMatrixScaling(&m_proj_matrix, 1.0f, -1.0f, 1.0f);
    D3DXMatrixTranslation(&tmp, -0.5f, height+0.5f, 0.0f);
    D3DXMatrixMultiply(&m_proj_matrix, &m_proj_matrix, &tmp);
    D3DXMatrixOrthoOffCenterLH(&tmp, 0, (float)width, 0, (float)height, 0.0f, 1.0f);
    D3DXMatrixMultiply(&m_proj_matrix, &m_proj_matrix, &tmp);
}



bool HGE_Impl::_GfxInit()
{
	static const char *szFormats[] =
		{ "UNKNOWN", "R5G6B5", "X1R5G5B5", "A1R5G5B5", "X8R8G8B8", "A8R8G8B8" };
	hgeGAPIAdapterIdentifier AdID;
	D3DDISPLAYMODE Mode;
	D3DFORMAT Format = D3DFMT_UNKNOWN;
	UINT nModes, i;

	// Init D3D
#if HGE_DIRECTX_VER == 8
	m_d3d=Direct3DCreate8(120); // D3D_SDK_VERSION
#endif
#if HGE_DIRECTX_VER == 9
	m_d3d=Direct3DCreate9(D3D_SDK_VERSION); // D3D_SDK_VERSION
#endif
	if (m_d3d == NULL)
	{
		_PostError(TXT("Can't create D3D interface"));
		return false;
	}

	// Get adapter info

#if HGE_DIRECTX_VER == 8
	m_d3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, D3DENUM_NO_WHQL_LEVEL, &AdID);
#endif
#if HGE_DIRECTX_VER == 9
	m_d3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &AdID);
#endif
	System_Log(TXT("D3D Driver: %s"), AdID.Driver);
	System_Log(TXT("Description: %s"), AdID.Description);
	System_Log(TXT("Version: %d.%d.%d.%d"),
			HIWORD(AdID.DriverVersion.HighPart),
			LOWORD(AdID.DriverVersion.HighPart),
			HIWORD(AdID.DriverVersion.LowPart),
			LOWORD(AdID.DriverVersion.LowPart));

	// Set up Windowed presentation parameters

	if (FAILED(m_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Mode))
			|| Mode.Format == D3DFMT_UNKNOWN)
	{
		_PostError(TXT("Can't determine desktop video mode"));
		if (m_windowed_flag)
			return false;
	}

	ZeroMemory(&m_d3d_pp_wnd, sizeof(m_d3d_pp_wnd));

	m_d3d_pp_wnd.BackBufferWidth = m_screen_width;
	m_d3d_pp_wnd.BackBufferHeight = m_screen_height;
	m_d3d_pp_wnd.BackBufferFormat = Mode.Format;
	m_d3d_pp_wnd.BackBufferCount = 1;
	m_d3d_pp_wnd.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_d3d_pp_wnd.hDeviceWindow = m_hwnd;
	m_d3d_pp_wnd.Windowed = TRUE;

#if HGE_DIRECTX_VER == 8
	if(m_framerate_limit==HGEFPS_VSYNC) m_d3d_pp_wnd.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
	else m_d3d_pp_wnd.SwapEffect = D3DSWAPEFFECT_COPY;
#endif
#if HGE_DIRECTX_VER == 9
	if(m_framerate_limit==HGEFPS_VSYNC)
	{
		m_d3d_pp_wnd.SwapEffect = D3DSWAPEFFECT_COPY;
		m_d3d_pp_wnd.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	}
	else
	{
		m_d3d_pp_wnd.SwapEffect = D3DSWAPEFFECT_COPY;
		m_d3d_pp_wnd.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
#endif

	if (m_zbuffer_flag)
	{
		m_d3d_pp_wnd.EnableAutoDepthStencil = TRUE;
		m_d3d_pp_wnd.AutoDepthStencilFormat = D3DFMT_D16;
	}

	// Set up Full Screen presentation parameters

#if HGE_DIRECTX_VER == 8
	nModes=m_d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT);
#endif
#if HGE_DIRECTX_VER == 9
	nModes=m_d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, Mode.Format);
#endif

	for (i = 0; i < nModes; i++)
	{
#if HGE_DIRECTX_VER == 8
		m_d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, i, &Mode);
#endif
#if HGE_DIRECTX_VER == 9
		m_d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, Mode.Format, i, &Mode);
#endif

		if (Mode.Width != (UINT) m_screen_width || Mode.Height
				!= (UINT) m_screen_height)
			continue;
		if (m_screen_color_depth == 16 && (_format_id(Mode.Format) > _format_id(
				D3DFMT_A1R5G5B5)))
			continue;
		if (_format_id(Mode.Format) > _format_id(Format))
			Format = Mode.Format;
	}

	if (Format == D3DFMT_UNKNOWN)
	{
		_PostError(TXT("Can't find appropriate full screen video mode"));
		if (!m_windowed_flag)
			return false;
	}

	ZeroMemory(&m_d3d_pp_fullscreen, sizeof(m_d3d_pp_fullscreen));

	m_d3d_pp_fullscreen.BackBufferWidth = m_screen_width;
	m_d3d_pp_fullscreen.BackBufferHeight = m_screen_height;
	m_d3d_pp_fullscreen.BackBufferFormat = Format;
	m_d3d_pp_fullscreen.BackBufferCount = 1;
	m_d3d_pp_fullscreen.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_d3d_pp_fullscreen.hDeviceWindow = m_hwnd;
	m_d3d_pp_fullscreen.Windowed = FALSE;

	m_d3d_pp_fullscreen.SwapEffect = D3DSWAPEFFECT_FLIP;
	m_d3d_pp_fullscreen.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

#if HGE_DIRECTX_VER == 8
	if(m_framerate_limit==HGEFPS_VSYNC) m_d3d_pp_fullscreen.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	else m_d3d_pp_fullscreen.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif
#if HGE_DIRECTX_VER == 9
	if(m_framerate_limit==HGEFPS_VSYNC) m_d3d_pp_fullscreen.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	else m_d3d_pp_fullscreen.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif
	if (m_zbuffer_flag)
	{
		m_d3d_pp_fullscreen.EnableAutoDepthStencil = TRUE;
		m_d3d_pp_fullscreen.AutoDepthStencilFormat = D3DFMT_D16;
	}

	m_d3d_pp = m_windowed_flag ? &m_d3d_pp_wnd : &m_d3d_pp_fullscreen;

	if (_format_id(m_d3d_pp->BackBufferFormat) < 4)
		m_screen_color_depth = 16;
	else
		m_screen_color_depth = 32;

	// Create D3D Device

	if (FAILED( m_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					m_d3d_pp, &m_d3d_device ) ))
	{
		_PostError(TXT("Can't create D3D device"));
		return false;
	}

	_AdjustWindow();

	System_Log(TXT("Mode: %d x %d x %s\n"), m_screen_width, m_screen_height,
			szFormats[_format_id(Format)]);

	// Create vertex batch buffer

	m_vert_array = 0;
	m_texture_list = 0;

	// Init all stuff that can be lost

	_SetProjectionMatrix(m_screen_width, m_screen_height);
	D3DXMatrixIdentity(&m_view_matrix);

	if (!_init_lost())
		return false;

	Gfx_Clear(0);

	return true;
}



int HGE_Impl::_format_id(D3DFORMAT fmt)
{
	switch (fmt)
	{
	case D3DFMT_R5G6B5:
		return 1;
	case D3DFMT_X1R5G5B5:
		return 2;
	case D3DFMT_A1R5G5B5:
		return 3;
	case D3DFMT_X8R8G8B8:
		return 4;
	case D3DFMT_A8R8G8B8:
		return 5;
	default:
		return 0;
	}
}



void HGE_Impl::_AdjustWindow()
{
	RECT *rc;
	LONG style;

	if (m_windowed_flag)
	{
		rc = &m_rect_wnd;
		style = m_style_wnd;
	}
	else
	{
		rc = &m_rect_fullscreen;
		style = m_style_fullscreen;
	}
	SetWindowLong(m_hwnd, GWL_STYLE, style);

	style = GetWindowLong(m_hwnd, GWL_EXSTYLE);
	if (m_windowed_flag)
	{
		SetWindowLong(m_hwnd, GWL_EXSTYLE, style & (~WS_EX_TOPMOST));
		SetWindowPos(m_hwnd, HWND_NOTOPMOST, rc->left, rc->top, rc->right
				- rc->left, rc->bottom - rc->top, SWP_FRAMECHANGED);
	}
	else
	{
		SetWindowLong(m_hwnd, GWL_EXSTYLE, style | WS_EX_TOPMOST);
		SetWindowPos(m_hwnd, HWND_TOPMOST, rc->left, rc->top, rc->right
				- rc->left, rc->bottom - rc->top, SWP_FRAMECHANGED);
	}
}



void HGE_Impl::_Resize(int width, int height)
{
	if (m_parent_hwnd)
	{
		//if(procFocusLostFunc) procFocusLostFunc();

		m_d3d_pp_wnd.BackBufferWidth = width;
		m_d3d_pp_wnd.BackBufferHeight = height;
		m_screen_width = width;
		m_screen_height = height;

		_SetProjectionMatrix(m_screen_width, m_screen_height);
		_GfxRestore();

		//if(procFocusGainFunc) procFocusGainFunc();
	}
}



void HGE_Impl::_GfxDone()
{
	CRenderTargetList *target = m_targets_list, *next_target;

	while (m_texture_list)
		Texture_Free(m_texture_list->tex);

	if (m_screen_surf)
	{
		m_screen_surf->Release();
		m_screen_surf = 0;
	}
	if (m_depth_surf)
	{
		m_depth_surf->Release();
		m_depth_surf = 0;
	}

	while (target)
	{
		if (target->pTex)
			target->pTex->Release();
		if (target->pDepth)
			target->pDepth->Release();
		next_target = target->next;
		delete target;
		target = next_target;
	}
	m_targets_list = 0;

	if (m_index_buf)
	{
#if HGE_DIRECTX_VER == 8
		m_d3d_device->SetIndices(NULL,0);
#endif
#if HGE_DIRECTX_VER == 9
		m_d3d_device->SetIndices(NULL);
#endif
		m_index_buf->Release();
		m_index_buf = 0;
	}
	if (m_vertex_buf)
	{
		if (m_vert_array)
		{
			m_vertex_buf->Unlock();
			m_vert_array = 0;
		}
#if HGE_DIRECTX_VER == 8
		m_d3d_device->SetStreamSource( 0, NULL, sizeof(hgeVertex) );
#endif
#if HGE_DIRECTX_VER == 9
		m_d3d_device->SetStreamSource( 0, NULL, 0, sizeof(hgeVertex) );
#endif
		m_vertex_buf->Release();
		m_vertex_buf = 0;
	}
	if (m_d3d_device)
	{
		m_d3d_device->Release();
		m_d3d_device = 0;
	}
	if (m_d3d)
	{
		m_d3d->Release();
		m_d3d = 0;
	}
}



bool HGE_Impl::_GfxRestore()
{
	CRenderTargetList *target = m_targets_list;

	//if(!pD3DDevice) return false;
	//if(pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICELOST) return;

	if (m_screen_surf)
		m_screen_surf->Release();
	if (m_depth_surf)
		m_depth_surf->Release();

	while (target)
	{
		if (target->pTex)
			target->pTex->Release();
		if (target->pDepth)
			target->pDepth->Release();
		target = target->next;
	}

	if (m_index_buf)
	{
#if HGE_DIRECTX_VER == 8
		m_d3d_device->SetIndices(NULL,0);
#endif
#if HGE_DIRECTX_VER == 9
		m_d3d_device->SetIndices(NULL);
#endif
		m_index_buf->Release();
	}
	if (m_vertex_buf)
	{
#if HGE_DIRECTX_VER == 8
		m_d3d_device->SetStreamSource( 0, NULL, sizeof(hgeVertex) );
#endif
#if HGE_DIRECTX_VER == 9
		m_d3d_device->SetStreamSource( 0, NULL,0, sizeof(hgeVertex) );
#endif
		m_vertex_buf->Release();
	}

	m_d3d_device->Reset(m_d3d_pp);

	if (!_init_lost())
		return false;

	if (m_gfx_restore_func)
		return m_gfx_restore_func();

	return true;
}



bool HGE_Impl::_init_lost()
{
	CRenderTargetList *target = m_targets_list;

	// Store render target

	m_screen_surf = 0;
	m_depth_surf = 0;

#if HGE_DIRECTX_VER == 8
	m_d3d_device->GetRenderTarget(&m_screen_surf);
#endif
#if HGE_DIRECTX_VER == 9
	m_d3d_device->GetRenderTarget(0, &m_screen_surf);
#endif
	m_d3d_device->GetDepthStencilSurface(&m_depth_surf);

	while (target)
	{
		if (target->pTex)
			D3DXCreateTexture(m_d3d_device, target->width, target->height, 1,
					D3DUSAGE_RENDERTARGET, m_d3d_pp->BackBufferFormat,
					D3DPOOL_DEFAULT, &target->pTex);
		if (target->pDepth)
#if HGE_DIRECTX_VER == 8
			m_d3d_device->CreateDepthStencilSurface(target->width, target->height,
					D3DFMT_D16, D3DMULTISAMPLE_NONE, &target->pDepth);
#endif
#if HGE_DIRECTX_VER == 9
			m_d3d_device->CreateDepthStencilSurface(target->width, target->height,
					D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, false, &target->pDepth, NULL);
#endif
			target = target->next;
	}

	// Create Vertex buffer
#if HGE_DIRECTX_VER == 8
	if( FAILED (m_d3d_device->CreateVertexBuffer(VERTEX_BUFFER_SIZE*sizeof(hgeVertex),
				D3DUSAGE_WRITEONLY, D3DFVF_HGEVERTEX, D3DPOOL_DEFAULT, &m_vertex_buf )))
#endif
#if HGE_DIRECTX_VER == 9
	if( FAILED (m_d3d_device->CreateVertexBuffer(VERTEX_BUFFER_SIZE*sizeof(hgeVertex),
				D3DUSAGE_WRITEONLY, D3DFVF_HGEVERTEX, D3DPOOL_DEFAULT, &m_vertex_buf, NULL)))
#endif
	{
		_PostError(TXT("Can't create D3D vertex buffer"));
		return false;
	}

#if HGE_DIRECTX_VER == 8
	m_d3d_device->SetVertexShader( D3DFVF_HGEVERTEX );
	m_d3d_device->SetStreamSource( 0, m_vertex_buf, sizeof(hgeVertex) );
#endif
#if HGE_DIRECTX_VER == 9
	m_d3d_device->SetVertexShader( NULL );
	m_d3d_device->SetFVF( D3DFVF_HGEVERTEX );
	m_d3d_device->SetStreamSource( 0, m_vertex_buf, 0, sizeof(hgeVertex) );
#endif

	// Create and setup Index buffer

#if HGE_DIRECTX_VER == 8
	if( FAILED( m_d3d_device->CreateIndexBuffer(VERTEX_BUFFER_SIZE*6/4*sizeof(uint16_t),
			D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_index_buf ) ) )
#endif
#if HGE_DIRECTX_VER == 9
	if( FAILED( m_d3d_device->CreateIndexBuffer(VERTEX_BUFFER_SIZE*6/4*sizeof(uint16_t),
			D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_index_buf, NULL) ) )
#endif
	{
		_PostError(TXT("Can't create D3D index buffer"));
		return false;
	}

	uint16_t *pIndices, n = 0;
#if HGE_DIRECTX_VER == 8
	if( FAILED( m_index_buf->Lock( 0, 0, (uint8_t**)&pIndices, 0 ) ) )
#endif
#if HGE_DIRECTX_VER == 9
	if( FAILED( m_index_buf->Lock( 0, 0, (VOID**)&pIndices, 0 ) ) )
#endif
	{
		_PostError(TXT("Can't lock D3D index buffer"));
		return false;
	}

	for (int i = 0; i < VERTEX_BUFFER_SIZE / 4; i++)
	{
		*pIndices++ = n;
		*pIndices++ = n + 1;
		*pIndices++ = n + 2;
		*pIndices++ = n + 2;
		*pIndices++ = n + 3;
		*pIndices++ = n;
		n += 4;
	}

	m_index_buf->Unlock();
#if HGE_DIRECTX_VER == 8
	m_d3d_device->SetIndices(m_index_buf,0);
#endif
#if HGE_DIRECTX_VER == 9
	m_d3d_device->SetIndices(m_index_buf);
#endif

	// Set common render states

	//pD3DDevice->SetRenderState( D3DRS_LASTPIXEL, FALSE );
	m_d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_d3d_device->SetRenderState(D3DRS_LIGHTING, FALSE );

	m_d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
	m_d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE );
	m_d3d_device->SetRenderState(D3DRS_ALPHAREF, 0x01);
	m_d3d_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	m_d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	m_d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

#if HGE_DIRECTX_VER == 8
	m_d3d_device->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	if(m_texture_filter_flag)
	{
		m_d3d_device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
		m_d3d_device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	}
	else
	{
		m_d3d_device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
		m_d3d_device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	}
#endif
#if HGE_DIRECTX_VER == 9
	m_d3d_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	if(m_texture_filter_flag)
	{
		m_d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	}
	else
	{
		m_d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		m_d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	}
#endif
	m_prim = 0;
	m_current_prim_type = HGEPRIM_QUADS;
	m_current_blend_mode = BLEND_DEFAULT;
	m_current_texture = NULL;
#if HGE_DIRECTX_VER >= 9
	m_current_shader = NULL;
#endif

	m_d3d_device->SetTransform(D3DTS_VIEW, &m_view_matrix);
	m_d3d_device->SetTransform(D3DTS_PROJECTION, &m_proj_matrix);

	return true;
}



#if HGE_DIRECTX_VER >= 9
HSHADER HGE_CALL HGE_Impl::Shader_Create(hgeConstString filename)
{
	LPD3DXBUFFER code = NULL;
	LPDIRECT3DPIXELSHADER9 pixelShader = NULL;
	HRESULT result = HGE_WINAPI_UNICODE_SUFFIX(D3DXCompileShaderFromFile)(
			filename,    //filepath
			NULL,        //macro's
			NULL,        //includes
			"ps_main",   //main function
			"ps_2_0",    //shader profile
			0,           //flags
			&code,       //compiled operations
			NULL,        //errors
			NULL);       //constants
	if(FAILED(result))
	{
		_PostError( TXT("Can't create shader") );
		return NULL;
	}

	m_d3d_device->CreatePixelShader((DWORD *)code->GetBufferPointer(), &pixelShader);
	code->Release();
	return (HSHADER)pixelShader;
}
#endif



#if HGE_DIRECTX_VER >= 9
void HGE_CALL HGE_Impl::Gfx_SetShader(HSHADER shader)
{
	if (m_current_shader != shader)
	{
		_render_batch();
		m_current_shader = shader;
		m_d3d_device->SetPixelShader((LPDIRECT3DPIXELSHADER9)shader);
	}
}
#endif



#if HGE_DIRECTX_VER >= 9
void HGE_CALL HGE_Impl::Shader_Free(HSHADER shader)
{
	((LPDIRECT3DPIXELSHADER9)shader)->Release();
}
#endif
