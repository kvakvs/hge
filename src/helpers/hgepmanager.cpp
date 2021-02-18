/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// hgeParticleManager helper class implementation
//

#include <hgeparticle.h>


// NOLINTNEXTLINE
hgeParticleManager::hgeParticleManager() {
  ps_ = 0;
  x_ = y_ = 0.0f;
}

hgeParticleManager::~hgeParticleManager() {
  for (int i = 0; i < ps_; i++) {
    delete list_[i];
  }
}

void hgeParticleManager::Update(const float dt) {
  for (int i = 0; i < ps_; i++) {
    list_[i]->Update(dt);
    if (list_[i]->GetAge() == -2.0f && list_[i]->GetParticlesAlive() == 0) {
      delete list_[i];
      list_[i] = list_[ps_ - 1];
      ps_--;
      i--;
    }
  }
}

void hgeParticleManager::Render() {
  for (int i = 0; i < ps_; i++) {
    list_[i]->Render();
  }
}

hgeParticleSystem *hgeParticleManager::SpawnPS(hgeParticleSystemInfo *psi,
                                               const float x, const float y) {
  if (ps_ == MAX_PSYSTEMS) {
    return nullptr;
  }
  list_[ps_] = new hgeParticleSystem(psi);
  list_[ps_]->FireAt(x, y);
  list_[ps_]->Transpose(x_, y_);
  ps_++;
  return list_[ps_ - 1];
}

bool hgeParticleManager::IsPSAlive(hgeParticleSystem *ps) const {
  for (int i = 0; i < ps_; i++)
    if (list_[i] == ps) {
      return true;
    }
  return false;
}

void hgeParticleManager::Transpose(const float x, const float y) {
  for (auto i = 0; i < ps_; i++) {
    list_[i]->Transpose(x, y);
  }
  x_ = x;
  y_ = y;
}

void hgeParticleManager::KillPS(hgeParticleSystem *ps) {
  for (int i = 0; i < ps_; i++) {
    if (list_[i] == ps) {
      delete list_[i];
      list_[i] = list_[ps_ - 1];
      ps_--;
      return;
    }
  }
}

void hgeParticleManager::KillAll() {
  for (int i = 0; i < ps_; i++) {
    delete list_[i];
  }
  ps_ = 0;
}
