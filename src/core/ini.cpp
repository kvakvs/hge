/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: ini file
*/


#include "hge_impl.h"


void CALL HGE_Impl::Ini_SetInt(const char *section, const char *name, int value)
{
	char buf[256];

	if(szIniFile[0]) {
		sprintf(buf,"%d",value);
		WritePrivateProfileString(section, name, buf, szIniFile);
	}
}


int CALL HGE_Impl::Ini_GetInt(const char *section, const char *name, int def_val)
{
	char buf[256];

	if(szIniFile[0]) {
		if(GetPrivateProfileString(section, name, "", buf, sizeof(buf), szIniFile))
		{ return atoi(buf); }
		else { return def_val; }
	}
	return def_val;
}


void CALL HGE_Impl::Ini_SetFloat(const char *section, const char *name, float value)
{
	char buf[256];

	if(szIniFile[0]) {
		sprintf(buf,"%f",value);
		WritePrivateProfileString(section, name, buf, szIniFile);
	}
}


float CALL HGE_Impl::Ini_GetFloat(const char *section, const char *name, float def_val)
{
	char buf[256];

	if(szIniFile[0]) {
		if(GetPrivateProfileString(section, name, "", buf, sizeof(buf), szIniFile))
		{ return (float)atof(buf); }
		else { return def_val; }
	}
	return def_val;
}


void CALL HGE_Impl::Ini_SetString(const char *section, const char *name, const char *value)
{
	if(szIniFile[0]) WritePrivateProfileString(section, name, value, szIniFile);
}


char* CALL HGE_Impl::Ini_GetString(const char *section, const char *name, const char *def_val)
{
	if(szIniFile[0]) GetPrivateProfileString(section, name, def_val, szIniString, sizeof(szIniString), szIniFile);
	else strcpy(szIniString, def_val);
	return szIniString;
}

