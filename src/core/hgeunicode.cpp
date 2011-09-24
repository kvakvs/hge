#include <hgeunicode.h>

#if HGE_UNICODE
FILE * hge_fopen_w( hgeString s )
{
	return _wfopen( s, L"w,ccs=UNICODE" );
}
#else
FILE * hge_fopen_w( hgeString s )
{
	return fopen(s, "w");
}
#endif

#if HGE_UNICODE
FILE * hge_fopen_wb( hgeString s )
{
	return _wfopen( s, L"wb" );
}
#else
FILE * hge_fopen_wb( hgeString s )
{
	return fopen(s, "wb");
}
#endif

#if HGE_UNICODE
FILE * hge_fopen_ab( hgeString s )
{
	return _wfopen( s, L"ab,ccs=UNICODE" );
}
#else
FILE * hge_fopen_ab( hgeString s )
{
	return fopen(s, "ab");
}
#endif

#if HGE_UNICODE
FILE * hge_fopen_rb( hgeString s )
{
	return _wfopen( s, L"rb" );
}
#else
FILE * hge_fopen_rb( hgeString s )
{
	return fopen(s, "rb");
}
#endif


void hgeWideToUtf8( const wchar_t * s, char * buf, size_t * cnt)
{
	WideCharToMultiByte(CP_ACP, 0, s, wcslen(s)+1, buf, cnt, NULL, NULL);
}


void hgeUtf8ToWide( const char * s, wchar_t buf, size_t * cnt)
{
	MultiByteToWideChar(CP_ACP, 0, s, strlen(s)+1, buf, cnt);
}
