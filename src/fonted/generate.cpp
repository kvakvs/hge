
#include "fonted.h"

CHAR_DESC vChars[256];


bool PlaceSymbols(int nWidth, int nHeight, CSymbolRange *pRanges, int nRangeCount)
{
	int i, j;
	int x=1, y=1;

	for(i=0; i<nRangeCount; i++)
	{
		for(j=pRanges[i].First; j<=pRanges[i].Last; j++ )
		{
			if(y+vChars[j].h+1 >= nHeight) return false;
			if(x+vChars[j].w+1 >= nWidth)
			{
				x=1;
				y+=vChars[j].h+1;
				if(y+vChars[j].h+1 >= nHeight) return false;
			}

			vChars[j].x = x;
			vChars[j].y = y;
			x+=vChars[j].w+1;
		}
	}

	return true;
}

HTEXTURE FontGenerate(char *szFontName,
					  int nSize,
					  int nPaddingTop, int nPaddingBtm, int nPaddingLft, int nPaddingRgt,
					  bool bBold,
					  bool bItalic,
					  bool bAntialias,
					  CSymbolRange *pRanges,
					  int nRangeCount)
{
	int i,j;
	int nWidth, nHeight;

	HDC			hBMDC;
	HBITMAP		hBM;
	BITMAPINFO	bmi;
	HFONT		hFont;
	ABCFLOAT	abc;
	TEXTMETRIC	tTextMetrics;

	HTEXTURE	tex;
	DWORD		*pPixels, *pTexData, dwPixel;

	// create font
	hFont = CreateFont(-nSize, 0, 0, 0, (bBold) ? FW_BOLD : FW_NORMAL,
		bItalic, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		(bAntialias) ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, szFontName);

	if(!hFont) return 0;

	// create and setup compatible DC
	hBMDC = CreateCompatibleDC(0);
	SetTextColor(hBMDC, RGB(255,255,255));
	SetBkColor(hBMDC, RGB(0,0,0));
	SetBkMode(hBMDC, TRANSPARENT);
	SetMapMode(hBMDC, MM_TEXT);
    SetTextAlign(hBMDC, TA_TOP);
	SelectObject(hBMDC, hFont);

	// calculate symbols metrics
	GetTextMetrics(hBMDC, &tTextMetrics);

	for (i = 0; i < nRangeCount; i++ )
	{
		for (j = pRanges[i].First; j <= pRanges[i].Last; j++ )
		{
			GetCharABCWidthsFloat(hBMDC, j, j, &abc);

			// reserve pixels for antialiasing
			vChars[j].a = int(abc.abcfA)-1;
			vChars[j].c = int(abc.abcfC)-1;
			vChars[j].w = int(abc.abcfB)+2 + nPaddingLft+nPaddingRgt;
			vChars[j].h = tTextMetrics.tmHeight + nPaddingTop+nPaddingBtm;
		}
	}

	// calculate symbols placement
	nWidth=32; nHeight=32;

	for(;;)
	{
		if(PlaceSymbols(nWidth, nHeight, pRanges, nRangeCount)) break;

		if(nWidth<=nHeight) nWidth<<=1;
		else nHeight<<=1;

		if(nWidth > MAX_TEXTURE_SIZE || nHeight > MAX_TEXTURE_SIZE)
		{
			DeleteObject(hFont);
			DeleteDC(hBMDC);
			return 0;
		}
	}

	// create DC bitmap
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biWidth = nWidth;
	bmi.bmiHeader.biHeight = -nHeight;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	hBM = CreateDIBSection(hBMDC, &bmi, DIB_RGB_COLORS, (void**)&pPixels, 0, 0);
	if(!hBM)
	{
		DeleteObject(hFont);
		DeleteDC(hBMDC);
		return 0;
	}

	SelectObject(hBMDC, hBM);

	// draw symbols onto DC bitmap
	for (i = 0; i < nRangeCount; i++ )
	{
		for (j = pRanges[i].First; j <= pRanges[i].Last; j++ )
		{
			char c = (char)j;
			TextOut(hBMDC, vChars[j].x-vChars[j].a+nPaddingLft, vChars[j].y+nPaddingTop, &c, 1);
		}
	}
	GdiFlush();

/*
Graphics    graphics(hdc);
FontFamily  fontFamily(L"Times New Roman");
Font        font(&fontFamily, 32, FontStyleRegular, UnitPixel);
SolidBrush  solidBrush(Color(255, 0, 0, 255));
WCHAR       string1[] = L"SingleBitPerPixel";
WCHAR       string2[] = L"AntiAlias";

graphics.SetTextRenderingHint(TextRenderingHintSingleBitPerPixel);
graphics.DrawString(string1, -1, &font, PointF(10.0f, 10.0f), &solidBrush);

graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
graphics.DrawString(string2, -1, &font, PointF(10.0f, 60.0f), &solidBrush);
*/

	// transfer DC bitmap onto HGE texture with alpha channel
	tex=hge->Texture_Create(nWidth, nHeight);
	pTexData=hge->Texture_Lock(tex, false);

	for (i=0; i<nHeight; i++)
	{
		for (j=0; j<nWidth; j++)
		{
			dwPixel = pPixels[i*nWidth+j];
			dwPixel = 0xFFFFFF | ((dwPixel & 0xFF) << 24);
			pTexData[i*nWidth+j] = dwPixel;
		}
	}

	hge->Texture_Unlock(tex);

	// clean up
	DeleteObject(hBM);
	DeleteObject(hFont);
	DeleteDC(hBMDC);
	
	return tex;
}
