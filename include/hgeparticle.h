/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeParticleSystem helper class header
*/
#pragma once
#ifndef HGEPARTICLE_H
#define HGEPARTICLE_H


#include <hge.h>
#include <hgesprite.h>
#include <hgevector.h>
#include <hgecolor.h>
#include <hgerect.h>


#define HGE_MAX_PARTICLES	500
#define HGE_MAX_PSYSTEMS	100

struct hgeParticle
{
	hgeVector	vecLocation;
	hgeVector	vecVelocity;

	float		fGravity;
	float		fRadialAccel;
	float		fTangentialAccel;

	float		fSpin;
	float		fSpinDelta;

	float		fSize;
	float		fSizeDelta;

	hgeColor	colColor;		// + alpha
	hgeColor	colColorDelta;

	float		fAge;
	float		fTerminalAge;
};

struct hgeParticleSystemInfo
{
	hgeSprite*	sprite;    // texture + blend mode
	int			nEmission; // particles per sec
	float		fLifetime;

	float		fParticleLifeMin;
	float		fParticleLifeMax;

	float		fDirection;
	float		fSpread;
	bool		bRelative;

	float		fSpeedMin;
	float		fSpeedMax;

	float		fGravityMin;
	float		fGravityMax;

	float		fRadialAccelMin;
	float		fRadialAccelMax;

	float		fTangentialAccelMin;
	float		fTangentialAccelMax;

	float		fSizeStart;
	float		fSizeEnd;
	float		fSizeVar;

	float		fSpinStart;
	float		fSpinEnd;
	float		fSpinVar;

	hgeColor	colColorStart; // + alpha
	hgeColor	colColorEnd;
	float		fColorVar;
	float		fAlphaVar;
};

class hgeParticleSystem
{
public:
	hgeParticleSystemInfo info;
	
	hgeParticleSystem(hgeConstString filename, hgeSprite *sprite);
	hgeParticleSystem(hgeParticleSystemInfo *psi);
	hgeParticleSystem(const hgeParticleSystem &ps);
	~hgeParticleSystem() { m_hge->Release(); }

	hgeParticleSystem&	operator= (const hgeParticleSystem &ps);


	void		Render();
	void		FireAt(float x, float y);
	void		Fire();
	void		Stop(bool bKillParticles=false);
	void		Update(float fDeltaTime);
	void		MoveTo(float x, float y, bool bMoveParticles=false);
	void		Transpose(float x, float y) { m_tx=x; m_ty=y; }
	void		SetScale(float scale) { m_scale = scale; }
	void		TrackBoundingBox(bool bTrack) { m_update_bbox_flag=bTrack; }

	int			GetParticlesAlive() const { return m_num_particles_alive; }
	float		GetAge() const { return m_age; }
	void		GetPosition(float *x, float *y) const { *x=m_location.x; *y=m_location.y; }
	void		GetTransposition(float *x, float *y) const { *x=m_tx; *y=m_ty; }
	float		GetScale() { return m_scale; }
	hgeRect *	GetBoundingBox(hgeRect * rect) const;

private:
	hgeParticleSystem();

	static HGE			* m_hge;

	float				m_age;
	float				m_emission_residue;

	hgeVector			m_prev_location;
	hgeVector			m_location;
	float				m_tx, m_ty;
	float				m_scale;

	int					m_num_particles_alive;
	hgeRect				m_bbox;
	bool				m_update_bbox_flag;

	hgeParticle			m_particles[HGE_MAX_PARTICLES];
};

class hgeParticleManager
{
public:
	hgeParticleManager();
	~hgeParticleManager();

	void				Update(float dt);
	void				Render();

	hgeParticleSystem*	SpawnPS(hgeParticleSystemInfo *psi, float x, float y);
	bool				IsPSAlive(hgeParticleSystem *ps) const;
	void				Transpose(float x, float y);
	void				GetTransposition(float *dx, float *dy) const {*dx=m_tx; *dy=m_ty;}
	void				KillPS(hgeParticleSystem *ps);
	void				KillAll();

private:
	hgeParticleManager(const hgeParticleManager &);
	hgeParticleManager&	operator= (const hgeParticleManager &);

	int					m_num_systems;
	float				m_tx;
	float				m_ty;
	hgeParticleSystem *	psList[HGE_MAX_PSYSTEMS];
};


#endif
