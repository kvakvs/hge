/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once

//
// HGE Handle types
// Handles hide a pointer which refers to an internal HGE struct, or BASS library struct
//

// FIXME: Won't compile in 64-bit mode due to handles (4 bytes) holding a pointer (8 bytes)
using HTEXTURE = size_t;
using HTARGET = size_t;
using HSHADER = size_t;
using HEFFECT = size_t;

using HMUSIC = uint32_t; // defined by bass.h
using HSTREAM = uint32_t; // defined by bass.h
using HCHANNEL = uint32_t; // defined by bass.h
