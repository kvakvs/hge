/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.8.1
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
// Modifications and additional types, included internally, do not include from your game project
// GAPI dependent includes and defines (DX8/DX9 switch)
// added by kvakvs@yandex.ru

#if HGE_DIRECTX_VER == 8
    #error "DirectX8 support was removed, please use DirectX9"
    #include <d3d8.h>
    #include <d3dx8.h>
#else
    #if HGE_DIRECTX_VER == 9
        #include <d3d9.h>
        #include <d3dx9.h>
    #else
        #error Please change your CMakeLists.txt; set HGE_DIRECTX_VER to be 8 or 9, or edit C++ preprocessor definitions settings
    #endif
#endif

#if HGE_DIRECTX_VER == 8
    typedef IDirect3D8              hgeGAPI;
    typedef IDirect3DDevice8        hgeGAPIDevice;
    typedef IDirect3DVertexBuffer8  hgeGAPIVertexBuffer;
    typedef IDirect3DIndexBuffer8   hgeGAPIIndexBuffer;
    typedef IDirect3DTexture8       hgeGAPITexture;
    typedef IDirect3DSurface8       hgeGAPISurface;
    typedef D3DVIEWPORT8            hgeGAPIViewport;
    typedef D3DADAPTER_IDENTIFIER8  hgeGAPIAdapterIdentifier;
	typedef D3DCAPS8				hgeGAPICaps;
#endif

#if HGE_DIRECTX_VER == 9
    typedef IDirect3D9              hgeGAPI;
    typedef IDirect3DDevice9        hgeGAPIDevice;
    typedef IDirect3DVertexBuffer9  hgeGAPIVertexBuffer;
    typedef IDirect3DIndexBuffer9   hgeGAPIIndexBuffer;
    typedef IDirect3DTexture9       hgeGAPITexture;
    typedef IDirect3DSurface9       hgeGAPISurface;
    typedef D3DVIEWPORT9            hgeGAPIViewport;
    typedef D3DADAPTER_IDENTIFIER9  hgeGAPIAdapterIdentifier;
	typedef D3DCAPS9				hgeGAPICaps;
#endif

