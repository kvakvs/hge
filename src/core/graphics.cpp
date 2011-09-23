/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: graphics
*/


#include "hge_impl.h"
#include <d3d8.h>
#include <d3dx8.h>


void CALL HGE_Impl::Gfx_Clear(DWORD color)
{
	if(pCurTarget)
	{
		if(pCurTarget->pDepth)
			pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0 );
		else
			pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );
	}
	else
	{
		if(bZBuffer)
			pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0 );
		else
			pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );
	}
}

void CALL HGE_Impl::Gfx_SetClipping(int x, int y, int w, int h)
{
	D3DVIEWPORT8 vp;
	int scr_width, scr_height;

	if(!pCurTarget) {
		scr_width=pHGE->System_GetStateInt(HGE_SCREENWIDTH);
		scr_height=pHGE->System_GetStateInt(HGE_SCREENHEIGHT);
	}
	else {
		scr_width=Texture_GetWidth((HTEXTURE)pCurTarget->pTex);
		scr_height=Texture_GetHeight((HTEXTURE)pCurTarget->pTex);
	}

	if(!w) {
		vp.X=0;
		vp.Y=0;
		vp.Width=scr_width;
		vp.Height=scr_height;
	}
	else
	{
		if(x<0) { w+=x; x=0; }
		if(y<0) { h+=y; y=0; }

		if(x+w > scr_width) w=scr_width-x;
		if(y+h > scr_height) h=scr_height-y;

		vp.X=x;
		vp.Y=y;
		vp.Width=w;
		vp.Height=h;
	}

	vp.MinZ=0.0f;
	vp.MaxZ=1.0f;

	_render_batch();
	pD3DDevice->SetViewport(&vp);

	D3DXMATRIX tmp;
	D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
	D3DXMatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
	D3DXMatrixMultiply(&matProj, &matProj, &tmp);
	D3DXMatrixOrthoOffCenterLH(&tmp, (float)vp.X, (float)(vp.X+vp.Width), -((float)(vp.Y+vp.Height)), -((float)vp.Y), vp.MinZ, vp.MaxZ);
	D3DXMatrixMultiply(&matProj, &matProj, &tmp);
	pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

void CALL HGE_Impl::Gfx_SetTransform(float x, float y, float dx, float dy, float rot, float hscale, float vscale)
{
	D3DXMATRIX tmp;

	if(vscale==0.0f) D3DXMatrixIdentity(&matView);
	else
	{
		D3DXMatrixTranslation(&matView, -x, -y, 0.0f);
		D3DXMatrixScaling(&tmp, hscale, vscale, 1.0f);
		D3DXMatrixMultiply(&matView, &matView, &tmp);
		D3DXMatrixRotationZ(&tmp, -rot);
		D3DXMatrixMultiply(&matView, &matView, &tmp);
		D3DXMatrixTranslation(&tmp, x+dx, y+dy, 0.0f);
		D3DXMatrixMultiply(&matView, &matView, &tmp);
	}

	_render_batch();
	pD3DDevice->SetTransform(D3DTS_VIEW, &matView);
}

bool CALL HGE_Impl::Gfx_BeginScene(HTARGET targ)
{
	LPDIRECT3DSURFACE8 pSurf=0, pDepth=0;
	D3DDISPLAYMODE Mode;
	CRenderTargetList *target=(CRenderTargetList *)targ;

	HRESULT hr = pD3DDevice->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST) return false;
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		if(bWindowed)
		{
			if(FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Mode)) || Mode.Format==D3DFMT_UNKNOWN) 
			{
				_PostError("Can't determine desktop video mode");
				return false;
			}

			d3dppW.BackBufferFormat = Mode.Format;
			if(_format_id(Mode.Format) < 4) nScreenBPP=16;
			else nScreenBPP=32;
		}

	    if(!_GfxRestore()) return false; 
	}
    
	if(VertArray)
	{
		_PostError("Gfx_BeginScene: Scene is already being rendered");
		return false;
	}
	
	if(target != pCurTarget)
	{
		if(target)
		{
			target->pTex->GetSurfaceLevel(0, &pSurf);
			pDepth=target->pDepth;
		}
		else
		{
			pSurf=pScreenSurf;
			pDepth=pScreenDepth;
		}
		if(FAILED(pD3DDevice->SetRenderTarget(pSurf, pDepth)))
		{
			if(target) pSurf->Release();
			_PostError("Gfx_BeginScene: Can't set render target");
			return false;
		}
		if(target)
		{
			pSurf->Release();
			if(target->pDepth) pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE ); 
			else pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE ); 
			_SetProjectionMatrix(target->width, target->height);
		}
		else
		{
			if(bZBuffer) pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE ); 
			else pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
			_SetProjectionMatrix(nScreenWidth, nScreenHeight);
		}

		pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
		D3DXMatrixIdentity(&matView);
		pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

		pCurTarget=target;
	}

	pD3DDevice->BeginScene();
	pVB->Lock( 0, 0, (BYTE**)&VertArray, 0 );

	return true;
}

void CALL HGE_Impl::Gfx_EndScene()
{
	_render_batch(true);
	pD3DDevice->EndScene();
	if(!pCurTarget) pD3DDevice->Present( NULL, NULL, NULL, NULL );
}

void CALL HGE_Impl::Gfx_RenderLine(float x1, float y1, float x2, float y2, DWORD color, float z)
{
	if(VertArray)
	{
		if(CurPrimType!=HGEPRIM_LINES || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_LINES || CurTexture || CurBlendMode!=BLEND_DEFAULT)
		{
			_render_batch();

			CurPrimType=HGEPRIM_LINES;
			if(CurBlendMode != BLEND_DEFAULT) _SetBlendMode(BLEND_DEFAULT);
			if(CurTexture) { pD3DDevice->SetTexture(0, 0); CurTexture=0; }
		}

		int i=nPrim*HGEPRIM_LINES;
		VertArray[i].x = x1; VertArray[i+1].x = x2;
		VertArray[i].y = y1; VertArray[i+1].y = y2;
		VertArray[i].z     = VertArray[i+1].z = z;
		VertArray[i].col   = VertArray[i+1].col = color;
		VertArray[i].tx    = VertArray[i+1].tx =
		VertArray[i].ty    = VertArray[i+1].ty = 0.0f;

		nPrim++;
	}
}

void CALL HGE_Impl::Gfx_RenderTriple(const hgeTriple *triple)
{
	if(VertArray)
	{
		if(CurPrimType!=HGEPRIM_TRIPLES || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_TRIPLES || CurTexture!=triple->tex || CurBlendMode!=triple->blend)
		{
			_render_batch();

			CurPrimType=HGEPRIM_TRIPLES;
			if(CurBlendMode != triple->blend) _SetBlendMode(triple->blend);
			if(triple->tex != CurTexture) {
				pD3DDevice->SetTexture( 0, (LPDIRECT3DTEXTURE8)triple->tex );
				CurTexture = triple->tex;
			}
		}

		memcpy(&VertArray[nPrim*HGEPRIM_TRIPLES], triple->v, sizeof(hgeVertex)*HGEPRIM_TRIPLES);
		nPrim++;
	}
}

void CALL HGE_Impl::Gfx_RenderQuad(const hgeQuad *quad)
{
	if(VertArray)
	{
		if(CurPrimType!=HGEPRIM_QUADS || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_QUADS || CurTexture!=quad->tex || CurBlendMode!=quad->blend)
		{
			_render_batch();

			CurPrimType=HGEPRIM_QUADS;
			if(CurBlendMode != quad->blend) _SetBlendMode(quad->blend);
			if(quad->tex != CurTexture)
			{
				pD3DDevice->SetTexture( 0, (LPDIRECT3DTEXTURE8)quad->tex );
				CurTexture = quad->tex;
			}
		}

		memcpy(&VertArray[nPrim*HGEPRIM_QUADS], quad->v, sizeof(hgeVertex)*HGEPRIM_QUADS);
		nPrim++;
	}
}

hgeVertex* CALL HGE_Impl::Gfx_StartBatch(int prim_type, HTEXTURE tex, int blend, int *max_prim)
{
	if(VertArray)
	{
		_render_batch();

		CurPrimType=prim_type;
		if(CurBlendMode != blend) _SetBlendMode(blend);
		if(tex != CurTexture)
		{
			pD3DDevice->SetTexture( 0, (LPDIRECT3DTEXTURE8)tex );
			CurTexture = tex;
		}

		*max_prim=VERTEX_BUFFER_SIZE / prim_type;
		return VertArray;
	}
	else return 0;
}

void CALL HGE_Impl::Gfx_FinishBatch(int nprim)
{
	nPrim=nprim;
}

HTARGET CALL HGE_Impl::Target_Create(int width, int height, bool zbuffer)
{
	CRenderTargetList *pTarget;
	D3DSURFACE_DESC TDesc;

	pTarget = new CRenderTargetList;
	pTarget->pTex=0;
	pTarget->pDepth=0;

	if(FAILED(D3DXCreateTexture(pD3DDevice, width, height, 1, D3DUSAGE_RENDERTARGET,
						d3dpp->BackBufferFormat, D3DPOOL_DEFAULT, &pTarget->pTex)))
	{
		_PostError("Can't create render target texture");
		delete pTarget;
		return 0;
	}

	pTarget->pTex->GetLevelDesc(0, &TDesc);
	pTarget->width=TDesc.Width;
	pTarget->height=TDesc.Height;

	if(zbuffer)
	{
		if(FAILED(pD3DDevice->CreateDepthStencilSurface(pTarget->width, pTarget->height,
						D3DFMT_D16, D3DMULTISAMPLE_NONE, &pTarget->pDepth)))
		{   
			pTarget->pTex->Release();
			_PostError("Can't create render target depth buffer");
			delete pTarget;
			return 0;
		}
	}

	pTarget->next=pTargets;
	pTargets=pTarget;

	return (HTARGET)pTarget;
}

void CALL HGE_Impl::Target_Free(HTARGET target)
{
	CRenderTargetList *pTarget=pTargets, *pPrevTarget=NULL;

	while(pTarget)
	{
		if((CRenderTargetList *)target == pTarget)
		{
			if(pPrevTarget)
				pPrevTarget->next = pTarget->next;
			else
				pTargets = pTarget->next;

			if(pTarget->pTex) pTarget->pTex->Release();
			if(pTarget->pDepth) pTarget->pDepth->Release();

			delete pTarget;
			return;
		}

		pPrevTarget = pTarget;
		pTarget = pTarget->next;
	}
}

HTEXTURE CALL HGE_Impl::Target_GetTexture(HTARGET target)
{
	CRenderTargetList *targ=(CRenderTargetList *)target;
	if(target) return (HTEXTURE)targ->pTex;
	else return 0;
}

HTEXTURE CALL HGE_Impl::Texture_Create(int width, int height)
{
	LPDIRECT3DTEXTURE8 pTex;

	if( FAILED( D3DXCreateTexture( pD3DDevice, width, height,
										1,					// Mip levels
										0,					// Usage
										D3DFMT_A8R8G8B8,	// Format
										D3DPOOL_MANAGED,	// Memory pool
										&pTex ) ) )
	{	
		_PostError("Can't create texture");
		return NULL;
	}

	return (HTEXTURE)pTex;
}

HTEXTURE CALL HGE_Impl::Texture_Load(const char *filename, DWORD size, bool bMipmap)
{
	void *data;
	DWORD _size;
	D3DFORMAT fmt1, fmt2;
	LPDIRECT3DTEXTURE8 pTex;
	D3DXIMAGE_INFO info;
	CTextureList *texItem;

	if(size) { data=(void *)filename; _size=size; }
	else
	{
		data=pHGE->Resource_Load(filename, &_size);
		if(!data) return NULL;
	}

	if(*(DWORD*)data == 0x20534444) // Compressed DDS format magic number
	{
		fmt1=D3DFMT_UNKNOWN;
		fmt2=D3DFMT_A8R8G8B8;
	}
	else
	{
		fmt1=D3DFMT_A8R8G8B8;
		fmt2=D3DFMT_UNKNOWN;
	}

//	if( FAILED( D3DXCreateTextureFromFileInMemory( pD3DDevice, data, _size, &pTex ) ) ) pTex=NULL;
	if( FAILED( D3DXCreateTextureFromFileInMemoryEx( pD3DDevice, data, _size,
										D3DX_DEFAULT, D3DX_DEFAULT,
										bMipmap ? 0:1,		// Mip levels
										0,					// Usage
										fmt1,				// Format
										D3DPOOL_MANAGED,	// Memory pool
										D3DX_FILTER_NONE,	// Filter
										D3DX_DEFAULT,		// Mip filter
										0,					// Color key
										&info, NULL,
										&pTex ) ) )

	if( FAILED( D3DXCreateTextureFromFileInMemoryEx( pD3DDevice, data, _size,
										D3DX_DEFAULT, D3DX_DEFAULT,
										bMipmap ? 0:1,		// Mip levels
										0,					// Usage
										fmt2,				// Format
										D3DPOOL_MANAGED,	// Memory pool
										D3DX_FILTER_NONE,	// Filter
										D3DX_DEFAULT,		// Mip filter
										0,					// Color key
										&info, NULL,
										&pTex ) ) )

	{	
		_PostError("Can't create texture");
		if(!size) Resource_Free(data);
		return NULL;
	}

	if(!size) Resource_Free(data);
	
	texItem=new CTextureList;
	texItem->tex=(HTEXTURE)pTex;
	texItem->width=info.Width;
	texItem->height=info.Height;
	texItem->next=textures;
	textures=texItem;

	return (HTEXTURE)pTex;
}

void CALL HGE_Impl::Texture_Free(HTEXTURE tex)
{
	LPDIRECT3DTEXTURE8 pTex=(LPDIRECT3DTEXTURE8)tex;
	CTextureList *texItem=textures, *texPrev=0;

	while(texItem)
	{
		if(texItem->tex==tex)
		{
			if(texPrev) texPrev->next=texItem->next;
			else textures=texItem->next;
			delete texItem;
			break;
		}
		texPrev=texItem;
		texItem=texItem->next;
	}
	if(pTex != NULL) pTex->Release();
}

int CALL HGE_Impl::Texture_GetWidth(HTEXTURE tex, bool bOriginal)
{
	D3DSURFACE_DESC TDesc;
	LPDIRECT3DTEXTURE8 pTex=(LPDIRECT3DTEXTURE8)tex;
	CTextureList *texItem=textures;

	if(bOriginal)
	{
		while(texItem)
		{
			if(texItem->tex==tex) return texItem->width;
			texItem=texItem->next;
		}
		return 0;
	}
	else
	{
		if(FAILED(pTex->GetLevelDesc(0, &TDesc))) return 0;
		else return TDesc.Width;
	}
}


int CALL HGE_Impl::Texture_GetHeight(HTEXTURE tex, bool bOriginal)
{
	D3DSURFACE_DESC TDesc;
	LPDIRECT3DTEXTURE8 pTex=(LPDIRECT3DTEXTURE8)tex;
	CTextureList *texItem=textures;

	if(bOriginal)
	{
		while(texItem)
		{
			if(texItem->tex==tex) return texItem->height;
			texItem=texItem->next;
		}
		return 0;
	}
	else
	{
		if(FAILED(pTex->GetLevelDesc(0, &TDesc))) return 0;
		else return TDesc.Height;
	}
}


DWORD * CALL HGE_Impl::Texture_Lock(HTEXTURE tex, bool bReadOnly, int left, int top, int width, int height)
{
	LPDIRECT3DTEXTURE8 pTex=(LPDIRECT3DTEXTURE8)tex;
	D3DSURFACE_DESC TDesc;
	D3DLOCKED_RECT TRect;
	RECT region, *prec;
	int flags;

	pTex->GetLevelDesc(0, &TDesc);
	if(TDesc.Format!=D3DFMT_A8R8G8B8 && TDesc.Format!=D3DFMT_X8R8G8B8) return 0;

	if(width && height)
	{
		region.left=left;
		region.top=top;
		region.right=left+width;
		region.bottom=top+height;
		prec=&region;
	}
	else prec=0;

	if(bReadOnly) flags=D3DLOCK_READONLY;
	else flags=0;

	if(FAILED(pTex->LockRect(0, &TRect, prec, flags)))
	{
		_PostError("Can't lock texture");
		return 0;
	}

	return (DWORD *)TRect.pBits;
}


void CALL HGE_Impl::Texture_Unlock(HTEXTURE tex)
{
	LPDIRECT3DTEXTURE8 pTex=(LPDIRECT3DTEXTURE8)tex;
	pTex->UnlockRect(0);
}

//////// Implementation ////////

void HGE_Impl::_render_batch(bool bEndScene)
{
	if(VertArray)
	{
		pVB->Unlock();
		
		if(nPrim)
		{
			switch(CurPrimType)
			{
				case HGEPRIM_QUADS:
					pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, nPrim<<2, 0, nPrim<<1);
					break;

				case HGEPRIM_TRIPLES:
					pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, nPrim);
					break;

				case HGEPRIM_LINES:
					pD3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, nPrim);
					break;
			}

			nPrim=0;
		}

		if(bEndScene) VertArray = 0;
		else pVB->Lock( 0, 0, (BYTE**)&VertArray, 0 );
	}
}

void HGE_Impl::_SetBlendMode(int blend)
{
	if((blend & BLEND_ALPHABLEND) != (CurBlendMode & BLEND_ALPHABLEND))
	{
		if(blend & BLEND_ALPHABLEND) pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		else pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

    if((blend & BLEND_ZWRITE) != (CurBlendMode & BLEND_ZWRITE))
	{
		if(blend & BLEND_ZWRITE) pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		else pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}			
	
	if((blend & BLEND_COLORADD) != (CurBlendMode & BLEND_COLORADD))
	{
		if(blend & BLEND_COLORADD) pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
		else pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	}

	CurBlendMode = blend;
}

void HGE_Impl::_SetProjectionMatrix(int width, int height)
{
	D3DXMATRIX tmp;
	D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
	D3DXMatrixTranslation(&tmp, -0.5f, height+0.5f, 0.0f);
	D3DXMatrixMultiply(&matProj, &matProj, &tmp);
	D3DXMatrixOrthoOffCenterLH(&tmp, 0, (float)width, 0, (float)height, 0.0f, 1.0f);
	D3DXMatrixMultiply(&matProj, &matProj, &tmp);
}

bool HGE_Impl::_GfxInit()
{
	static const char *szFormats[]={"UNKNOWN", "R5G6B5", "X1R5G5B5", "A1R5G5B5", "X8R8G8B8", "A8R8G8B8"};
	D3DADAPTER_IDENTIFIER8 AdID;
	D3DDISPLAYMODE Mode;
	D3DFORMAT Format=D3DFMT_UNKNOWN;
	UINT nModes, i;
	
// Init D3D
							
	pD3D=Direct3DCreate8(120); // D3D_SDK_VERSION
	if(pD3D==NULL)
	{
		_PostError("Can't create D3D interface");
		return false;
	}

// Get adapter info

	pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, D3DENUM_NO_WHQL_LEVEL, &AdID);
	System_Log("D3D Driver: %s",AdID.Driver);
	System_Log("Description: %s",AdID.Description);
	System_Log("Version: %d.%d.%d.%d",
			HIWORD(AdID.DriverVersion.HighPart),
			LOWORD(AdID.DriverVersion.HighPart),
			HIWORD(AdID.DriverVersion.LowPart),
			LOWORD(AdID.DriverVersion.LowPart));

// Set up Windowed presentation parameters
	
	if(FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Mode)) || Mode.Format==D3DFMT_UNKNOWN) 
	{
		_PostError("Can't determine desktop video mode");
		if(bWindowed) return false;
	}
	
	ZeroMemory(&d3dppW, sizeof(d3dppW));

	d3dppW.BackBufferWidth  = nScreenWidth;
	d3dppW.BackBufferHeight = nScreenHeight;
	d3dppW.BackBufferFormat = Mode.Format;
	d3dppW.BackBufferCount  = 1;
	d3dppW.MultiSampleType  = D3DMULTISAMPLE_NONE;
	d3dppW.hDeviceWindow    = hwnd;
	d3dppW.Windowed         = TRUE;

	if(nHGEFPS==HGEFPS_VSYNC) d3dppW.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
	else					  d3dppW.SwapEffect = D3DSWAPEFFECT_COPY;

	if(bZBuffer)
	{
		d3dppW.EnableAutoDepthStencil = TRUE;
		d3dppW.AutoDepthStencilFormat = D3DFMT_D16;
	}

// Set up Full Screen presentation parameters

	nModes=pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT);

	for(i=0; i<nModes; i++)
	{
		pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, i, &Mode);
		if(Mode.Width != (UINT)nScreenWidth || Mode.Height != (UINT)nScreenHeight) continue;
		if(nScreenBPP==16 && (_format_id(Mode.Format) > _format_id(D3DFMT_A1R5G5B5))) continue;
		if(_format_id(Mode.Format) > _format_id(Format)) Format=Mode.Format;
	}

	if(Format == D3DFMT_UNKNOWN)
	{
		_PostError("Can't find appropriate full screen video mode");
		if(!bWindowed) return false;
	}

	ZeroMemory(&d3dppFS, sizeof(d3dppFS));

	d3dppFS.BackBufferWidth  = nScreenWidth;
	d3dppFS.BackBufferHeight = nScreenHeight;
	d3dppFS.BackBufferFormat = Format;
	d3dppFS.BackBufferCount  = 1;
	d3dppFS.MultiSampleType  = D3DMULTISAMPLE_NONE;
	d3dppFS.hDeviceWindow    = hwnd;
	d3dppFS.Windowed         = FALSE;

	d3dppFS.SwapEffect       = D3DSWAPEFFECT_FLIP;
	d3dppFS.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if(nHGEFPS==HGEFPS_VSYNC) d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	else					  d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if(bZBuffer)
	{
		d3dppFS.EnableAutoDepthStencil = TRUE;
		d3dppFS.AutoDepthStencilFormat = D3DFMT_D16;
	}

	d3dpp = bWindowed ? &d3dppW : &d3dppFS;

	if(_format_id(d3dpp->BackBufferFormat) < 4) nScreenBPP=16;
	else nScreenBPP=32;
	
// Create D3D Device

	if( FAILED( pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                                  D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                  d3dpp, &pD3DDevice ) ) )
	{
		_PostError("Can't create D3D device");
		return false;
	}

	_AdjustWindow();

	System_Log("Mode: %d x %d x %s\n",nScreenWidth,nScreenHeight,szFormats[_format_id(Format)]);

// Create vertex batch buffer

	VertArray=0;
	textures=0;

// Init all stuff that can be lost

	_SetProjectionMatrix(nScreenWidth, nScreenHeight);
	D3DXMatrixIdentity(&matView);
	
	if(!_init_lost()) return false;

	Gfx_Clear(0);

	return true;
}

int HGE_Impl::_format_id(D3DFORMAT fmt)
{
	switch(fmt) {
		case D3DFMT_R5G6B5:		return 1;
		case D3DFMT_X1R5G5B5:	return 2;
		case D3DFMT_A1R5G5B5:	return 3;
		case D3DFMT_X8R8G8B8:	return 4;
		case D3DFMT_A8R8G8B8:	return 5;
		default:				return 0;
	}
}

void HGE_Impl::_AdjustWindow()
{
	RECT *rc;
	LONG style;

	if(bWindowed) {rc=&rectW; style=styleW; }
	else  {rc=&rectFS; style=styleFS; }
	SetWindowLong(hwnd, GWL_STYLE, style);

	style=GetWindowLong(hwnd, GWL_EXSTYLE);
	if(bWindowed)
	{
		SetWindowLong(hwnd, GWL_EXSTYLE, style & (~WS_EX_TOPMOST));
	    SetWindowPos(hwnd, HWND_NOTOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_FRAMECHANGED);
	}
	else
	{
		SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_TOPMOST);
	    SetWindowPos(hwnd, HWND_TOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_FRAMECHANGED);
	}
}

void HGE_Impl::_Resize(int width, int height)
{
	if(hwndParent)
	{
		//if(procFocusLostFunc) procFocusLostFunc();

		d3dppW.BackBufferWidth=width;
		d3dppW.BackBufferHeight=height;
		nScreenWidth=width;
		nScreenHeight=height;

		_SetProjectionMatrix(nScreenWidth, nScreenHeight);
		_GfxRestore();

		//if(procFocusGainFunc) procFocusGainFunc();
	}
}

void HGE_Impl::_GfxDone()
{
	CRenderTargetList *target=pTargets, *next_target;
	
	while(textures)	Texture_Free(textures->tex);

	if(pScreenSurf) { pScreenSurf->Release(); pScreenSurf=0; }
	if(pScreenDepth) { pScreenDepth->Release(); pScreenDepth=0; }

	while(target)
	{
		if(target->pTex) target->pTex->Release();
		if(target->pDepth) target->pDepth->Release();
		next_target=target->next;
		delete target;
		target=next_target;
	}
	pTargets=0;

	if(pIB)
	{
		pD3DDevice->SetIndices(NULL,0);
		pIB->Release();
		pIB=0;
	}
	if(pVB)
	{
		if(VertArray) {	pVB->Unlock(); VertArray=0;	}
		pD3DDevice->SetStreamSource( 0, NULL, sizeof(hgeVertex) );
		pVB->Release();
		pVB=0;
	}
	if(pD3DDevice) { pD3DDevice->Release(); pD3DDevice=0; }
	if(pD3D) { pD3D->Release(); pD3D=0; }
}


bool HGE_Impl::_GfxRestore()
{
	CRenderTargetList *target=pTargets;

	//if(!pD3DDevice) return false;
	//if(pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICELOST) return;

	if(pScreenSurf) pScreenSurf->Release();
	if(pScreenDepth) pScreenDepth->Release();

	while(target)
	{
		if(target->pTex) target->pTex->Release();
		if(target->pDepth) target->pDepth->Release();
		target=target->next;
	}

	if(pIB)
	{
		pD3DDevice->SetIndices(NULL,0);
		pIB->Release();
	}
	if(pVB)
	{
		pD3DDevice->SetStreamSource( 0, NULL, sizeof(hgeVertex) );
		pVB->Release();
	}

	pD3DDevice->Reset(d3dpp);

	if(!_init_lost()) return false;

	if(procGfxRestoreFunc) return procGfxRestoreFunc();

	return true;
}


bool HGE_Impl::_init_lost()
{
	CRenderTargetList *target=pTargets;

// Store render target

	pScreenSurf=0;
	pScreenDepth=0;

	pD3DDevice->GetRenderTarget(&pScreenSurf);
	pD3DDevice->GetDepthStencilSurface(&pScreenDepth);
	
	while(target)
	{
		if(target->pTex)
			D3DXCreateTexture(pD3DDevice, target->width, target->height, 1, D3DUSAGE_RENDERTARGET,
							  d3dpp->BackBufferFormat, D3DPOOL_DEFAULT, &target->pTex);
		if(target->pDepth)
			pD3DDevice->CreateDepthStencilSurface(target->width, target->height,
												  D3DFMT_D16, D3DMULTISAMPLE_NONE, &target->pDepth);
		target=target->next;
	}

// Create Vertex buffer
	
	if( FAILED (pD3DDevice->CreateVertexBuffer(VERTEX_BUFFER_SIZE*sizeof(hgeVertex),
                                              D3DUSAGE_WRITEONLY,
											  D3DFVF_HGEVERTEX,
                                              D3DPOOL_DEFAULT, &pVB )))
	{
		_PostError("Can't create D3D vertex buffer");
		return false;
	}

	pD3DDevice->SetVertexShader( D3DFVF_HGEVERTEX );
	pD3DDevice->SetStreamSource( 0, pVB, sizeof(hgeVertex) );

// Create and setup Index buffer

	if( FAILED( pD3DDevice->CreateIndexBuffer(VERTEX_BUFFER_SIZE*6/4*sizeof(WORD),
                                              D3DUSAGE_WRITEONLY,
											  D3DFMT_INDEX16,
                                              D3DPOOL_DEFAULT, &pIB ) ) )
	{
		_PostError("Can't create D3D index buffer");
		return false;
	}

	WORD *pIndices, n=0;
	if( FAILED( pIB->Lock( 0, 0, (BYTE**)&pIndices, 0 ) ) )
	{
		_PostError("Can't lock D3D index buffer");
		return false;
	}

	for(int i=0; i<VERTEX_BUFFER_SIZE/4; i++) {
		*pIndices++=n;
		*pIndices++=n+1;
		*pIndices++=n+2;
		*pIndices++=n+2;
		*pIndices++=n+3;
		*pIndices++=n;
		n+=4;
	}

	pIB->Unlock();
	pD3DDevice->SetIndices(pIB,0);

// Set common render states

	//pD3DDevice->SetRenderState( D3DRS_LASTPIXEL, FALSE );
	pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	
	pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pD3DDevice->SetRenderState( D3DRS_ALPHAREF,        0x01 );
	pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);

	if(bTextureFilter)
	{
		pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
		pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	}
	else
	{
		pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
		pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	}

	nPrim=0;
	CurPrimType=HGEPRIM_QUADS;
	CurBlendMode = BLEND_DEFAULT;
	CurTexture = NULL;

	pD3DDevice->SetTransform(D3DTS_VIEW, &matView);
	pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	return true;
}
