/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeParticleManager helper class implementation
*/


#include "../../include/hgeparticle.h"


hgeParticleManager::hgeParticleManager() {
    nPS = 0;
    tX = tY = 0.0f;
}

hgeParticleManager::~hgeParticleManager() {
    for (int i = 0; i < nPS; i++) {
        delete psList[i];
    }
}

void hgeParticleManager::Update(const float dt) {
    for (int i = 0; i < nPS; i++) {
        psList[i]->Update(dt);
        if (psList[i]->GetAge() == -2.0f && psList[i]->GetParticlesAlive() == 0) {
            delete psList[i];
            psList[i] = psList[nPS - 1];
            nPS--;
            i--;
        }
    }
}

void hgeParticleManager::Render() {
    for (int i = 0; i < nPS; i++) {
        psList[i]->Render();
    }
}

hgeParticleSystem* hgeParticleManager::SpawnPS(hgeParticleSystemInfo* psi, 
                                               const float x, const float y) {
    if (nPS == MAX_PSYSTEMS) {
        return nullptr;
    }
    psList[nPS] = new hgeParticleSystem(psi);
    psList[nPS]->FireAt(x, y);
    psList[nPS]->Transpose(tX, tY);
    nPS++;
    return psList[nPS - 1];
}

bool hgeParticleManager::IsPSAlive(hgeParticleSystem* ps) const {
    for (int i = 0; i < nPS; i++)
        if (psList[i] == ps) {
            return true;
        }
    return false;
}

void hgeParticleManager::Transpose(const float x, const float y) {
    for (auto i = 0; i < nPS; i++) {
        psList[i]->Transpose(x, y);
    }
    tX = x;
    tY = y;
}

void hgeParticleManager::KillPS(hgeParticleSystem* ps) {
    for (int i = 0; i < nPS; i++) {
        if (psList[i] == ps) {
            delete psList[i];
            psList[i] = psList[nPS - 1];
            nPS--;
            return;
        }
    }
}

void hgeParticleManager::KillAll() {
    for (int i = 0; i < nPS; i++) {
        delete psList[i];
    }
    nPS = 0;
}
