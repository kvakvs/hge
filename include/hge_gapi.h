/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
// Modifications and additional types, included internally, do not include from your game project
// GAPI dependent includes and defines (DX8/DX9 switch)
// added by kvakvs@yandex.ru

#include <d3d9.h>
#include <d3dx9.h>

using hgeGAPI = IDirect3D9;
using hgeGAPIDevice = IDirect3DDevice9;
using hgeGAPIVertexBuffer = IDirect3DVertexBuffer9;
using hgeGAPIIndexBuffer = IDirect3DIndexBuffer9;
using hgeGAPITexture = IDirect3DTexture9;
using hgeGAPISurface = IDirect3DSurface9;
using hgeGAPIViewport = D3DVIEWPORT9;
using hgeGAPIAdapterIdentifier = D3DADAPTER_IDENTIFIER9;
using hgeGAPICaps = D3DCAPS9;
