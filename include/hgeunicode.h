// TODO: Hide all this extra stuff in hge:: namespace
#pragma once

#define _CRT_NON_CONFORMING_SWPRINTFS
#include <stdio.h>

#include <hgeimportexport.h>

#if HGE_UNICODE
	typedef wchar_t hgeChar;
	typedef wchar_t * hgeString;
	typedef const wchar_t * hgeConstString;

	#define TXT(S)			L##S
	#define hge_strtoi		_wtoi
	#define hge_strtof(a)	wcstod(a, NULL)
	#define hge_strcpy		wcscpy
	#define hge_strncpy		wcsncpy
	#define hge_strcat		wcscat
	#define hge_strupr		wcsupr
	#define hge_strcmp		wcscmp
	#define hge_strncmp		wcsncmp
	#define hge_strlen		wcslen
	#define hge_strrchr		wcsrchr
	#define hge_sprintf		swprintf
	#define hge_printf		wprintf
	#define hge_vsnprintf	_vsnwprintf
	#define hge_vsprintf	vswprintf
	#define hge_fprintf		fwprintf
	#define hge_vfprintf	vfwprintf
	#define hge_getcwd		_wgetcwd
	#define hge_chdir		_wchdir
	#define hge_sscanf		swscanf
	#define HGE_WINAPI_UNICODE_SUFFIX(Func) Func##W
#else
	typedef char hgeChar;
	typedef char * hgeString;
	typedef const char * hgeConstString;

	#define TXT(S)			S
	#define hge_strtoi		atoi
	#define hge_strtof(a)	atof(a)
	#define hge_strcpy		strcpy
	#define hge_strncpy		strncpy
	#define hge_strcat		strcat
	#define hge_strupr		strupr
	#define hge_strcmp		strcmp
	#define hge_strncmp		strncmp
	#define hge_strlen		strlen
	#define hge_strrchr		strrchr
	#define hge_sprintf		sprintf
	#define hge_printf		printf
	#define hge_vsnprintf	_vsnprintf
	#define hge_vsprintf	vsprintf
	#define hge_fprintf		fprintf
	#define hge_vfprintf	vfprintf
	#define hge_getcwd		_getcwd
	#define hge_chdir		_chdir
	#define hge_sscanf		sscanf
	#define HGE_WINAPI_UNICODE_SUFFIX(Func) Func##A
#endif

extern "C" { 
	HGE_EXPORT FILE *	HGE_CALL hge_fopen_w( hgeConstString s );
	HGE_EXPORT FILE *	HGE_CALL hge_fopen_wb( hgeConstString s );
	HGE_EXPORT FILE *	HGE_CALL hge_fopen_ab( hgeConstString s );
	HGE_EXPORT FILE *	HGE_CALL hge_fopen_rb( hgeConstString s );
	HGE_EXPORT void		HGE_CALL hgeWideToUtf8( const wchar_t * s, char * buf, size_t cnt);
	HGE_EXPORT void		HGE_CALL hgeUtf8ToWide( const char * s, wchar_t * buf, size_t cnt);
}
