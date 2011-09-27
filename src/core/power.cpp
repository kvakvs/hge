/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: power status
*/


#include "hge_impl.h"


void HGE_Impl::_InitPowerStatus()
{
	m_kernel32_module = LoadLibrary("kernel32.dll");

	if (m_kernel32_module != NULL)
		m_GetSystemPowerStatus_func = (GetSystemPowerStatusFunc) GetProcAddress(
				m_kernel32_module, "GetSystemPowerStatus");

	_UpdatePowerStatus();
}



void HGE_Impl::_UpdatePowerStatus()
{
	SYSTEM_POWER_STATUS ps;

	if (m_GetSystemPowerStatus_func != NULL && m_GetSystemPowerStatus_func(&ps))
	{
		if (ps.ACLineStatus == 1)
		{
			m_power_status = HGEPWR_AC;
		}
		else if (ps.BatteryFlag < 128)
		{
			m_power_status = ps.BatteryLifePercent;
		}
		else
		{
			m_power_status = HGEPWR_UNSUPPORTED;
		}
	}
	else
	{
		m_power_status = HGEPWR_UNSUPPORTED;
	}
}



void HGE_Impl::_DonePowerStatus()
{
	if (m_kernel32_module != NULL)
		FreeLibrary(m_kernel32_module);
}
