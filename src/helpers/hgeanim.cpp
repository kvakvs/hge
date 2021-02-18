/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// hgeAnimation helper class implementation
//

#include <hgeanim.h>


// NOLINTNEXTLINE
hgeAnimation::hgeAnimation(const HTEXTURE tex, const int nframes,
                           const float fps, const float x, const float y,
                           const float w, const float h)
        : hgeSprite(tex, x, y, w, h) {
  orig_width_ = hge_->Texture_GetWidth(tex, true);

  since_last_frame_ = -1.0f;
  speed_ = 1.0f / fps;
  is_playing_ = false;
  frames_ = nframes;

  mode_ = HGEANIM_FWD | HGEANIM_LOOP;
  delta_ = 1;
  SetFrame(0);
}

hgeAnimation::hgeAnimation(const hgeAnimation &anim)
        : hgeSprite(anim) {
  // Copy hgeAnimation parameters:
  this->orig_width_ = anim.orig_width_;
  this->is_playing_ = anim.is_playing_;
  this->speed_ = anim.speed_;
  this->since_last_frame_ = anim.since_last_frame_;
  this->mode_ = anim.mode_;
  this->delta_ = anim.delta_;
  this->frames_ = anim.frames_;
  this->cur_frame_ = anim.cur_frame_;
}

void hgeAnimation::SetMode(const int mode) {
  mode_ = mode;

  if (mode & HGEANIM_REV) {
    delta_ = -1;
    SetFrame(frames_ - 1);
  } else {
    delta_ = 1;
    SetFrame(0);
  }
}


void hgeAnimation::Play() {
  is_playing_ = true;
  since_last_frame_ = -1.0f;
  if (mode_ & HGEANIM_REV) {
    delta_ = -1;
    SetFrame(frames_ - 1);
  } else {
    delta_ = 1;
    SetFrame(0);
  }
}


void hgeAnimation::Update(const float delta_time) {
  if (!is_playing_) {
    return;
  }

  if (since_last_frame_ == -1.0f) {
    since_last_frame_ = 0.0f;
  } else {
    since_last_frame_ += delta_time;
  }

  while (since_last_frame_ >= speed_) {
    since_last_frame_ -= speed_;

    if (cur_frame_ + delta_ == frames_) {
      switch (mode_) {
        case HGEANIM_FWD:
        case HGEANIM_REV | HGEANIM_PINGPONG:
          is_playing_ = false;
          break;

        case HGEANIM_FWD | HGEANIM_PINGPONG:
        case HGEANIM_FWD | HGEANIM_PINGPONG | HGEANIM_LOOP:
        case HGEANIM_REV | HGEANIM_PINGPONG | HGEANIM_LOOP:
          delta_ = -delta_;
          break;
      }
    } else if (cur_frame_ + delta_ < 0) {
      switch (mode_) {
        case HGEANIM_REV:
        case HGEANIM_FWD | HGEANIM_PINGPONG:
          is_playing_ = false;
          break;

        case HGEANIM_REV | HGEANIM_PINGPONG:
        case HGEANIM_REV | HGEANIM_PINGPONG | HGEANIM_LOOP:
        case HGEANIM_FWD | HGEANIM_PINGPONG | HGEANIM_LOOP:
          delta_ = -delta_;
          break;
      }
    }

    if (is_playing_) {
      SetFrame(cur_frame_ + delta_);
    }
  }
}

void hgeAnimation::SetFrame(int n) {
  const auto ncols = int(orig_width_) / int(width_);

  n = n % frames_;
  if (n < 0) {
    n = frames_ + n;
  }
  cur_frame_ = n;

  // calculate texture coords for frame n
  auto ty1 = ty_;
  auto tx1 = tx_ + n * width_;

  if (tx1 > orig_width_ - width_) {
    n -= int(orig_width_ - tx_) / int(width_);
    tx1 = width_ * (n % ncols);
    ty1 += height_ * (1 + n / ncols);
  }

  auto tx2 = tx1 + width_;
  auto ty2 = ty1 + height_;

  tx1 /= tex_width_;
  ty1 /= tex_height_;
  tx2 /= tex_width_;
  ty2 /= tex_height_;

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
