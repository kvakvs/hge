/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once

#include <hge.h>
#include "hgesprite.h"

enum {
    HGETEXT_LEFT = 0,
    HGETEXT_RIGHT = 1,
    HGETEXT_CENTER = 2,
    HGETEXT_HORZMASK = 0x03,
};

enum {
    HGETEXT_TOP = 0,
    HGETEXT_BOTTOM = 4,
    HGETEXT_MIDDLE = 8,
    HGETEXT_VERTMASK = 0x0C,
};

/*
** HGE Font class
*/
class hgeFont {
public:
    hgeFont(const char *filename, bool bMipmap = false);

    ~hgeFont();

    void Render(float x, float y, int align, const char *string);

    void printf(float x, float y, int align, const char *format, ...);

    void printfb(float x, float y, float w, float h, int align, const char *format, ...);

    void SetColor(hgeColor32 col);

    void SetZ(float z);

    void SetBlendMode(hgeBlendMode blend);

    void SetScale(const float scale) {
      scale_ = scale;
    }

    void SetProportion(const float prop) {
      proportion_ = prop;
    }

    void SetRotation(const float rot) {
      rot_ = rot;
    }

    void SetTracking(const float tracking) {
      tracking_ = tracking;
    }

    void SetSpacing(const float spacing) {
      spacing_ = spacing;
    }

    hgeColor32 GetColor() const {
      return col_;
    }

    float GetZ() const {
      return z_;
    }

    int GetBlendMode() const {
      return blend_;
    }

    float GetScale() const {
      return scale_;
    }

    float GetProportion() const {
      return proportion_;
    }

    float GetRotation() const {
      return rot_;
    }

    float GetTracking() const {
      return tracking_;
    }

    float GetSpacing() const {
      return spacing_;
    }

    hgeSprite *GetSprite(const char chr) const {
      return letters_[static_cast<unsigned char>(chr)];
    }

    float GetPreWidth(const char chr) const {
      return pre_[static_cast<unsigned char>(chr)];
    }

    float GetPostWidth(const char chr) const {
      return post_[static_cast<unsigned char>(chr)];
    }

    float GetHeight() const {
      return height_;
    }

    float GetStringWidth(const char *string, bool bMultiline = true) const;

private:
    hgeFont() = default;

    hgeFont(const hgeFont &fnt);

    hgeFont &operator=(const hgeFont &fnt);

    const char *_get_line(const char *file, hgeMUTABLE char *line) const;

    static HGE *hge_;

    static char buffer_[1024];

    HTEXTURE texture_;
    hgeSprite *letters_[256];
    float pre_[256];
    float post_[256];
    float height_;
    float scale_;
    float proportion_;
    float rot_;
    float tracking_;
    float spacing_;

    hgeColor32 col_;
    float z_;
    hgeBlendMode blend_;
};
