/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: ini file
*/


#include "hge_impl.h"


void HGE_CALL HGE_Impl::Ini_SetInt(hgeConstString section, hgeConstString name, int value)
{
	hgeChar buf[256];

	if(szIniFile[0]) {
		hge_sprintf( buf, sizeof buf, TXT("%d"), value );
		HGE_WINAPI_UNICODE_SUFFIX(WritePrivateProfileString)( section, name, buf, szIniFile );
	}
}


int HGE_CALL HGE_Impl::Ini_GetInt(hgeConstString section, hgeConstString name, int def_val)
{
	hgeChar buf[256];

	if(szIniFile[0]) {
		if( HGE_WINAPI_UNICODE_SUFFIX(GetPrivateProfileString)(
			section, name, TXT(""), buf, sizeof(buf), szIniFile)
			)
		{
			return hge_strtoi(buf);
		} else {
			return def_val;
		}
	}
	return def_val;
}


void HGE_CALL HGE_Impl::Ini_SetFloat(hgeConstString section, hgeConstString name, float value)
{
	hgeChar buf[256];

	if(szIniFile[0]) {
		hge_sprintf(buf, sizeof buf, TXT("%f"), value);
		HGE_WINAPI_UNICODE_SUFFIX(WritePrivateProfileString)( section, name, buf, szIniFile );
	}
}


float HGE_CALL HGE_Impl::Ini_GetFloat(hgeConstString section, hgeConstString name, float def_val)
{
	hgeChar buf[256];

	if(szIniFile[0]) {
		if( HGE_WINAPI_UNICODE_SUFFIX(GetPrivateProfileString)(
				section, name, TXT(""), buf, sizeof(buf), szIniFile ))
		{
			return (float)hge_strtof(buf);
		} else { 
			return def_val;
		}
	}
	return def_val;
}


void HGE_CALL HGE_Impl::Ini_SetString(hgeConstString section, hgeConstString name, hgeConstString value)
{
	if(szIniFile[0]) {
		HGE_WINAPI_UNICODE_SUFFIX(WritePrivateProfileString)(section, name, value, szIniFile);
	}
}


hgeString HGE_CALL HGE_Impl::Ini_GetString(hgeConstString section, hgeConstString name, hgeConstString def_val)
{
	if(szIniFile[0]) {
		HGE_WINAPI_UNICODE_SUFFIX(GetPrivateProfileString)(
			section, name, def_val, szIniString, sizeof(szIniString), szIniFile
			);
	} else {
		hge_strcpy(szIniString, def_val);
	}
	return szIniString;
}

