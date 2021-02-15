#include "fonted.h"

CHAR_DESC vChars[256];


bool place_symbols(const int n_width, const int n_height,
                   CSymbolRange *p_ranges, const int n_range_count) {
  auto x = 1;
  auto y = 1;

  for (auto i = 0; i < n_range_count; i++) {
    for (int j = p_ranges[i].First; j <= p_ranges[i].Last; j++) {
      if (y + vChars[j].h + 1 >= n_height) {
        return false;
      }
      if (x + vChars[j].w + 1 >= n_width) {
        x = 1;
        y += vChars[j].h + 1;
        if (y + vChars[j].h + 1 >= n_height) {
          return false;
        }
      }

      vChars[j].x = x;
      vChars[j].y = y;
      x += vChars[j].w + 1;
    }
  }

  return true;
}

HTEXTURE generate_font(const char *sz_font_name, const int n_size,
                       const int n_padding_top, const int n_padding_btm,
                       const int n_padding_lft, const int n_padding_rgt,
                       const bool b_bold, const bool b_italic,
                       const bool b_antialias, CSymbolRange *p_ranges,
                       const int n_range_count) {
  int i, j;

  BITMAPINFO bmi;
  ABCFLOAT abc;
  TEXTMETRIC tTextMetrics;

  uint32_t *pPixels;

  // create font
  const auto h_font = CreateFont(
          -n_size, 0, 0, 0, (b_bold) ? FW_BOLD : FW_NORMAL,
          b_italic, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
          CLIP_DEFAULT_PRECIS,
          (b_antialias) ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
          DEFAULT_PITCH | FF_DONTCARE, sz_font_name);

  if (!h_font) {
    return 0;
  }

  // create and setup compatible DC
  const auto h_bmdc = CreateCompatibleDC(nullptr);
  SetTextColor(h_bmdc, RGB(255, 255, 255));
  SetBkColor(h_bmdc, RGB(0, 0, 0));
  SetBkMode(h_bmdc, TRANSPARENT);
  SetMapMode(h_bmdc, MM_TEXT);
  SetTextAlign(h_bmdc, TA_TOP);
  SelectObject(h_bmdc, h_font);

  // calculate symbols metrics
  GetTextMetrics(h_bmdc, &tTextMetrics);

  for (i = 0; i < n_range_count; i++) {
    for (j = p_ranges[i].First; j <= p_ranges[i].Last; j++) {
      GetCharABCWidthsFloat(h_bmdc, j, j, &abc);

      // reserve pixels for antialiasing
      vChars[j].a = int(abc.abcfA) - 1;
      vChars[j].c = int(abc.abcfC) - 1;
      vChars[j].w = int(abc.abcfB) + 2 + n_padding_lft + n_padding_rgt;
      vChars[j].h = tTextMetrics.tmHeight + n_padding_top + n_padding_btm;
    }
  }

  // calculate symbols placement
  auto n_width = 32;
  auto n_height = 32;

  for (;;) {
    if (place_symbols(n_width, n_height, p_ranges, n_range_count)) {
      break;
    }

    if (n_width <= n_height) {
      n_width <<= 1;
    } else {
      n_height <<= 1;
    }

    if (n_width > MAX_TEXTURE_SIZE || n_height > MAX_TEXTURE_SIZE) {
      DeleteObject(h_font);
      DeleteDC(h_bmdc);
      return 0;
    }
  }

  // create DC bitmap
  memset(&bmi, 0, sizeof(BITMAPINFO));
  bmi.bmiHeader.biWidth = n_width;
  bmi.bmiHeader.biHeight = -n_height;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

  const auto h_bm = CreateDIBSection(h_bmdc, &bmi, DIB_RGB_COLORS,
                                     reinterpret_cast<void **>(&pPixels), nullptr, 0);
  if (!h_bm) {
    DeleteObject(h_font);
    DeleteDC(h_bmdc);
    return 0;
  }

  SelectObject(h_bmdc, h_bm);

  // draw symbols onto DC bitmap
  for (i = 0; i < n_range_count; i++) {
    for (j = p_ranges[i].First; j <= p_ranges[i].Last; j++) {
      auto c = static_cast<char>(j);
      TextOut(h_bmdc, vChars[j].x - vChars[j].a + n_padding_lft, vChars[j].y + n_padding_top,
              &c,
              1);
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
  const auto tex = hge->Texture_Create(n_width, n_height);
  uint32_t *pTexData = hge->Texture_Lock(tex, false);

  for (i = 0; i < n_height; i++) {
    for (j = 0; j < n_width; j++) {
      uint32_t dwPixel = pPixels[i * n_width + j];
      dwPixel = 0xFFFFFF | ((dwPixel & 0xFF) << 24);
      pTexData[i * n_width + j] = dwPixel;
    }
  }

  hge->Texture_Unlock(tex);

  // clean up
  DeleteObject(h_bm);
  DeleteObject(h_font);
  DeleteDC(h_bmdc);

  return tex;
}
