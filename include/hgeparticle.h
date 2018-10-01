/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeParticleSystem helper class header
*/


#pragma once


#include "hge.h"
#include "hgesprite.h"
#include "hgevector.h"
#include "hgecolor.h"
#include "hgerect.h"


#define MAX_PARTICLES	500
#define MAX_PSYSTEMS	100

struct hgeParticle {
    hgeVector vecLocation;
    hgeVector vecVelocity;

    float fGravity;
    float fRadialAccel;
    float fTangentialAccel;

    float fSpin;
    float fSpinDelta;

    float fSize;
    float fSizeDelta;

    hgeColor colColor; // + alpha
    hgeColor colColorDelta;

    float fAge;
    float fTerminalAge;
};

struct hgeParticleSystemInfo {
    hgeSprite* sprite; // texture + blend mode
    int nEmission; // particles per sec
    float fLifetime;

    float fParticleLifeMin;
    float fParticleLifeMax;

    float fDirection;
    float fSpread;
    bool bRelative;

    float fSpeedMin;
    float fSpeedMax;

    float fGravityMin;
    float fGravityMax;

    float fRadialAccelMin;
    float fRadialAccelMax;

    float fTangentialAccelMin;
    float fTangentialAccelMax;

    float fSizeStart;
    float fSizeEnd;
    float fSizeVar;

    float fSpinStart;
    float fSpinEnd;
    float fSpinVar;

    hgeColor colColorStart; // + alpha
    hgeColor colColorEnd;
    float fColorVar;
    float fAlphaVar;
};

class hgeParticleSystem {
public:
    hgeParticleSystemInfo info;

    hgeParticleSystem(const char* filename, hgeSprite* sprite);
    hgeParticleSystem(hgeParticleSystemInfo* psi);
    hgeParticleSystem(const hgeParticleSystem& ps);

    ~hgeParticleSystem() {
        hge_->Release();
    }

    hgeParticleSystem& operator=(const hgeParticleSystem& ps);


    void Render();
    void FireAt(float x, float y);
    void Fire();
    void Stop(bool bKillParticles = false);
    void Update(float fDeltaTime);
    void MoveTo(float x, float y, bool bMoveParticles = false);

    void Transpose(const float x, const float y) {
        tx_ = x;
        ty_ = y;
    }

    void SetScale(const float scale) {
        scale_ = scale;
    }

    void TrackBoundingBox(const bool bTrack) {
        update_bounding_box_ = bTrack;
    }

    int GetParticlesAlive() const {
        return particles_alive_;
    }

    float GetAge() const {
        return age_;
    }

    void GetPosition(float* x, float* y) const {
        *x = location_.x;
        *y = location_.y;
    }

    void GetTransposition(float* x, float* y) const {
        *x = tx_;
        *y = ty_;
    }

    float GetScale() const {
        return scale_;
    }

    hgeRect* GetBoundingBox(hgeRect* rect) const;

private:
    hgeParticleSystem();

    static HGE* hge_;

    float age_;
    float emission_residue_;

    hgeVector prev_location_;
    hgeVector location_;
    float tx_;
    float ty_;
    float scale_;

    int particles_alive_;
    hgeRect bounding_box_;
    bool update_bounding_box_;

    hgeParticle particles_[MAX_PARTICLES];
};

class hgeParticleManager {
public:
    hgeParticleManager();
    ~hgeParticleManager();

    void Update(float dt);
    void Render();

    hgeParticleSystem* SpawnPS(hgeParticleSystemInfo* psi, float x, float y);
    bool IsPSAlive(hgeParticleSystem* ps) const;
    void Transpose(float x, float y);

    void GetTransposition(float* dx, float* dy) const {
        *dx = x_;
        *dy = y_;
    }

    void KillPS(hgeParticleSystem* ps);
    void KillAll();

private:
    hgeParticleManager(const hgeParticleManager&);
    hgeParticleManager& operator=(const hgeParticleManager&);

    int ps_;
    float x_;
    float y_;
    hgeParticleSystem* list_[MAX_PSYSTEMS];
};
