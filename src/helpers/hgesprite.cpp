/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// hgeSprite helper class implementation
//


#include <hgesprite.h>
#include <cmath>


HGE *hgeSprite::hge_ = nullptr;


hgeSprite::hgeSprite(const HTEXTURE texture, const float texx, const float texy,
                     const float w, const float h) {

  hge_ = hgeCreate(HGE_VERSION);

  tx_ = texx;
  ty_ = texy;
  width_ = w;
  height_ = h;

  if (texture) {
    tex_width_ = static_cast<float>(hge_->Texture_GetWidth(texture));
    tex_height_ = static_cast<float>(hge_->Texture_GetHeight(texture));
  } else {
    tex_width_ = 1.0f;
    tex_height_ = 1.0f;
  }

  hot_x_ = 0;
  hot_y_ = 0;
  x_flip_ = false;
  y_flip_ = false;
  hs_flip_ = false;
  quad_.tex = texture;

  const auto texx1 = texx / tex_width_;
  const auto texy1 = texy / tex_height_;
  const auto texx2 = (texx + w) / tex_width_;
  const auto texy2 = (texy + h) / tex_height_;

  quad_.v[0].tx = texx1;
  quad_.v[0].ty = texy1;
  quad_.v[1].tx = texx2;
  quad_.v[1].ty = texy1;
  quad_.v[2].tx = texx2;
  quad_.v[2].ty = texy2;
  quad_.v[3].tx = texx1;
  quad_.v[3].ty = texy2;

  quad_.v[0].z =
  quad_.v[1].z =
  quad_.v[2].z =
  quad_.v[3].z = 0.5f;

  quad_.v[0].col =
  quad_.v[1].col =
  quad_.v[2].col =
  quad_.v[3].col = 0xffffffff;

  quad_.blend = BLEND_DEFAULT;
}

hgeSprite::hgeSprite(const hgeSprite &spr)
        : quad_(spr.quad_),
          tx_(spr.tx_),
          ty_(spr.ty_),
          width_(spr.width_),
          height_(spr.height_),
          tex_width_(spr.tex_width_),
          tex_height_(spr.tex_height_),
          hot_x_(spr.hot_x_),
          hot_y_(spr.hot_y_),
          x_flip_(spr.x_flip_),
          y_flip_(spr.y_flip_),
          hs_flip_(spr.hs_flip_) {
  hge_ = hgeCreate(HGE_VERSION);
}

void hgeSprite::Render(const float x, const float y) {
  const auto tempx1 = x - hot_x_;
  const auto tempy1 = y - hot_y_;
  const auto tempx2 = x + width_ - hot_x_;
  const auto tempy2 = y + height_ - hot_y_;

  quad_.v[0].x = tempx1;
  quad_.v[0].y = tempy1;
  quad_.v[1].x = tempx2;
  quad_.v[1].y = tempy1;
  quad_.v[2].x = tempx2;
  quad_.v[2].y = tempy2;
  quad_.v[3].x = tempx1;
  quad_.v[3].y = tempy2;

  hge_->Gfx_RenderQuad(&quad_);
}


void hgeSprite::RenderEx(const float x, const float y, const float rot,
                         const float hscale, float vscale) {

  if (vscale == 0) {
    vscale = hscale;
  }

  const auto tx1 = -hot_x_ * hscale;
  const auto ty1 = -hot_y_ * vscale;
  const auto tx2 = (width_ - hot_x_) * hscale;
  const auto ty2 = (height_ - hot_y_) * vscale;

  if (rot != 0.0f) {
    const auto cost = cosf(rot);
    const auto sint = sinf(rot);

    quad_.v[0].x = tx1 * cost - ty1 * sint + x;
    quad_.v[0].y = tx1 * sint + ty1 * cost + y;

    quad_.v[1].x = tx2 * cost - ty1 * sint + x;
    quad_.v[1].y = tx2 * sint + ty1 * cost + y;

    quad_.v[2].x = tx2 * cost - ty2 * sint + x;
    quad_.v[2].y = tx2 * sint + ty2 * cost + y;

    quad_.v[3].x = tx1 * cost - ty2 * sint + x;
    quad_.v[3].y = tx1 * sint + ty2 * cost + y;
  } else {
    quad_.v[0].x = tx1 + x;
    quad_.v[0].y = ty1 + y;
    quad_.v[1].x = tx2 + x;
    quad_.v[1].y = ty1 + y;
    quad_.v[2].x = tx2 + x;
    quad_.v[2].y = ty2 + y;
    quad_.v[3].x = tx1 + x;
    quad_.v[3].y = ty2 + y;
  }

  hge_->Gfx_RenderQuad(&quad_);
}


void hgeSprite::RenderStretch(const float x1, const float y1,
                              const float x2, const float y2) {
  quad_.v[0].x = x1;
  quad_.v[0].y = y1;
  quad_.v[1].x = x2;
  quad_.v[1].y = y1;
  quad_.v[2].x = x2;
  quad_.v[2].y = y2;
  quad_.v[3].x = x1;
  quad_.v[3].y = y2;

  hge_->Gfx_RenderQuad(&quad_);
}


void hgeSprite::Render4V(const float x0, const float y0,
                         const float x1, const float y1,
                         const float x2, const float y2,
                         const float x3, const float y3) {
  quad_.v[0].x = x0;
  quad_.v[0].y = y0;
  quad_.v[1].x = x1;
  quad_.v[1].y = y1;
  quad_.v[2].x = x2;
  quad_.v[2].y = y2;
  quad_.v[3].x = x3;
  quad_.v[3].y = y3;

  hge_->Gfx_RenderQuad(&quad_);
}


hgeRect *hgeSprite::GetBoundingBoxEx(const float x, const float y,
                                     const float rot,
                                     const float hscale, const float vscale,
                                     hgeOUT hgeRect *rect) const {
  rect->Clear();

  const auto tx1 = -hot_x_ * hscale;
  const auto ty1 = -hot_y_ * vscale;
  const auto tx2 = (width_ - hot_x_) * hscale;
  const auto ty2 = (height_ - hot_y_) * vscale;

  if (rot != 0.0f) {
    const auto cost = std::cosf(rot);
    const auto sint = std::sinf(rot);

    rect->Encapsulate(tx1 * cost - ty1 * sint + x, tx1 * sint + ty1 * cost + y);
    rect->Encapsulate(tx2 * cost - ty1 * sint + x, tx2 * sint + ty1 * cost + y);
    rect->Encapsulate(tx2 * cost - ty2 * sint + x, tx2 * sint + ty2 * cost + y);
    rect->Encapsulate(tx1 * cost - ty2 * sint + x, tx1 * sint + ty2 * cost + y);
  } else {
    rect->Encapsulate(tx1 + x, ty1 + y);
    rect->Encapsulate(tx2 + x, ty1 + y);
    rect->Encapsulate(tx2 + x, ty2 + y);
    rect->Encapsulate(tx1 + x, ty2 + y);
  }

  return rect;
}

void hgeSprite::SetFlip(const bool b_x, const bool b_y, const bool b_hot_spot) {
  float tx, ty;

  if (hs_flip_ && x_flip_) {
    hot_x_ = width_ - hot_x_;
  }
  if (hs_flip_ && y_flip_) {
    hot_y_ = height_ - hot_y_;
  }

  hs_flip_ = b_hot_spot;

  if (hs_flip_ && x_flip_) {
    hot_x_ = width_ - hot_x_;
  }
  if (hs_flip_ && y_flip_) {
    hot_y_ = height_ - hot_y_;
  }

  if (b_x != x_flip_) {
    tx = quad_.v[0].tx;
    quad_.v[0].tx = quad_.v[1].tx;
    quad_.v[1].tx = tx;
    ty = quad_.v[0].ty;
    quad_.v[0].ty = quad_.v[1].ty;
    quad_.v[1].ty = ty;
    tx = quad_.v[3].tx;
    quad_.v[3].tx = quad_.v[2].tx;
    quad_.v[2].tx = tx;
    ty = quad_.v[3].ty;
    quad_.v[3].ty = quad_.v[2].ty;
    quad_.v[2].ty = ty;

    x_flip_ = !x_flip_;
  }

  if (b_y != y_flip_) {
    tx = quad_.v[0].tx;
    quad_.v[0].tx = quad_.v[3].tx;
    quad_.v[3].tx = tx;
    ty = quad_.v[0].ty;
    quad_.v[0].ty = quad_.v[3].ty;
    quad_.v[3].ty = ty;
    tx = quad_.v[1].tx;
    quad_.v[1].tx = quad_.v[2].tx;
    quad_.v[2].tx = tx;
    ty = quad_.v[1].ty;
    quad_.v[1].ty = quad_.v[2].ty;
    quad_.v[2].ty = ty;

    y_flip_ = !y_flip_;
  }
}


void hgeSprite::SetTexture(const HTEXTURE tex) {
  float tw, th;

  quad_.tex = tex;

  if (tex) {
    tw = static_cast<float>(hge_->Texture_GetWidth(tex));
    th = static_cast<float>(hge_->Texture_GetHeight(tex));
  } else {
    tw = 1.0f;
    th = 1.0f;
  }

  if (tw != tex_width_ || th != tex_height_) {
    float tx1 = quad_.v[0].tx * tex_width_;
    float ty1 = quad_.v[0].ty * tex_height_;
    float tx2 = quad_.v[2].tx * tex_width_;
    float ty2 = quad_.v[2].ty * tex_height_;

    tex_width_ = tw;
    tex_height_ = th;

    tx1 /= tw;
    ty1 /= th;
    tx2 /= tw;
    ty2 /= th;

    quad_.v[0].tx = tx1;
    quad_.v[0].ty = ty1;
    quad_.v[1].tx = tx2;
    quad_.v[1].ty = ty1;
    quad_.v[2].tx = tx2;
    quad_.v[2].ty = ty2;
    quad_.v[3].tx = tx1;
    quad_.v[3].ty = ty2;
  }
}


void hgeSprite::SetTextureRect(const float x, const float y,
                               const float w, const float h,
                               const bool adj_size) {

  tx_ = x;
  ty_ = y;

  if (adj_size) {
    width_ = w;
    height_ = h;
  }

  const auto tx1 = tx_ / tex_width_;
  const auto ty1 = ty_ / tex_height_;
  const auto tx2 = (tx_ + w) / tex_width_;
  const auto ty2 = (ty_ + h) / tex_height_;

  quad_.v[0].tx = tx1;
  quad_.v[0].ty = ty1;
  quad_.v[1].tx = tx2;
  quad_.v[1].ty = ty1;
  quad_.v[2].tx = tx2;
  quad_.v[2].ty = ty2;
  quad_.v[3].tx = tx1;
  quad_.v[3].ty = ty2;

  const auto b_x = x_flip_;
  const auto b_y = y_flip_;
  const auto b_hs = hs_flip_;
  x_flip_ = false;
  y_flip_ = false;
  SetFlip(b_x, b_y, b_hs);
}


void hgeSprite::SetColor(const hgeColor32 col, const int i) {
  if (i != -1) {
    quad_.v[i].col = col.argb;
  } else {
    quad_.v[0].col = quad_.v[1].col = quad_.v[2].col = quad_.v[3].col = col.argb;
  }
}

void hgeSprite::SetZ(const float z, const int i) {
  if (i != -1) {
    quad_.v[i].z = z;
  } else {
    quad_.v[0].z = quad_.v[1].z = quad_.v[2].z = quad_.v[3].z = z;
  }
}
