/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Core functions implementation: random number generation
//


#include "hge_impl.h"

namespace hgeImpl {

    unsigned int g_seed = 0;

// NOLINTNEXTLINE
    void HGE_CALL HGE_Impl::Random_Seed(const int seed) {
      if (!seed) {
        g_seed = timeGetTime();
      } else {
        g_seed = seed;
      }
    }

    int HGE_CALL HGE_Impl::Random_Int(const int min, const int max) {
      g_seed = 214013 * g_seed + 2531011;
      return min + (g_seed ^ g_seed >> 15) % (max - min + 1);
    }

    float HGE_CALL HGE_Impl::Random_Float(const float min, const float max) {
      g_seed = 214013 * g_seed + 2531011;
      //return min+g_seed*(1.0f/4294967295.0f)*(max-min);
      return min + (g_seed >> 16) * (1.0f / 65535.0f) * (max - min);
    }

} // namespace
