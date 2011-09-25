#include <hgeunicode.h>

#include <Windows.h>
#include <Winnls.h>


#if HGE_UNICODE
FILE * HGE_CALL hge_fopen_w( hgeConstString s )
{
	return _wfopen( s, L"w,ccs=UNICODE" );
}
#else
FILE * HGE_CALL hge_fopen_w( hgeConstString s )
{
	return fopen(s, "w");
}
#endif



#if HGE_UNICODE
FILE * HGE_CALL hge_fopen_wb( hgeConstString s )
{
	return _wfopen( s, L"wb" );
}
#else
FILE * HGE_CALL hge_fopen_wb( hgeConstString s )
{
	return fopen(s, "wb");
}
#endif



#if HGE_UNICODE
FILE * HGE_CALL hge_fopen_ab( hgeConstString s )
{
	return _wfopen( s, L"ab,ccs=UNICODE" );
}
#else
FILE * HGE_CALL hge_fopen_ab( hgeConstString s )
{
	return fopen(s, "ab");
}
#endif



#if HGE_UNICODE
FILE * HGE_CALL hge_fopen_rb( hgeConstString s )
{
	return _wfopen( s, L"rb" );
}
#else
FILE * HGE_CALL hge_fopen_rb( hgeConstString s )
{
	return fopen(s, "rb");
}
#endif



void HGE_CALL hgeWideToUtf8( const wchar_t * s, char * buf, size_t cnt)
{
	WideCharToMultiByte(CP_ACP, 0, s, wcslen(s)+1, buf, cnt, NULL, NULL);
}



void HGE_CALL hgeUtf8ToWide( const char * s, wchar_t * buf, size_t cnt)
{
	MultiByteToWideChar(CP_ACP, 0, s, strlen(s)+1, buf, cnt);
}

