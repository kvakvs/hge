/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeSprite helper class implementation
*/


#include "../../include/hgesprite.h"
#include <math.h>


HGE* hgeSprite::hge = nullptr;


hgeSprite::hgeSprite(const HTEXTURE texture, const float texx, const float texy,
                     const float w, const float h) {

    hge = hgeCreate(HGE_VERSION);

    tx = texx;
    ty = texy;
    width = w;
    height = h;

    if (texture) {
        tex_width = static_cast<float>(hge->Texture_GetWidth(texture));
        tex_height = static_cast<float>(hge->Texture_GetHeight(texture));
    }
    else {
        tex_width = 1.0f;
        tex_height = 1.0f;
    }

    hotX = 0;
    hotY = 0;
    bXFlip = false;
    bYFlip = false;
    bHSFlip = false;
    quad.tex = texture;

    const auto texx1 = texx / tex_width;
    const auto texy1 = texy / tex_height;
    const auto texx2 = (texx + w) / tex_width;
    const auto texy2 = (texy + h) / tex_height;

    quad.v[0].tx = texx1;
    quad.v[0].ty = texy1;
    quad.v[1].tx = texx2;
    quad.v[1].ty = texy1;
    quad.v[2].tx = texx2;
    quad.v[2].ty = texy2;
    quad.v[3].tx = texx1;
    quad.v[3].ty = texy2;

    quad.v[0].z =
            quad.v[1].z =
            quad.v[2].z =
            quad.v[3].z = 0.5f;

    quad.v[0].col =
            quad.v[1].col =
            quad.v[2].col =
            quad.v[3].col = 0xffffffff;

    quad.blend = BLEND_DEFAULT;
}

hgeSprite::hgeSprite(const hgeSprite& spr) {
    memcpy(this, &spr, sizeof(hgeSprite));
    hge = hgeCreate(HGE_VERSION);
}

void hgeSprite::Render(const float x, const float y) {

    const auto tempx1 = x - hotX;
    const auto tempy1 = y - hotY;
    const auto tempx2 = x + width - hotX;
    const auto tempy2 = y + height - hotY;

    quad.v[0].x = tempx1;
    quad.v[0].y = tempy1;
    quad.v[1].x = tempx2;
    quad.v[1].y = tempy1;
    quad.v[2].x = tempx2;
    quad.v[2].y = tempy2;
    quad.v[3].x = tempx1;
    quad.v[3].y = tempy2;

    hge->Gfx_RenderQuad(&quad);
}


void hgeSprite::RenderEx(const float x, const float y, const float rot,
                         const float hscale, float vscale) {

    if (vscale == 0) {
        vscale = hscale;
    }

    const auto tx1 = -hotX * hscale;
    const auto ty1 = -hotY * vscale;
    const auto tx2 = (width - hotX) * hscale;
    const auto ty2 = (height - hotY) * vscale;

    if (rot != 0.0f) {
        const auto cost = cosf(rot);
        const auto sint = sinf(rot);

        quad.v[0].x = tx1 * cost - ty1 * sint + x;
        quad.v[0].y = tx1 * sint + ty1 * cost + y;

        quad.v[1].x = tx2 * cost - ty1 * sint + x;
        quad.v[1].y = tx2 * sint + ty1 * cost + y;

        quad.v[2].x = tx2 * cost - ty2 * sint + x;
        quad.v[2].y = tx2 * sint + ty2 * cost + y;

        quad.v[3].x = tx1 * cost - ty2 * sint + x;
        quad.v[3].y = tx1 * sint + ty2 * cost + y;
    }
    else {
        quad.v[0].x = tx1 + x;
        quad.v[0].y = ty1 + y;
        quad.v[1].x = tx2 + x;
        quad.v[1].y = ty1 + y;
        quad.v[2].x = tx2 + x;
        quad.v[2].y = ty2 + y;
        quad.v[3].x = tx1 + x;
        quad.v[3].y = ty2 + y;
    }

    hge->Gfx_RenderQuad(&quad);
}


void hgeSprite::RenderStretch(const float x1, const float y1,
                              const float x2, const float y2) {
    quad.v[0].x = x1;
    quad.v[0].y = y1;
    quad.v[1].x = x2;
    quad.v[1].y = y1;
    quad.v[2].x = x2;
    quad.v[2].y = y2;
    quad.v[3].x = x1;
    quad.v[3].y = y2;

    hge->Gfx_RenderQuad(&quad);
}


void hgeSprite::Render4V(const float x0, const float y0,
                         const float x1, const float y1,
                         const float x2, const float y2,
                         const float x3, const float y3) {
    quad.v[0].x = x0;
    quad.v[0].y = y0;
    quad.v[1].x = x1;
    quad.v[1].y = y1;
    quad.v[2].x = x2;
    quad.v[2].y = y2;
    quad.v[3].x = x3;
    quad.v[3].y = y3;

    hge->Gfx_RenderQuad(&quad);
}


hgeRect* hgeSprite::GetBoundingBoxEx(const float x, const float y,
                                     const float rot, const float hscale,
                                     const float vscale, hgeRect* rect) const {
    rect->Clear();

    const auto tx1 = -hotX * hscale;
    const auto ty1 = -hotY * vscale;
    const auto tx2 = (width - hotX) * hscale;
    const auto ty2 = (height - hotY) * vscale;

    if (rot != 0.0f) {
        const auto cost = cosf(rot);
        const auto sint = sinf(rot);

        rect->Encapsulate(tx1 * cost - ty1 * sint + x, tx1 * sint + ty1 * cost + y);
        rect->Encapsulate(tx2 * cost - ty1 * sint + x, tx2 * sint + ty1 * cost + y);
        rect->Encapsulate(tx2 * cost - ty2 * sint + x, tx2 * sint + ty2 * cost + y);
        rect->Encapsulate(tx1 * cost - ty2 * sint + x, tx1 * sint + ty2 * cost + y);
    }
    else {
        rect->Encapsulate(tx1 + x, ty1 + y);
        rect->Encapsulate(tx2 + x, ty1 + y);
        rect->Encapsulate(tx2 + x, ty2 + y);
        rect->Encapsulate(tx1 + x, ty2 + y);
    }

    return rect;
}

void hgeSprite::SetFlip(const bool b_x, const bool b_y, const bool b_hot_spot) {
    float tx, ty;

    if (bHSFlip && bXFlip) {
        hotX = width - hotX;
    }
    if (bHSFlip && bYFlip) {
        hotY = height - hotY;
    }

    bHSFlip = b_hot_spot;

    if (bHSFlip && bXFlip) {
        hotX = width - hotX;
    }
    if (bHSFlip && bYFlip) {
        hotY = height - hotY;
    }

    if (b_x != bXFlip) {
        tx = quad.v[0].tx;
        quad.v[0].tx = quad.v[1].tx;
        quad.v[1].tx = tx;
        ty = quad.v[0].ty;
        quad.v[0].ty = quad.v[1].ty;
        quad.v[1].ty = ty;
        tx = quad.v[3].tx;
        quad.v[3].tx = quad.v[2].tx;
        quad.v[2].tx = tx;
        ty = quad.v[3].ty;
        quad.v[3].ty = quad.v[2].ty;
        quad.v[2].ty = ty;

        bXFlip = !bXFlip;
    }

    if (b_y != bYFlip) {
        tx = quad.v[0].tx;
        quad.v[0].tx = quad.v[3].tx;
        quad.v[3].tx = tx;
        ty = quad.v[0].ty;
        quad.v[0].ty = quad.v[3].ty;
        quad.v[3].ty = ty;
        tx = quad.v[1].tx;
        quad.v[1].tx = quad.v[2].tx;
        quad.v[2].tx = tx;
        ty = quad.v[1].ty;
        quad.v[1].ty = quad.v[2].ty;
        quad.v[2].ty = ty;

        bYFlip = !bYFlip;
    }
}


void hgeSprite::SetTexture(const HTEXTURE tex) {
    float tw, th;

    quad.tex = tex;

    if (tex) {
        tw = static_cast<float>(hge->Texture_GetWidth(tex));
        th = static_cast<float>(hge->Texture_GetHeight(tex));
    }
    else {
        tw = 1.0f;
        th = 1.0f;
    }

    if (tw != tex_width || th != tex_height) {
        float tx1 = quad.v[0].tx * tex_width;
        float ty1 = quad.v[0].ty * tex_height;
        float tx2 = quad.v[2].tx * tex_width;
        float ty2 = quad.v[2].ty * tex_height;

        tex_width = tw;
        tex_height = th;

        tx1 /= tw;
        ty1 /= th;
        tx2 /= tw;
        ty2 /= th;

        quad.v[0].tx = tx1;
        quad.v[0].ty = ty1;
        quad.v[1].tx = tx2;
        quad.v[1].ty = ty1;
        quad.v[2].tx = tx2;
        quad.v[2].ty = ty2;
        quad.v[3].tx = tx1;
        quad.v[3].ty = ty2;
    }
}


void hgeSprite::SetTextureRect(const float x, const float y,
                               const float w, const float h,
                               const bool adj_size) {

    tx = x;
    ty = y;

    if (adj_size) {
        width = w;
        height = h;
    }

    const auto tx1 = tx / tex_width;
    const auto ty1 = ty / tex_height;
    const auto tx2 = (tx + w) / tex_width;
    const auto ty2 = (ty + h) / tex_height;

    quad.v[0].tx = tx1;
    quad.v[0].ty = ty1;
    quad.v[1].tx = tx2;
    quad.v[1].ty = ty1;
    quad.v[2].tx = tx2;
    quad.v[2].ty = ty2;
    quad.v[3].tx = tx1;
    quad.v[3].ty = ty2;

    const auto b_x = bXFlip;
    const auto b_y = bYFlip;
    const auto b_hs = bHSFlip;
    bXFlip = false;
    bYFlip = false;
    SetFlip(b_x, b_y, b_hs);
}


void hgeSprite::SetColor(const hgeU32 col, const int i) {
    if (i != -1) {
        quad.v[i].col = col;
    }
    else {
        quad.v[0].col = quad.v[1].col = quad.v[2].col = quad.v[3].col = col;
    }
}

void hgeSprite::SetZ(const float z, const int i) {
    if (i != -1) {
        quad.v[i].z = z;
    }
    else {
        quad.v[0].z = quad.v[1].z = quad.v[2].z = quad.v[3].z = z;
    }
}
