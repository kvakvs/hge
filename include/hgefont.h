/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeFont helper class header
*/


#pragma once


#include "hge.h"
#include "hgesprite.h"


#define HGETEXT_LEFT		0
#define HGETEXT_RIGHT		1
#define HGETEXT_CENTER		2
#define HGETEXT_HORZMASK	0x03

#define HGETEXT_TOP			0
#define HGETEXT_BOTTOM		4
#define HGETEXT_MIDDLE		8
#define HGETEXT_VERTMASK	0x0C

/*
** HGE Font class
*/
class hgeFont {
public:
    hgeFont(const char* filename, bool bMipmap = false);
    ~hgeFont();

    void Render(float x, float y, int align, const char* string);
    void printf(float x, float y, int align, const char* format, ...);
    void printfb(float x, float y, float w, float h, int align, const char* format, ...);

    void SetColor(uint32_t col);
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

    uint32_t GetColor() const {
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

    hgeSprite* GetSprite(const char chr) const {
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

    float GetStringWidth(const char* string, bool bMultiline = true) const;

private:
    hgeFont();
    hgeFont(const hgeFont& fnt);
    hgeFont& operator=(const hgeFont& fnt);

    char* _get_line(char* file, char* line);

    static HGE* hge_;

    static char buffer_[1024];

    HTEXTURE texture_;
    hgeSprite* letters_[256];
    float pre_[256];
    float post_[256];
    float height_;
    float scale_;
    float proportion_;
    float rot_;
    float tracking_;
    float spacing_;

    uint32_t col_;
    float z_;
    hgeBlendMode blend_;
};
