/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/

#include "hge_impl.h"

namespace hgeImpl {

    float HGE_CALL HGE_Impl::Timer_GetTime() {
      return time_;
    }

    float HGE_CALL HGE_Impl::Timer_GetDelta() {
      return delta_time_;
    }


    int HGE_CALL HGE_Impl::Timer_GetFPS() {
      return fps_;
    }

} // namespace
