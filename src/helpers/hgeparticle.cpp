/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// hgeParticleSystem helper class implementation
//

#include <hgeparticle.h>


HGE *hgeParticleSystem::hge_ = nullptr;


hgeParticleSystem::hgeParticleSystem(const char *filename, hgeSprite *sprite) {

  hge_ = hgeCreate(HGE_VERSION);

  void *psi = hge_->Resource_Load(filename);
  if (!psi) {
    return;
  }
  memcpy(&info, psi, sizeof(hgeParticleSystemInfo));
  hge_->Resource_Free(psi);
  info.sprite = sprite;

  location_.x = prev_location_.x = 0.0f;
  location_.y = prev_location_.y = 0.0f;
  tx_ = ty_ = 0;
  scale_ = 1.0f;

  emission_residue_ = 0.0f;
  particles_alive_ = 0;
  age_ = -2.0;

  bounding_box_.Clear();
  update_bounding_box_ = false;
}

hgeParticleSystem::hgeParticleSystem(hgeParticleSystemInfo *psi) {
  hge_ = hgeCreate(HGE_VERSION);

  memcpy(&info, psi, sizeof(hgeParticleSystemInfo));

  location_.x = prev_location_.x = 0.0f;
  location_.y = prev_location_.y = 0.0f;
  tx_ = ty_ = 0;
  scale_ = 1.0f;

  emission_residue_ = 0.0f;
  particles_alive_ = 0;
  age_ = -2.0;

  bounding_box_.Clear();
  update_bounding_box_ = false;
}

hgeParticleSystem::hgeParticleSystem(const hgeParticleSystem &ps) {
  memcpy(this, &ps, sizeof(hgeParticleSystem));
  hge_ = hgeCreate(HGE_VERSION);
}

void hgeParticleSystem::Update(const float f_delta_time) {
  int i;
  float ang;

  if (age_ >= 0) {
    age_ += f_delta_time;
    if (age_ >= info.fLifetime) {
      age_ = -2.0f;
    }
  }

  // update all alive particles

  if (update_bounding_box_) {
    bounding_box_.Clear();
  }
  hgeParticle *par = particles_;

  for (i = 0; i < particles_alive_; i++) {
    par->fAge += f_delta_time;
    if (par->fAge >= par->fTerminalAge) {
      particles_alive_--;
      memcpy(par, &particles_[particles_alive_], sizeof(hgeParticle));
      i--;
      continue;
    }

    hgeVector vecAccel = par->vecLocation - location_;
    vecAccel.Normalize();
    hgeVector vecAccel2 = vecAccel;
    vecAccel *= par->fRadialAccel;

    // vecAccel2.Rotate(M_PI_2);
    // the following is faster
    ang = vecAccel2.x;
    vecAccel2.x = -vecAccel2.y;
    vecAccel2.y = ang;

    vecAccel2 *= par->fTangentialAccel;
    par->vecVelocity += (vecAccel + vecAccel2) * f_delta_time;
    par->vecVelocity.y += par->fGravity * f_delta_time;

    par->vecLocation += par->vecVelocity * f_delta_time;

    par->fSpin += par->fSpinDelta * f_delta_time;
    par->fSize += par->fSizeDelta * f_delta_time;
    par->colColor += par->colColorDelta * f_delta_time;

    if (update_bounding_box_) {
      bounding_box_.Encapsulate(par->vecLocation.x, par->vecLocation.y);
    }

    par++;
  }

  // generate new particles

  if (age_ != -2.0f) {
    const auto f_particles_needed = info.nEmission * f_delta_time + emission_residue_;
    const int nParticlesCreated = static_cast<unsigned int>(f_particles_needed);
    emission_residue_ = f_particles_needed - nParticlesCreated;

    par = &particles_[particles_alive_];

    for (i = 0; i < nParticlesCreated; i++) {
      if (particles_alive_ >= MAX_PARTICLES) {
        break;
      }

      par->fAge = 0.0f;
      par->fTerminalAge = hge_->Random_Float(info.fParticleLifeMin, info.fParticleLifeMax);

      par->vecLocation = prev_location_ + (location_ - prev_location_) * hge_->
              Random_Float(0.0f, 1.0f);
      par->vecLocation.x += hge_->Random_Float(-2.0f, 2.0f);
      par->vecLocation.y += hge_->Random_Float(-2.0f, 2.0f);

      ang = info.fDirection - M_PI_2 + hge_->Random_Float(0, info.fSpread) - info.fSpread /
                                                                             2.0f;
      if (info.bRelative) {
        ang += (prev_location_ - location_).Angle() + M_PI_2;
      }
      par->vecVelocity.x = cosf(ang);
      par->vecVelocity.y = sinf(ang);
      par->vecVelocity *= hge_->Random_Float(info.fSpeedMin, info.fSpeedMax);

      par->fGravity = hge_->Random_Float(info.fGravityMin, info.fGravityMax);
      par->fRadialAccel = hge_->Random_Float(info.fRadialAccelMin, info.fRadialAccelMax);
      par->fTangentialAccel = hge_->Random_Float(info.fTangentialAccelMin,
                                                 info.fTangentialAccelMax);

      par->fSize = hge_->Random_Float(info.fSizeStart,
                                      info.fSizeStart + (info.fSizeEnd - info.fSizeStart) *
                                                        info.fSizeVar);
      par->fSizeDelta = (info.fSizeEnd - par->fSize) / par->fTerminalAge;

      par->fSpin = hge_->Random_Float(info.fSpinStart,
                                      info.fSpinStart + (info.fSpinEnd - info.fSpinStart) *
                                                        info.fSpinVar);
      par->fSpinDelta = (info.fSpinEnd - par->fSpin) / par->fTerminalAge;

      par->colColor.r = hge_->Random_Float(info.colColorStart.r,
                                           info.colColorStart.r + (info.colColorEnd.r - info.
                                                   colColorStart.r) * info.fColorVar);
      par->colColor.g = hge_->Random_Float(info.colColorStart.g,
                                           info.colColorStart.g + (info.colColorEnd.g - info.
                                                   colColorStart.g) * info.fColorVar);
      par->colColor.b = hge_->Random_Float(info.colColorStart.b,
                                           info.colColorStart.b + (info.colColorEnd.b - info.
                                                   colColorStart.b) * info.fColorVar);
      par->colColor.a = hge_->Random_Float(info.colColorStart.a,
                                           info.colColorStart.a + (info.colColorEnd.a - info.
                                                   colColorStart.a) * info.fAlphaVar);

      par->colColorDelta.r = (info.colColorEnd.r - par->colColor.r) / par->fTerminalAge;
      par->colColorDelta.g = (info.colColorEnd.g - par->colColor.g) / par->fTerminalAge;
      par->colColorDelta.b = (info.colColorEnd.b - par->colColor.b) / par->fTerminalAge;
      par->colColorDelta.a = (info.colColorEnd.a - par->colColor.a) / par->fTerminalAge;

      if (update_bounding_box_) {
        bounding_box_.Encapsulate(par->vecLocation.x, par->vecLocation.y);
      }

      particles_alive_++;
      par++;
    }
  }

  prev_location_ = location_;
}

void hgeParticleSystem::MoveTo(const float x, const float y,
                               const bool b_move_particles) {

  if (b_move_particles) {
    const auto dx = x - location_.x;
    const auto dy = y - location_.y;

    for (int i = 0; i < particles_alive_; i++) {
      particles_[i].vecLocation.x += dx;
      particles_[i].vecLocation.y += dy;
    }

    prev_location_.x = prev_location_.x + dx;
    prev_location_.y = prev_location_.y + dy;
  } else {
    if (age_ == -2.0) {
      prev_location_.x = x;
      prev_location_.y = y;
    } else {
      prev_location_.x = location_.x;
      prev_location_.y = location_.y;
    }
  }

  location_.x = x;
  location_.y = y;
}

void hgeParticleSystem::FireAt(const float x, const float y) {
  Stop();
  MoveTo(x, y);
  Fire();
}

void hgeParticleSystem::Fire() {
  if (info.fLifetime == -1.0f) {
    age_ = -1.0f;
  } else {
    age_ = 0.0f;
  }
}

void hgeParticleSystem::Stop(const bool b_kill_particles) {
  age_ = -2.0f;
  if (b_kill_particles) {
    particles_alive_ = 0;
    bounding_box_.Clear();
  }
}

void hgeParticleSystem::Render() {
  auto par = particles_;

  const auto col = info.sprite->GetColor();

  for (auto i = 0; i < particles_alive_; i++) {
    if (info.colColorStart.r < 0) {
      info.sprite->SetColor(info.sprite->GetColor().set_a(par->colColor.a * 255));
    } else {
      info.sprite->SetColor(par->colColor.GetHWColor());
    }
    info.sprite->RenderEx(par->vecLocation.x * scale_ + tx_, par->vecLocation.y * scale_ + ty_,
                          par->fSpin * par->fAge, par->fSize * scale_);
    par++;
  }

  info.sprite->SetColor(col);
}


hgeRect *hgeParticleSystem::GetBoundingBox(hgeRect *rect) const {
  *rect = bounding_box_;

  rect->x1 *= scale_;
  rect->y1 *= scale_;
  rect->x2 *= scale_;
  rect->y2 *= scale_;

  return rect;
}
