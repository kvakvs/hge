/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeSprite helper class header
*/


#pragma once


#include "hge.h"
#include "hgerect.h"


/*
** HGE Sprite class
*/
class hgeSprite {
public:
    hgeSprite(HTEXTURE tex, float x, float y, float w, float h);
    hgeSprite(const hgeSprite& spr);

    ~hgeSprite() {
        hge_->Release();
    }


    void Render(float x, float y);
    void RenderEx(float x, float y, float rot, float hscale = 1.0f, float vscale = 0.0f);
    void RenderStretch(float x1, float y1, float x2, float y2);
    void Render4V(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);

    void SetTexture(HTEXTURE tex);
    void SetTextureRect(float x, float y, float w, float h, bool adjSize = true);
    void SetColor(uint32_t col, int i = -1);
    void SetZ(float z, int i = -1);

    void SetBlendMode(const int blend) {
        quad_.blend = blend;
    }

    void SetHotSpot(const float x, const float y) {
        hot_x_ = x;
        hot_y_ = y;
    }

    void SetFlip(bool bX, bool bY, bool bHotSpot = false);

    HTEXTURE GetTexture() const {
        return quad_.tex;
    }

    void GetTextureRect(float* x, float* y, float* w, float* h) const {
        *x = tx_;
        *y = ty_;
        *w = width_;
        *h = height_;
    }

    uint32_t GetColor(const int i = 0) const {
        return quad_.v[i].col;
    }

    float GetZ(const int i = 0) const {
        return quad_.v[i].z;
    }

    int GetBlendMode() const {
        return quad_.blend;
    }

    void GetHotSpot(float* x, float* y) const {
        *x = hot_x_;
        *y = hot_y_;
    }

    void GetFlip(bool* bX, bool* bY) const {
        *bX = x_flip_;
        *bY = y_flip_;
    }

    float GetWidth() const {
        return width_;
    }

    float GetHeight() const {
        return height_;
    }

    hgeRect* GetBoundingBox(const float x, const float y, 
                            hgeRect* rect) const {
        rect->Set(x - hot_x_, y - hot_y_, x - hot_x_ + width_, y - hot_y_ + height_);
        return rect;
    }

    hgeRect* GetBoundingBoxEx(float x, float y, float rot, float hscale, float vscale,
                              hgeRect* rect) const;

protected:
    hgeSprite();
    static HGE* hge_;

    hgeQuad quad_;
    float tx_;
    float ty_;
    float width_;
    float height_;
    float tex_width_;
    float tex_height_;
    float hot_x_;
    float hot_y_;
    bool x_flip_;
    bool y_flip_;
    bool hs_flip_;
};
