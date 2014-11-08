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


void HGE_CALL HGE_Impl::Gfx_Clear(hgeU32 color)
{
  if(pCurTarget) {
    SDL_SetRenderDrawColor(m_renderer,
                          GETR(color), GETG(color), GETB(color), GETA(color));

      // Clear the entire screen to our selected color.
    SDL_RenderClear(m_renderer);
  }
}

void HGE_CALL HGE_Impl::Gfx_SetClipping(int x, int y, int w, int h)
{
	SDL_Rect rc = {x, y, w, h};
	SDL_RenderSetClipRect(m_renderer, & rc);

    //hgeGAPIViewport vp;
    //int scr_width, scr_height;

    //if(!pCurTarget) {
    //    scr_width=pHGE->System_GetStateInt(HGE_SCREENWIDTH);
    //    scr_height=pHGE->System_GetStateInt(HGE_SCREENHEIGHT);
    //} else {
    //    scr_width=Texture_GetWidth((HTEXTURE)pCurTarget->pTex);
    //    scr_height=Texture_GetHeight((HTEXTURE)pCurTarget->pTex);
    //}

    //if(!w) {
    //    vp.X=0;
    //    vp.Y=0;
    //    vp.Width=scr_width;
    //    vp.Height=scr_height;
    //} else {
    //    if(x<0) {
    //        w+=x;
    //        x=0;
    //    }
    //    if(y<0) {
    //        h+=y;
    //        y=0;
    //    }

    //    if(x+w > scr_width) {
    //        w=scr_width-x;
    //    }
    //    if(y+h > scr_height) {
    //        h=scr_height-y;
    //    }

    //    vp.X=x;
    //    vp.Y=y;
    //    vp.Width=w;
    //    vp.Height=h;
    //}

    //vp.MinZ=0.0f;
    //vp.MaxZ=1.0f;

    //_render_batch();
    //pD3DDevice->SetViewport(&vp);

    //D3DXMATRIX tmp;
    //D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
    //D3DXMatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
    //D3DXMatrixMultiply(&matProj, &matProj, &tmp);
    //D3DXMatrixOrthoOffCenterLH(&tmp, (float)vp.X, (float)(vp.X+vp.Width), -((float)(vp.Y+vp.Height)),
    //                           -((float)vp.Y), vp.MinZ, vp.MaxZ);
    //D3DXMatrixMultiply(&matProj, &matProj, &tmp);
    //pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

void HGE_CALL HGE_Impl::Gfx_SetTransform(float x, float y, float dx, float dy, float rot,
        float hscale, float vscale)
{
	// TODO!
    //D3DXMATRIX tmp;

    //if(vscale==0.0f) {
    //    D3DXMatrixIdentity(&matView);
    //} else {
    //    D3DXMatrixTranslation(&matView, -x, -y, 0.0f);
    //    D3DXMatrixScaling(&tmp, hscale, vscale, 1.0f);
    //    D3DXMatrixMultiply(&matView, &matView, &tmp);
    //    D3DXMatrixRotationZ(&tmp, -rot);
    //    D3DXMatrixMultiply(&matView, &matView, &tmp);
    //    D3DXMatrixTranslation(&tmp, x+dx, y+dy, 0.0f);
    //    D3DXMatrixMultiply(&matView, &matView, &tmp);
    //}

    //_render_batch();
    //pD3DDevice->SetTransform(D3DTS_VIEW, &matView);
}

bool HGE_CALL HGE_Impl::Gfx_BeginScene(HTARGET target)
{
  pCurTarget = target;
  return true;
}

void HGE_CALL HGE_Impl::Gfx_EndScene()
{
    _render_batch(true);
    pD3DDevice->EndScene();
    if(!pCurTarget) {
        pD3DDevice->Present( NULL, NULL, NULL, NULL );
    }
}

void HGE_CALL HGE_Impl::Gfx_RenderLine(float x1, float y1, float x2, float y2, hgeU32 color,
                                       float z)
{
    if(VertArray) {
        if(CurPrimType!=HGEPRIM_LINES || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_LINES || CurTexture
                || CurBlendMode!=BLEND_DEFAULT) {
            _render_batch();

            CurPrimType=HGEPRIM_LINES;
            if(CurBlendMode != BLEND_DEFAULT) {
                _SetBlendMode(BLEND_DEFAULT);
            }
            if(CurTexture) {
                pD3DDevice->SetTexture(0, 0);
                CurTexture=0;
            }
        }

        int i=nPrim*HGEPRIM_LINES;
        VertArray[i].x = x1;
        VertArray[i+1].x = x2;
        VertArray[i].y = y1;
        VertArray[i+1].y = y2;
        VertArray[i].z     = VertArray[i+1].z = z;
        VertArray[i].col   = VertArray[i+1].col = color;
        VertArray[i].tx    = VertArray[i+1].tx =
                                 VertArray[i].ty    = VertArray[i+1].ty = 0.0f;

        nPrim++;
    }
}

void HGE_CALL HGE_Impl::Gfx_RenderTriple(const hgeTriple *triple)
{
    //if(VertArray) {
    //    if(CurPrimType!=HGEPRIM_TRIPLES || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_TRIPLES
    //            || CurTexture!=triple->tex || CurBlendMode!=triple->blend) {
    //        _render_batch();

    //        CurPrimType=HGEPRIM_TRIPLES;
    //        if(CurBlendMode != triple->blend) {
    //            _SetBlendMode(triple->blend);
    //        }
    //        if(triple->tex != CurTexture) {
    //            pD3DDevice->SetTexture( 0, (hgeGAPITexture *)triple->tex );
    //            CurTexture = triple->tex;
    //        }
    //    }

    //    memcpy(&VertArray[nPrim*HGEPRIM_TRIPLES], triple->v, sizeof(hgeVertex)*HGEPRIM_TRIPLES);
    //    nPrim++;
    //}
}

void HGE_CALL HGE_Impl::Gfx_RenderQuad(const hgeQuad *quad)
{
    if(VertArray) {
        if(CurPrimType!=HGEPRIM_QUADS || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_QUADS || CurTexture!=quad->tex
                || CurBlendMode!=quad->blend) {
            _render_batch();

            CurPrimType=HGEPRIM_QUADS;
            if(CurBlendMode != quad->blend) {
                _SetBlendMode(quad->blend);
            }
            if(quad->tex != CurTexture) {
                pD3DDevice->SetTexture( 0, (hgeGAPITexture *)quad->tex );
                CurTexture = quad->tex;
            }
        }

        memcpy(&VertArray[nPrim*HGEPRIM_QUADS], quad->v, sizeof(hgeVertex)*HGEPRIM_QUADS);
        nPrim++;
    }
}

hgeVertex* HGE_CALL HGE_Impl::Gfx_StartBatch(int prim_type, HTEXTURE tex, int blend, int *max_prim)
{
    if(VertArray) {
        _render_batch();

        CurPrimType=prim_type;
        if(CurBlendMode != blend) {
            _SetBlendMode(blend);
        }
        if(tex != CurTexture) {
            pD3DDevice->SetTexture( 0, (hgeGAPITexture *)tex );
            CurTexture = tex;
        }

        *max_prim=VERTEX_BUFFER_SIZE / prim_type;
        return VertArray;
    } else {
        return 0;
    }
}

void HGE_CALL HGE_Impl::Gfx_FinishBatch(int nprim)
{
    nPrim=nprim;
}

HTARGET HGE_CALL HGE_Impl::Target_Create(int width, int height, bool zbuffer)
{
    SDL_Texture * t = SDL_CreateTexture(m_renderer, 
                                        SDL_PIXELFORMAT_ARGB8888, 
                                        SDL_TEXTUREACCESS_TARGET,
                                        width, height);

//    if(zbuffer) {
//#if HGE_DIRECTX_VER == 8
//        if(FAILED(pD3DDevice->CreateDepthStencilSurface(pTarget->width, pTarget->height,
//                  D3DFMT_D16, D3DMULTISAMPLE_NONE, &pTarget->pDepth)))
//#endif
//#if HGE_DIRECTX_VER == 9
//            if(FAILED(pD3DDevice->CreateDepthStencilSurface(width, height,
//                      D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, false, &pTarget->pDepth, NULL)))
//#endif
//            {
//                pTarget->pTex->Release();
//                _PostError("Can't create render target depth buffer");
//                delete pTarget;
//                return 0;
//            }
//    }

    return new_htarget(t);
}

void HGE_CALL HGE_Impl::Target_Free(HTARGET target)
{
  SDL_DestroyTexture(from_htarget(target));
}

HTEXTURE HGE_CALL HGE_Impl::Target_GetTexture(HTARGET target)
{
  return new_htexture(from_htarget(target));
}

HTEXTURE HGE_CALL HGE_Impl::Texture_Create(int width, int height)
{
  // access_static: changes rarely, not lockable
  // access_streaming: changes frequently, lockable
  SDL_Texture * t = SDL_CreateTexture(m_renderer, 
                                    SDL_PIXELFORMAT_ARGB8888, 
                                    SDL_TEXTUREACCESS_STREAMING,
                                    width, height);
  return new_htexture(t);
}

HTEXTURE HGE_CALL HGE_Impl::Texture_Load(const char *filename, hgeU32 size, bool bMipmap)
{
    void *data;
    hgeU32 _size;
    D3DFORMAT fmt1, fmt2;
    hgeGAPITexture * pTex;
    D3DXIMAGE_INFO info;
    CTextureList *texItem;

    if(size) {
        data=(void *)filename;
        _size=size;
    } else {
        data=pHGE->Resource_Load(filename, &_size);
        if(!data) {
            return NULL;
        }
    }

    if(*(hgeU32*)data == 0x20534444) { // Compressed DDS format magic number
        fmt1=D3DFMT_UNKNOWN;
        fmt2=D3DFMT_A8R8G8B8;
    } else {
        fmt1=D3DFMT_A8R8G8B8;
        fmt2=D3DFMT_UNKNOWN;
    }

//  if( FAILED( D3DXCreateTextureFromFileInMemory( pD3DDevice, data, _size, &pTex ) ) ) pTex=NULL;
    if( FAILED( D3DXCreateTextureFromFileInMemoryEx( pD3DDevice, data, _size,
                D3DX_DEFAULT, D3DX_DEFAULT,
                bMipmap ? 0:1,      // Mip levels
                0,                  // Usage
                fmt1,               // Format
                D3DPOOL_MANAGED,    // Memory pool
                D3DX_FILTER_NONE,   // Filter
                D3DX_DEFAULT,       // Mip filter
                0,                  // Color key
                &info, NULL,
                &pTex ) ) )

        if( FAILED( D3DXCreateTextureFromFileInMemoryEx( pD3DDevice, data, _size,
                    D3DX_DEFAULT, D3DX_DEFAULT,
                    bMipmap ? 0:1,      // Mip levels
                    0,                  // Usage
                    fmt2,               // Format
                    D3DPOOL_MANAGED,    // Memory pool
                    D3DX_FILTER_NONE,   // Filter
                    D3DX_DEFAULT,       // Mip filter
                    0,                  // Color key
                    &info, NULL,
                    &pTex ) ) )

        {
            _PostError("Can't create texture");
            if(!size) {
                Resource_Free(data);
            }
            return NULL;
        }

    if(!size) {
        Resource_Free(data);
    }

    texItem=new CTextureList;
    texItem->tex=(HTEXTURE)pTex;
    texItem->width=info.Width;
    texItem->height=info.Height;
    texItem->next=textures;
    textures=texItem;

    return (HTEXTURE)pTex;
}

void HGE_CALL HGE_Impl::Texture_Free(HTEXTURE tex)
{
    hgeGAPITexture * pTex = (hgeGAPITexture *)tex;
    CTextureList *texItem=textures, *texPrev=0;

    while(texItem) {
        if(texItem->tex==tex) {
            if(texPrev) {
                texPrev->next=texItem->next;
            } else {
                textures=texItem->next;
            }
            delete texItem;
            break;
        }
        texPrev=texItem;
        texItem=texItem->next;
    }
    if(pTex != NULL) {
        pTex->Release();
    }
}

int HGE_CALL HGE_Impl::Texture_GetWidth(HTEXTURE tex, bool bOriginal)
{
    D3DSURFACE_DESC TDesc;
    hgeGAPITexture * pTex = (hgeGAPITexture *)tex;
    CTextureList *texItem=textures;

    if(bOriginal) {
        while(texItem) {
            if(texItem->tex==tex) {
                return texItem->width;
            }
            texItem=texItem->next;
        }
        return 0;
    } else {
        if(FAILED(pTex->GetLevelDesc(0, &TDesc))) {
            return 0;
        } else {
            return TDesc.Width;
        }
    }
}


int HGE_CALL HGE_Impl::Texture_GetHeight(HTEXTURE tex, bool bOriginal)
{
    D3DSURFACE_DESC TDesc;
    hgeGAPITexture * pTex = (hgeGAPITexture *)tex;
    CTextureList *texItem=textures;

    if(bOriginal) {
        while(texItem) {
            if(texItem->tex==tex) {
                return texItem->height;
            }
            texItem=texItem->next;
        }
        return 0;
    } else {
        if(FAILED(pTex->GetLevelDesc(0, &TDesc))) {
            return 0;
        } else {
            return TDesc.Height;
        }
    }
}


hgeU32 * HGE_CALL HGE_Impl::Texture_Lock(HTEXTURE tex, bool bReadOnly, int left, int top, int width,
        int height)
{
    hgeGAPITexture * pTex = (hgeGAPITexture *)tex;
    D3DSURFACE_DESC TDesc;
    D3DLOCKED_RECT TRect;
    RECT region, *prec;
    int flags;

    pTex->GetLevelDesc(0, &TDesc);
    if(TDesc.Format!=D3DFMT_A8R8G8B8 && TDesc.Format!=D3DFMT_X8R8G8B8) {
        return 0;
    }

    if(width && height) {
        region.left=left;
        region.top=top;
        region.right=left+width;
        region.bottom=top+height;
        prec=&region;
    } else {
        prec=0;
    }

    if(bReadOnly) {
        flags=D3DLOCK_READONLY;
    } else {
        flags=0;
    }

    if(FAILED(pTex->LockRect(0, &TRect, prec, flags))) {
        _PostError("Can't lock texture");
        return 0;
    }

    return (hgeU32 *)TRect.pBits;
}


void HGE_CALL HGE_Impl::Texture_Unlock(HTEXTURE tex)
{
    hgeGAPITexture * pTex = (hgeGAPITexture *)tex;
    pTex->UnlockRect(0);
}

//////// Implementation ////////

void HGE_Impl::_render_batch(bool bEndScene)
{
    if(VertArray) {
        pVB->Unlock();

        if(nPrim) {
            switch(CurPrimType) {
            case HGEPRIM_QUADS:
#if HGE_DIRECTX_VER == 8
                pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, nPrim<<2, 0, nPrim<<1);
#endif
#if HGE_DIRECTX_VER == 9
                pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nPrim<<2, 0, nPrim<<1);
#endif
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

        if(bEndScene) {
            VertArray = 0;
        }
#if HGE_DIRECTX_VER == 8
        else {
            pVB->Lock( 0, 0, (hgeU8**)&VertArray, D3DLOCK_DISCARD );
        }
#endif
#if HGE_DIRECTX_VER == 9
        else {
            pVB->Lock( 0, 0, (VOID**)&VertArray, D3DLOCK_DISCARD );
        }
#endif
    }
}

//void HGE_Impl::_SetBlendMode(int blend)
//{
//    if((blend & BLEND_ALPHABLEND) != (CurBlendMode & BLEND_ALPHABLEND))
//    {
//        if(blend & BLEND_ALPHABLEND) pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//        else pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
//    }
//
//    if((blend & BLEND_ZWRITE) != (CurBlendMode & BLEND_ZWRITE))
//    {
//        if(blend & BLEND_ZWRITE) pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
//        else pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
//    }
//
//    if((blend & BLEND_COLORADD) != (CurBlendMode & BLEND_COLORADD))
//    {
//        if(blend & BLEND_COLORADD) pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
//        else pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
//    }
//
//    CurBlendMode = blend;
//}
void HGE_Impl::_SetBlendMode(int blend)
{
    int d = -1;

    if((blend & BLEND_ALPHABLEND) != (CurBlendMode & BLEND_ALPHABLEND)) {
        if(blend & BLEND_ALPHABLEND) {
            d = D3DBLEND_INVSRCALPHA;
        } else {
            d = D3DBLEND_ONE;
        }
    }

    if ((blend & BLEND_DARKEN) != (CurBlendMode & BLEND_DARKEN)) {
        if (blend & BLEND_DARKEN) {
            pD3DDevice->SetRenderState (D3DRS_SRCBLEND, D3DBLEND_ZERO);
            d = D3DBLEND_SRCCOLOR;
        } else {
            pD3DDevice->SetRenderState (D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            d = D3DBLEND_INVSRCALPHA;
        }
    }

    if (d != -1) {
        pD3DDevice->SetRenderState (D3DRS_DESTBLEND, d);
    }

    if((blend & BLEND_ZWRITE) != (CurBlendMode & BLEND_ZWRITE)) {
        if(blend & BLEND_ZWRITE) {
            pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        } else {
            pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        }
    }

    if((blend & (BLEND_COLORADD+BLEND_DARKEN)) != (CurBlendMode & (BLEND_COLORADD+BLEND_DARKEN))) {
        if (blend & BLEND_COLORADD) {
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
        } else if (blend & BLEND_DARKEN) {
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BLENDCURRENTALPHA);
        } else {
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        }
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
//    static const char *szFormats[]= {"UNKNOWN", "R5G6B5", "X1R5G5B5", "A1R5G5B5", "X8R8G8B8", "A8R8G8B8"};
//    hgeGAPIAdapterIdentifier AdID;
//    D3DDISPLAYMODE Mode;
//    D3DFORMAT Format=D3DFMT_UNKNOWN;
//    UINT nModes, i;
//
//// Init D3D
//#if HGE_DIRECTX_VER == 8
//    pD3D=Direct3DCreate8(120); // D3D_SDK_VERSION
//#endif
//#if HGE_DIRECTX_VER == 9
//    pD3D=Direct3DCreate9(D3D_SDK_VERSION); // D3D_SDK_VERSION
//#endif
//    if(pD3D==NULL) {
//        _PostError("Can't create D3D interface");
//        return false;
//    }
//
//// Get adapter info
//
//#if HGE_DIRECTX_VER == 8
//    pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, D3DENUM_NO_WHQL_LEVEL, &AdID);
//#endif
//#if HGE_DIRECTX_VER == 9
//    pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &AdID);
//#endif
//    System_Log("D3D Driver: %s",AdID.Driver);
//    System_Log("Description: %s",AdID.Description);
//    System_Log("Version: %d.%d.%d.%d",
//               HIWORD(AdID.DriverVersion.HighPart),
//               LOWORD(AdID.DriverVersion.HighPart),
//               HIWORD(AdID.DriverVersion.LowPart),
//               LOWORD(AdID.DriverVersion.LowPart));
//
//// Set up Windowed presentation parameters
//
//    if(FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Mode)) || Mode.Format==D3DFMT_UNKNOWN) {
//        _PostError("Can't determine desktop video mode");
//        if(bWindowed) {
//            return false;
//        }
//    }
//
//    ZeroMemory(&d3dppW, sizeof(d3dppW));
//
//    d3dppW.BackBufferWidth  = nScreenWidth;
//    d3dppW.BackBufferHeight = nScreenHeight;
//    d3dppW.BackBufferFormat = Mode.Format;
//    d3dppW.BackBufferCount  = 1;
//    d3dppW.MultiSampleType  = D3DMULTISAMPLE_NONE;
//    d3dppW.hDeviceWindow    = hwnd;
//    d3dppW.Windowed         = TRUE;
//
//#if HGE_DIRECTX_VER == 8
//    if(nHGEFPS==HGEFPS_VSYNC) {
//        d3dppW.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
//    } else {
//        d3dppW.SwapEffect = D3DSWAPEFFECT_COPY;
//    }
//#endif
//#if HGE_DIRECTX_VER == 9
//    if(nHGEFPS==HGEFPS_VSYNC) {
//        d3dppW.SwapEffect = D3DSWAPEFFECT_COPY;
//        d3dppW.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
//    } else {
//        d3dppW.SwapEffect = D3DSWAPEFFECT_COPY;
//        d3dppW.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
//    }
//#endif
//
//    if(bZBuffer) {
//        d3dppW.EnableAutoDepthStencil = TRUE;
//        d3dppW.AutoDepthStencilFormat = D3DFMT_D16;
//    }
//
//// Set up Full Screen presentation parameters
//
//#if HGE_DIRECTX_VER == 8
//    nModes=pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT);
//#endif
//#if HGE_DIRECTX_VER == 9
//    nModes=pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, Mode.Format);
//#endif
//
//    for(i=0; i<nModes; i++) {
//#if HGE_DIRECTX_VER == 8
//        pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, i, &Mode);
//#endif
//#if HGE_DIRECTX_VER == 9
//        pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, Mode.Format, i, &Mode);
//#endif
//
//        if(Mode.Width != (UINT)nScreenWidth || Mode.Height != (UINT)nScreenHeight) {
//            continue;
//        }
//        if(nScreenBPP==16 && (_format_id(Mode.Format) > _format_id(D3DFMT_A1R5G5B5))) {
//            continue;
//        }
//        if(_format_id(Mode.Format) > _format_id(Format)) {
//            Format=Mode.Format;
//        }
//    }
//
//    if(Format == D3DFMT_UNKNOWN) {
//        _PostError("Can't find appropriate full screen video mode");
//        if(!bWindowed) {
//            return false;
//        }
//    }
//
//    ZeroMemory(&d3dppFS, sizeof(d3dppFS));
//
//    d3dppFS.BackBufferWidth  = nScreenWidth;
//    d3dppFS.BackBufferHeight = nScreenHeight;
//    d3dppFS.BackBufferFormat = Format;
//    d3dppFS.BackBufferCount  = 1;
//    d3dppFS.MultiSampleType  = D3DMULTISAMPLE_NONE;
//    d3dppFS.hDeviceWindow    = hwnd;
//    d3dppFS.Windowed         = FALSE;
//
//    d3dppFS.SwapEffect       = D3DSWAPEFFECT_FLIP;
//    d3dppFS.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
//
//#if HGE_DIRECTX_VER == 8
//    if(nHGEFPS==HGEFPS_VSYNC) {
//        d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
//    } else {
//        d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
//    }
//#endif
//#if HGE_DIRECTX_VER == 9
//    if(nHGEFPS==HGEFPS_VSYNC) {
//        d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
//    } else {
//        d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
//    }
//#endif
//    if(bZBuffer) {
//        d3dppFS.EnableAutoDepthStencil = TRUE;
//        d3dppFS.AutoDepthStencilFormat = D3DFMT_D16;
//    }
//
//    d3dpp = bWindowed ? &d3dppW : &d3dppFS;
//
//    if(_format_id(d3dpp->BackBufferFormat) < 4) {
//        nScreenBPP=16;
//    } else {
//        nScreenBPP=32;
//    }
//
//// Create D3D Device
//// #if HGE_DIRECTX_VER == 8
////     if( FAILED( pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
////                                   D3DCREATE_SOFTWARE_VERTEXPROCESSING,
////                                   d3dpp, &pD3DDevice ) ) )
////     {
////         _PostError("Can't create D3D8 device");
////         return false;
////     }
//// #endif
//// #if HGE_DIRECTX_VER == 9
//    hgeGAPICaps caps;
//    pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
//    hgeU32   vp;
//    if((caps.VertexShaderVersion < D3DVS_VERSION(1,1))
//            || !(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)) {
//        System_Log("Software Vertex-processing device selected");
//        vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
//    } else {
//        System_Log("Hardware Vertex-processing device selected");
//        vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
//    }
//    if( FAILED( pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, d3dpp,
//                                    &pD3DDevice ) ) ) {
//        _PostError("Can't create D3D device");
//        return false;
//    }
//// #endif
//
//    _AdjustWindow();
//
//    System_Log("Mode: %d x %d x %s\n",nScreenWidth,nScreenHeight,szFormats[_format_id(Format)]);
//
//// Create vertex batch buffer
//
//    VertArray=0;
//    textures=0;
//
//// Init all stuff that can be lost
//
//    _SetProjectionMatrix(nScreenWidth, nScreenHeight);
//    D3DXMatrixIdentity(&matView);
//
//    if(!_init_lost()) {
//        return false;
//    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

    Gfx_Clear(0);

    return true;
}

int HGE_Impl::_format_id(D3DFORMAT fmt)
{
    switch(fmt) {
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

    if(bWindowed) {
        rc=&rectW;
        style=styleW;
    } else  {
        rc=&rectFS;
        style=styleFS;
    }
    SetWindowLong(hwnd, GWL_STYLE, style);

    style=GetWindowLong(hwnd, GWL_EXSTYLE);
    if(bWindowed) {
        SetWindowLong(hwnd, GWL_EXSTYLE, style & (~WS_EX_TOPMOST));
        SetWindowPos(hwnd, HWND_NOTOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top,
                     SWP_FRAMECHANGED);
    } else {
        SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_TOPMOST);
        SetWindowPos(hwnd, HWND_TOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top,
                     SWP_FRAMECHANGED);
    }
}

void HGE_Impl::_Resize(int width, int height)
{
    if(hwndParent) {
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

    while(textures) {
        Texture_Free(textures->tex);
    }

    if(pScreenSurf) {
        pScreenSurf->Release();
        pScreenSurf=0;
    }
    if(pScreenDepth) {
        pScreenDepth->Release();
        pScreenDepth=0;
    }

    while(target) {
        if(target->pTex) {
            target->pTex->Release();
        }
        if(target->pDepth) {
            target->pDepth->Release();
        }
        next_target=target->next;
        delete target;
        target=next_target;
    }
    pTargets=0;

    if(pIB && pD3DDevice) {
#if HGE_DIRECTX_VER == 8
        pD3DDevice->SetIndices(NULL,0);
#endif
#if HGE_DIRECTX_VER == 9
        pD3DDevice->SetIndices(NULL);
#endif
        pIB->Release();
        pIB=0;
    }
    if(pVB && pD3DDevice) {
        if(VertArray) {
            pVB->Unlock();
            VertArray=0;
        }
#if HGE_DIRECTX_VER == 8
        pD3DDevice->SetStreamSource( 0, NULL, sizeof(hgeVertex) );
#endif
#if HGE_DIRECTX_VER == 9
        pD3DDevice->SetStreamSource( 0, NULL, 0, sizeof(hgeVertex) );
#endif
        pVB->Release();
        pVB=0;
    }
    if(pD3DDevice) {
        pD3DDevice->Release();
        pD3DDevice=0;
    }
    if(pD3D) {
        pD3D->Release();
        pD3D=0;
    }
}


bool HGE_Impl::_GfxRestore()
{
    CRenderTargetList *target=pTargets;

    //if(!pD3DDevice) return false;
    //if(pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICELOST) return;

    if(pScreenSurf) {
        pScreenSurf->Release();
    }
    if(pScreenDepth) {
        pScreenDepth->Release();
    }

    while(target) {
        if(target->pTex) {
            target->pTex->Release();
        }
        if(target->pDepth) {
            target->pDepth->Release();
        }
        target=target->next;
    }

    if(pIB) {
#if HGE_DIRECTX_VER == 8
        pD3DDevice->SetIndices(NULL,0);
#endif
#if HGE_DIRECTX_VER == 9
        pD3DDevice->SetIndices(NULL);
#endif
        pIB->Release();
    }
    if(pVB) {
#if HGE_DIRECTX_VER == 8
        pD3DDevice->SetStreamSource( 0, NULL, sizeof(hgeVertex) );
#endif
#if HGE_DIRECTX_VER == 9
        pD3DDevice->SetStreamSource( 0, NULL,0, sizeof(hgeVertex) );
#endif
        pVB->Release();
    }

    pD3DDevice->Reset(d3dpp);

    if(!_init_lost()) {
        return false;
    }

    if(procGfxRestoreFunc) {
        return procGfxRestoreFunc();
    }

    return true;
}


bool HGE_Impl::_init_lost()
{
    CRenderTargetList *target=pTargets;

// Store render target

    pScreenSurf=0;
    pScreenDepth=0;

#if HGE_DIRECTX_VER == 8
    pD3DDevice->GetRenderTarget(&pScreenSurf);
#endif
#if HGE_DIRECTX_VER == 9
    pD3DDevice->GetRenderTarget(0, &pScreenSurf);
#endif
    pD3DDevice->GetDepthStencilSurface(&pScreenDepth);

    while(target) {
        if(target->pTex)
            D3DXCreateTexture(pD3DDevice, target->width, target->height, 1, D3DUSAGE_RENDERTARGET,
                              d3dpp->BackBufferFormat, D3DPOOL_DEFAULT, &target->pTex);
        if(target->pDepth)
#if HGE_DIRECTX_VER == 8
            pD3DDevice->CreateDepthStencilSurface(target->width, target->height,
                                                  D3DFMT_D16, D3DMULTISAMPLE_NONE, &target->pDepth);
#endif
#if HGE_DIRECTX_VER == 9
        pD3DDevice->CreateDepthStencilSurface(target->width, target->height,
                                              D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, false, &target->pDepth, NULL);
#endif
        target=target->next;
    }

// Create Vertex buffer
#if HGE_DIRECTX_VER == 8
    if( FAILED (pD3DDevice->CreateVertexBuffer(VERTEX_BUFFER_SIZE*sizeof(hgeVertex),
                D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                D3DFVF_HGEVERTEX,
                D3DPOOL_DEFAULT, &pVB )))
#endif
#if HGE_DIRECTX_VER == 9
        if( FAILED (pD3DDevice->CreateVertexBuffer(VERTEX_BUFFER_SIZE*sizeof(hgeVertex),
                    D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                    D3DFVF_HGEVERTEX,
                    D3DPOOL_DEFAULT,
                    &pVB,
                    NULL)))
#endif
        {
            _PostError("Can't create D3D vertex buffer");
            return false;
        }

#if HGE_DIRECTX_VER == 8
    pD3DDevice->SetVertexShader( D3DFVF_HGEVERTEX );
    pD3DDevice->SetStreamSource( 0, pVB, sizeof(hgeVertex) );
#endif
#if HGE_DIRECTX_VER == 9
    pD3DDevice->SetVertexShader( NULL );
    pD3DDevice->SetFVF( D3DFVF_HGEVERTEX );
    pD3DDevice->SetStreamSource( 0, pVB, 0, sizeof(hgeVertex) );
#endif

// Create and setup Index buffer

#if HGE_DIRECTX_VER == 8
    if( FAILED( pD3DDevice->CreateIndexBuffer(VERTEX_BUFFER_SIZE*6/4*sizeof(hgeU16),
                D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX16,
                D3DPOOL_DEFAULT, &pIB ) ) )
#endif
#if HGE_DIRECTX_VER == 9
        if( FAILED( pD3DDevice->CreateIndexBuffer(VERTEX_BUFFER_SIZE*6/4*sizeof(hgeU16),
                    D3DUSAGE_WRITEONLY,
                    D3DFMT_INDEX16,
                    D3DPOOL_DEFAULT,
                    &pIB,
                    NULL) ) )
#endif
        {
            _PostError("Can't create D3D index buffer");
            return false;
        }

    hgeU16 *pIndices, n=0;
#if HGE_DIRECTX_VER == 8
    if( FAILED( pIB->Lock( 0, 0, (hgeU8**)&pIndices, 0 ) ) )
#endif
#if HGE_DIRECTX_VER == 9
        if( FAILED( pIB->Lock( 0, 0, (VOID**)&pIndices, 0 ) ) )
#endif
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
#if HGE_DIRECTX_VER == 8
    pD3DDevice->SetIndices(pIB,0);
#endif
#if HGE_DIRECTX_VER == 9
    pD3DDevice->SetIndices(pIB);
#endif

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

#if HGE_DIRECTX_VER == 8
    pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
    if(bTextureFilter) {
        pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
        pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
    } else {
        pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
        pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_POINT);
    }
#endif
#if HGE_DIRECTX_VER == 9
    pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
    if(bTextureFilter) {
        pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    } else {
        pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    }
#endif
    nPrim=0;
    CurPrimType = HGEPRIM_QUADS;

    CurBlendMode = BLEND_DEFAULT;
    // Reset default DirectX Zbuffer write to false
    if (false == bZBuffer) {
        pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }

    CurTexture = NULL;
#if HGE_DIRECTX_VER >= 9
    CurShader = NULL;
#endif

    pD3DDevice->SetTransform(D3DTS_VIEW, &matView);
    pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

    return true;
}

#if HGE_DIRECTX_VER >= 9
HSHADER HGE_CALL HGE_Impl::Shader_Create(const char *filename)
{
    LPD3DXBUFFER					code			= NULL;
    LPDIRECT3DPIXELSHADER9          pixelShader    = NULL;
    HRESULT result = D3DXCompileShaderFromFile( filename,   //filepath
                     NULL,          //macro's
                     NULL,          //includes
                     "ps_main",     //main function
                     "ps_2_0",      //shader profile
                     0,             //flags
                     &code,         //compiled operations
                     NULL,          //errors
                     NULL);         //constants
    if(FAILED(result)) {
        _PostError("Can't create shader");
        return NULL;
    }

    pD3DDevice->CreatePixelShader((DWORD *)code->GetBufferPointer(), &pixelShader);
    code->Release();
    return (HSHADER)pixelShader;
}
#endif

#if HGE_DIRECTX_VER >= 9
void HGE_CALL HGE_Impl::Gfx_SetShader(HSHADER shader)
{
    if (CurShader != shader) {
        _render_batch();
        CurShader = shader;
        pD3DDevice->SetPixelShader((LPDIRECT3DPIXELSHADER9)shader);
    }
}
#endif

#if HGE_DIRECTX_VER >= 9
void HGE_CALL HGE_Impl::Shader_Free(HSHADER shader)
{
    ((LPDIRECT3DPIXELSHADER9)shader)->Release();
}
#endif
