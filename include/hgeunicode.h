// TODO: Hide all this extra stuff in hge:: namespace
#pragma once

#include <stdio.h>

#if HGE_UNICODE
	typedef wchar_t hgeChar;
	typedef wchar_t * hgeString;

	#define TXT(S)			L##S
	#define hge_strtoi		_wtoi
	#define hge_strtof(a)	wcstod(a, NULL)
	#define hge_strcpy		wcscpy
	#define hge_strncpy		wcsncpy
	#define hge_strcat		wcscat
	#define hge_strupr		wcsupr
	#define hge_strcmp		wcscmp
	#define hge_strlen		wcslen
	#define hge_strrchr		wcsrchr
	#define hge_sprintf		swprintf
	#define hge_vsnprintf	_vsnwprintf
	#define hge_vsprintf	vswprintf
	#define hge_fprintf		fwprintf

	FILE * hge_fopen_w( hgeString s );
	FILE * hge_fopen_wb( hgeString s );
	FILE * hge_fopen_ab( hgeString s );
	FILE * hge_fopen_rb( hgeString s );
#else
	typedef char hgeChar;
	typedef char * hgeString;

	#define TXT(S)			S
	#define hge_strtoi		atoi
	#define hge_strtof(a)	atof(a)
	#define hge_strcpy		strcpy
	#define hge_strncpy		strncpy
	#define hge_strcat		strcat
	#define hge_strupr		strupr
	#define hge_strcmp		strcmp
	#define hge_strlen		strlen
	#define hge_strrchr		strrchr
	#define hge_sprintf		sprintf
	#define hge_vsnprintf	_vsnprintf
	#define hge_vsprintf	vsprintf
	#define hge_fprintf		fprintf

	FILE * hge_fopen_w( hgeString s );
	FILE * hge_fopen_wb( hgeString s );
	FILE * hge_fopen_ab( hgeString s );
	FILE * hge_fopen_rb( hgeString s );
#endif

void hgeWideToUtf8( const wchar_t * s, char * buf, size_t * cnt);
void hgeUtf8ToWide( const char * s, wchar_t buf, size_t * cnt);
