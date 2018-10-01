/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: power status
*/


#include "hge_impl.h"


void HGE_Impl::_InitPowerStatus() {
    krnl32_ = LoadLibrary("kernel32.dll");

    if (krnl32_ != nullptr) {
        get_system_power_status_ = reinterpret_cast<GetSystemPowerStatusFunc>(
            GetProcAddress(krnl32_, "GetSystemPowerStatus"));
    }

    _UpdatePowerStatus();
}


void HGE_Impl::_UpdatePowerStatus() {
    SYSTEM_POWER_STATUS ps;

    if (get_system_power_status_ != nullptr && get_system_power_status_(&ps)) {
        if (ps.ACLineStatus == 1) {
            power_status_ = HGEPWR_AC;
        }
        else if (ps.BatteryFlag < 128) {
            power_status_ = ps.BatteryLifePercent;
        }
        else {
            power_status_ = HGEPWR_UNSUPPORTED;
        }
    }
    else {
        power_status_ = HGEPWR_UNSUPPORTED;
    }
}


void HGE_Impl::_DonePowerStatus() const {
    if (krnl32_ != nullptr) {
        FreeLibrary(krnl32_);
    }
}
