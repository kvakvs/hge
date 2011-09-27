/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: ini file
*/


#include "hge_impl.h"


void HGE_CALL HGE_Impl::Ini_SetInt(hgeConstString section, hgeConstString name,
		int value)
{
	hgeChar buf[256];

	if (m_ini_file_path[0])
	{
		hge_sprintf(buf, sizeof buf, TXT("%d"), value);
		HGE_WINAPI_UNICODE_SUFFIX(WritePrivateProfileString)(section, name,
				buf, m_ini_file_path);
	}
}


int HGE_CALL HGE_Impl::Ini_GetInt(hgeConstString section, hgeConstString name,
		int def_val)
{
	hgeChar buf[256];

	if (m_ini_file_path[0])
	{
		if (HGE_WINAPI_UNICODE_SUFFIX(GetPrivateProfileString)(section, name,
				TXT(""), buf, sizeof(buf), m_ini_file_path))
		{
			return hge_strtoi(buf);
		}
		else
		{
			return def_val;
		}
	}
	return def_val;
}


void HGE_CALL HGE_Impl::Ini_SetFloat(hgeConstString section,
		hgeConstString name, float value)
{
	hgeChar buf[256];

	if (m_ini_file_path[0])
	{
		hge_sprintf(buf, sizeof buf, TXT("%f"), value);
		HGE_WINAPI_UNICODE_SUFFIX(WritePrivateProfileString)(section, name,
				buf, m_ini_file_path);
	}
}


float HGE_CALL HGE_Impl::Ini_GetFloat(hgeConstString section,
		hgeConstString name, float def_val)
{
	hgeChar buf[256];

	if (m_ini_file_path[0])
	{
		if (HGE_WINAPI_UNICODE_SUFFIX(GetPrivateProfileString)(section, name,
				TXT(""), buf, sizeof(buf), m_ini_file_path))
		{
			return (float) hge_strtof(buf);
		}
		else
		{
			return def_val;
		}
	}
	return def_val;
}


void HGE_CALL HGE_Impl::Ini_SetString(hgeConstString section,
		hgeConstString name, hgeConstString value)
{
	if (m_ini_file_path[0])
	{
		HGE_WINAPI_UNICODE_SUFFIX(WritePrivateProfileString)(section, name,
				value, m_ini_file_path);
	}
}


hgeString HGE_CALL HGE_Impl::Ini_GetString(hgeConstString section,
		hgeConstString name, hgeConstString def_val)
{
	if (m_ini_file_path[0])
	{
		HGE_WINAPI_UNICODE_SUFFIX(GetPrivateProfileString)(section, name,
				def_val, m_ini_string, sizeof(m_ini_string), m_ini_file_path);
	}
	else
	{
		hge_strcpy(m_ini_string, def_val);
	}
	return m_ini_string;
}

