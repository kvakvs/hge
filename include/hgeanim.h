/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeAnimation helper class header
*/


#pragma once


#include "hgesprite.h"


#define HGEANIM_FWD			0
#define HGEANIM_REV			1
#define HGEANIM_PINGPONG	2
#define HGEANIM_NOPINGPONG	0
#define HGEANIM_LOOP		4
#define HGEANIM_NOLOOP		0


/*
** HGE Animation class
*/
class hgeAnimation : public hgeSprite {
public:
    hgeAnimation(HTEXTURE tex, int nframes, float FPS, float x, float y, float w, float h);
    hgeAnimation(const hgeAnimation& anim);

    void Play();

    void Stop() {
        bPlaying = false;
    }

    void Resume() {
        bPlaying = true;
    }

    void Update(float fDeltaTime);

    bool IsPlaying() const {
        return bPlaying;
    }

    void SetTexture(const HTEXTURE tex) {
        hgeSprite::SetTexture(tex);
        orig_width = hge->Texture_GetWidth(tex, true);
    }

    void SetTextureRect(const float x1, const float y1, 
                        const float x2, const float y2) {
        hgeSprite::SetTextureRect(x1, y1, x2, y2);
        SetFrame(nCurFrame);
    }

    void SetMode(int mode);

    void SetSpeed(const float fps) {
        fSpeed = 1.0f / fps;
    }

    void SetFrame(int n);

    void SetFrames(const int n) {
        nFrames = n;
    }

    int GetMode() const {
        return Mode;
    }

    float GetSpeed() const {
        return 1.0f / fSpeed;
    }

    int GetFrame() const {
        return nCurFrame;
    }

    int GetFrames() const {
        return nFrames;
    }

private:
    hgeAnimation();

    int orig_width;

    bool bPlaying;

    float fSpeed;
    float fSinceLastFrame;

    int Mode;
    int nDelta;
    int nFrames;
    int nCurFrame;
};
