#include <hgeunicode.h>

#include <Windows.h>
#include <Winnls.h>



FILE * HGE_CALL hge_fopen_w(hgeConstString s)
{
#if HGE_UNICODE
	return _wfopen(s, L"w,ccs=UNICODE");
#else
	return fopen(s, "w");
#endif
}



FILE * HGE_CALL hge_fopen_wb(hgeConstString s)
{
#if HGE_UNICODE
	return _wfopen(s, L"wb");
#else
	return fopen(s, "wb");
#endif
}



FILE * HGE_CALL hge_fopen_ab(hgeConstString s)
{
#if HGE_UNICODE
	return _wfopen(s, L"ab,ccs=UNICODE");
#else
	return fopen(s, "ab");
#endif
}



FILE * HGE_CALL hge_fopen_rb(hgeConstString s)
{
#if HGE_UNICODE
	return _wfopen(s, L"rb");
#else
	return fopen(s, "rb");
#endif
}



void HGE_CALL hgeWideToUtf8(const wchar_t * s, char * buf, size_t cnt)
{
	WideCharToMultiByte(CP_ACP, 0, s, wcslen(s) + 1, buf, cnt, NULL, NULL);
}



void HGE_CALL hgeUtf8ToWide(const char * s, wchar_t * buf, size_t cnt)
{
	MultiByteToWideChar(CP_ACP, 0, s, strlen(s) + 1, buf, cnt);
}



void HGE_CALL hgeStrToUtf8(hgeConstString src, char * dst, size_t cnt)
{
#if HGE_UNICODE
	hgeWideToUtf8(src, dst, cnt);
#else
	dst[cnt - 1] = 0;
	strncpy(dst, src, cnt);
#endif
}



void HGE_CALL hgeStrToWide(hgeConstString src, wchar_t * dst, size_t cnt)
{
#if HGE_UNICODE
	dst[cnt - 1] = 0;
	wcsncpy(dst, src, cnt);
#else
	hgeUtf8ToWide(src, dst, cnt);
#endif
}



void HGE_CALL hgeStrFromUtf8(const char * src, hgeString dst, size_t cnt)
{
#if HGE_UNICODE
	hgeUtf8ToWide(src, dst, cnt);
#else
	dst[cnt - 1] = 0;
	strncpy(dst, src, cnt);
#endif
}



void HGE_CALL hgeStrFromWide(const wchar_t * src, hgeString dst, size_t cnt)
{
#if HGE_UNICODE
	dst[cnt - 1] = 0;
	wcsncpy(dst, src, cnt);
#else
	hgeWideToUtf8(src, dst, cnt);
#endif
}
