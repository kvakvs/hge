/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: ini file
*/


#include "hge_impl.h"


void HGE_CALL HGE_Impl::Ini_SetInt(const hgeString section, const hgeString name, int value)
{
	hgeChar buf[256];

	if(szIniFile[0]) {
		hge_sprintf(buf, TXT("%d"), value);
		WritePrivateProfileStringW( section, name, buf, szIniFile );
	}
}


int HGE_CALL HGE_Impl::Ini_GetInt(const hgeString section, const hgeString name, int def_val)
{
	hgeChar buf[256];

	if(szIniFile[0]) {
		if(GetPrivateProfileStringW(section, name, TXT(""), buf, sizeof(buf), szIniFile)) {
			return hge_strtoi(buf);
		} else {
			return def_val;
		}
	}
	return def_val;
}


void HGE_CALL HGE_Impl::Ini_SetFloat(const hgeString section, const hgeString name, float value)
{
	hgeChar buf[256];

	if(szIniFile[0]) {
		hge_sprintf(buf, TXT("%f"), value);
		WritePrivateProfileStringW( section, name, buf, szIniFile );
	}
}


float HGE_CALL HGE_Impl::Ini_GetFloat(const hgeString section, const hgeString name, float def_val)
{
	hgeChar buf[256];

	if(szIniFile[0]) {
		if(GetPrivateProfileStringW(section, name, TXT(""), buf, sizeof(buf), szIniFile)) {
			return (float)hge_strtof(buf);
		} else { 
			return def_val;
		}
	}
	return def_val;
}


void HGE_CALL HGE_Impl::Ini_SetString(const hgeString section, const hgeString name, const hgeString value)
{
	if(szIniFile[0]) {
		WritePrivateProfileStringW(section, name, value, szIniFile);
	}
}


hgeString HGE_CALL HGE_Impl::Ini_GetString(const hgeString section, const hgeString name, const hgeString def_val)
{
	if(szIniFile[0]) {
		GetPrivateProfileStringW(section, name, def_val, szIniString, sizeof(szIniString), szIniFile);
	} else {
		hge_strcpy(szIniString, def_val);
	}
	return szIniString;
}

