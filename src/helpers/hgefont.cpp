/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// hgeFont helper class implementation
//

#include <hgefont.h>
#include <cstdio>
#include <string>

const char fnt_header_tag[] = "[HGEFONT]";
const char fnt_bitmap_tag[] = "Bitmap";
const char fnt_char_tag[] = "Char";


HGE *hgeFont::hge_ = nullptr;
char hgeFont::buffer_[1024];


// NOLINTNEXTLINE
hgeFont::hgeFont(const char *font, const bool mipmap) {
  uint32_t size;
  char linebuf[256];
  char buf[MAX_PATH], *pbuf;
  int i, x, y, w, h, a, c;

  // Setup variables

  hge_ = hgeCreate(HGE_VERSION);

  height_ = 0.0f;
  scale_ = 1.0f;
  proportion_ = 1.0f;
  rot_ = 0.0f;
  tracking_ = 0.0f;
  spacing_ = 1.0f;
  texture_ = 0;

  z_ = 0.5f;
  blend_ = (hgeBlendMode)(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
  col_ = hgeColor32::WHITE();

  ZeroMemory(&letters_, sizeof(letters_));
  ZeroMemory(&pre_, sizeof(letters_));
  ZeroMemory(&post_, sizeof(letters_));

  // Load font description

  void *data = hge_->Resource_Load(font, &size);
  if (!data) {
    return;
  }

  std::string desc((const char *)data, size);
  hge_->Resource_Free(data);

  auto pdesc = _get_line(desc.c_str(), linebuf);
  if (strcmp(linebuf, fnt_header_tag) != 0) {
    hge_->System_Log("Font %s has incorrect format.", font);
    return;
  }

  // Parse font description

  while ((pdesc = _get_line(pdesc, linebuf))) {
    if (!strncmp(linebuf, fnt_bitmap_tag, sizeof(fnt_bitmap_tag) - 1)) {
      strcpy(buf, font);
      pbuf = strrchr(buf, '\\');
      if (!pbuf) {
        pbuf = strrchr(buf, '/');
      }
      if (!pbuf) {
        pbuf = buf;
      } else {
        pbuf++;
      }
      if (!sscanf(linebuf, "Bitmap = %s", pbuf)) {
        continue;
      }

      texture_ = hge_->Texture_Load(buf, 0, mipmap);
      if (!texture_) {
        return;
      }
    } else if (!strncmp(linebuf, fnt_char_tag, sizeof(fnt_char_tag) - 1)) {
      pbuf = strchr(linebuf, '=');
      if (!pbuf) {
        continue;
      }
      pbuf++;
      while (*pbuf == ' ') {
        pbuf++;
      }
      if (*pbuf == '\"') {
        pbuf++;
        i = static_cast<unsigned char>(*pbuf++);
        pbuf++; // skip "
      } else {
        i = 0;
        while ((*pbuf >= '0' && *pbuf <= '9') || (*pbuf >= 'A' && *pbuf <= 'F') || (*pbuf >=
                                                                                    'a' &&
                                                                                    *pbuf <= 'f')) {
          char chr = *pbuf;
          if (chr >= 'a') {
            chr -= 'a' - ':';
          }
          if (chr >= 'A') {
            chr -= 'A' - ':';
          }
          chr -= '0';
          if (chr > 0xF) {
            chr = 0xF;
          }
          i = (i << 4) | chr;
          pbuf++;
        }
        if (i < 0 || i > 255) {
          continue;
        }
      }
      sscanf(pbuf, " , %d , %d , %d , %d , %d , %d", &x, &y, &w, &h, &a, &c);

      letters_[i] = new hgeSprite(texture_,
                                  static_cast<float>(x),
                                  static_cast<float>(y),
                                  static_cast<float>(w),
                                  static_cast<float>(h));
      pre_[i] = static_cast<float>(a);
      post_[i] = static_cast<float>(c);
      if (h > height_) {
        height_ = static_cast<float>(h);
      }
    }
  }
}


hgeFont::~hgeFont() {
  for (auto &letter : letters_)
    delete letter;

  if (texture_) {
    hge_->Texture_Free(texture_);
  }
  hge_->Release();
}

void hgeFont::Render(const float x, float y, int align, const char *string) {
  float fx = x;

  align &= HGETEXT_HORZMASK;
  if (align == HGETEXT_RIGHT) {
    fx -= GetStringWidth(string, false);
  }
  if (align == HGETEXT_CENTER) {
    fx -= int(GetStringWidth(string, false) / 2.0f);
  }

  while (*string) {
    if (*string == '\n') {
      y += int(height_ * scale_ * spacing_);
      fx = x;
      if (align == HGETEXT_RIGHT) {
        fx -= GetStringWidth(string + 1, false);
      }
      if (align == HGETEXT_CENTER) {
        fx -= int(GetStringWidth(string + 1, false) / 2.0f);
      }
    } else {
      int i = static_cast<unsigned char>(*string);
      if (!letters_[i]) {
        i = '?';
      }
      if (letters_[i]) {
        fx += pre_[i] * scale_ * proportion_;
        letters_[i]->RenderEx(fx, y, rot_, scale_ * proportion_, scale_);
        fx += (letters_[i]->GetWidth() + post_[i] + tracking_) * scale_ * proportion_;
      }
    }
    string++;
  }
}

void hgeFont::printf(const float x, const float y, const int align,
                     const char *format, ...) {
  const auto p_arg = reinterpret_cast<char *>(&format) + sizeof(format);

  _vsnprintf(buffer_, sizeof(buffer_) - 1, format, p_arg);
  buffer_[sizeof(buffer_) - 1] = 0;
  //vsprintf(buffer, format, pArg);

  Render(x, y, align, buffer_);
}

void hgeFont::printfb(const float x, const float y, const float w, const float h,
                      const int align, const char *format, ...) {
  auto lines = 0;
  const auto p_arg = reinterpret_cast<char *>(&format) + sizeof(format);

  _vsnprintf(buffer_, sizeof(buffer_) - 1, format, p_arg);
  buffer_[sizeof(buffer_) - 1] = 0;
  //vsprintf(buffer, format, pArg);

  char *linestart = buffer_;
  char *pbuf = buffer_;
  char *prevword = nullptr;

  for (;;) {
    int i = 0;
    while (pbuf[i] && pbuf[i] != ' ' && pbuf[i] != '\n') {
      i++;
    }

    const auto chr = pbuf[i];
    pbuf[i] = 0;
    const auto ww = GetStringWidth(linestart);
    pbuf[i] = chr;

    if (ww > w) {
      if (pbuf == linestart) {
        pbuf[i] = '\n';
        linestart = &pbuf[i + 1];
      } else {
        *prevword = '\n';
        linestart = prevword + 1;
      }

      lines++;
    }

    if (pbuf[i] == '\n') {
      prevword = &pbuf[i];
      linestart = &pbuf[i + 1];
      pbuf = &pbuf[i + 1];
      lines++;
      continue;
    }

    if (!pbuf[i]) {
      lines++;
      break;
    }

    prevword = &pbuf[i];
    pbuf = &pbuf[i + 1];
  }

  auto tx = x;
  auto ty = y;
  const auto hh = height_ * spacing_ * scale_ * lines;

  switch (align & HGETEXT_HORZMASK) {
    case HGETEXT_LEFT:
      break;
    case HGETEXT_RIGHT:
      tx += w;
      break;
    case HGETEXT_CENTER:
      tx += int(w / 2);
      break;
  }

  switch (align & HGETEXT_VERTMASK) {
    case HGETEXT_TOP:
      break;
    case HGETEXT_BOTTOM:
      ty += h - hh;
      break;
    case HGETEXT_MIDDLE:
      ty += int((h - hh) / 2);
      break;
  }

  Render(tx, ty, align, buffer_);
}

float hgeFont::GetStringWidth(const char *string, const bool b_multiline) const {
  float w = 0;

  while (*string) {
    float linew = 0;

    while (*string && *string != '\n') {
      int i = static_cast<unsigned char>(*string);
      if (!letters_[i]) {
        i = '?';
      }
      if (letters_[i]) {
        linew += letters_[i]->GetWidth() + pre_[i] + post_[i] + tracking_;
      }

      string++;
    }

    if (!b_multiline) {
      return linew * scale_ * proportion_;
    }

    if (linew > w) {
      w = linew;
    }

    while (*string == '\n' || *string == '\r') {
      string++;
    }
  }

  return w * scale_ * proportion_;
}

void hgeFont::SetColor(const hgeColor32 col) {
  col_ = col;

  for (auto &letter : letters_)
    if (letter) {
      letter->SetColor(col);
    }
}

void hgeFont::SetZ(const float z) {
  z_ = z;

  for (auto &letter : letters_)
    if (letter) {
      letter->SetZ(z);
    }
}

void hgeFont::SetBlendMode(const hgeBlendMode blend) {
  blend_ = blend;

  for (auto &letter : letters_) {
    if (letter) {
      letter->SetBlendMode(blend);
    }
  }
}

const char *hgeFont::_get_line(const char *file, char *line) const {
  auto i = 0;

  if (!file[i]) {
    return nullptr;
  }

  while (file[i] && file[i] != '\n' && file[i] != '\r') {
    line[i] = file[i];
    i++;
  }
  line[i] = 0;

  while (file[i] && (file[i] == '\n' || file[i] == '\r')) {
    i++;
  }

  return file + i;
}
