/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Core functions implementation: power status
//

#include "hge_impl.h"

namespace hgeImpl {

    void HGE_Impl::init_power_status() {
      krnl32_ = LoadLibrary("kernel32.dll");

      if (krnl32_ != nullptr) {
        get_system_power_status_ = reinterpret_cast<GetSystemPowerStatusFunc>(
                GetProcAddress(krnl32_, "GetSystemPowerStatus"));
      }

      update_power_status();
    }


    void HGE_Impl::update_power_status() {
      SYSTEM_POWER_STATUS ps;

      if (get_system_power_status_ != nullptr && get_system_power_status_(&ps)) {
        if (ps.ACLineStatus == 1) {
          power_status_ = HGEPWR_AC;
        } else if (ps.BatteryFlag < 128) {
          power_status_ = ps.BatteryLifePercent;
        } else {
          power_status_ = HGEPWR_UNSUPPORTED;
        }
      } else {
        power_status_ = HGEPWR_UNSUPPORTED;
      }
    }


    void HGE_Impl::done_power_status() const {
      if (krnl32_ != nullptr) {
        FreeLibrary(krnl32_);
      }
    }

} // namespace
