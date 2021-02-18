/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// OS- and Compiler-dependent definitions
//
#pragma once

// CMake adds PROJECTNAME_EXPORTS when compiles DLL
#ifdef hge_EXPORTS
#define HGEDLL
#endif
//------

#ifdef HGEDLL
#define HGE_EXPORT  __declspec(dllexport)
#else
#define HGE_EXPORT
#endif

#define HGE_CALL  __stdcall

#ifdef __BORLANDC__
#define floorf (float)floor
#define sqrtf (float)sqrt
#define acosf (float)acos
#define atan2f (float)atan2
#define cosf (float)cos
#define sinf (float)sin
#define powf (float)pow
#define fabsf (float)fabs
#endif


/*
** Common data types; use uint?_t, int?_t, size_t
*/
#include <cstdint>

/*
** Common math constants
*/
#ifndef M_PI
#   define M_PI    3.14159265358979323846f
#   define M_PI_2  1.57079632679489661923f
#   define M_PI_4  0.785398163397448309616f
#   define M_1_PI  0.318309886183790671538f
#   define M_2_PI  0.636619772367581343076f
#endif
