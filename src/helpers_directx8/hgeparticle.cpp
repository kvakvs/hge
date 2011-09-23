/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeParticleSystem helper class implementation
*/


#include "..\..\include\hgeparticle.h"


HGE	*hgeParticleSystem::hge=0;


hgeParticleSystem::hgeParticleSystem(const char *filename, hgeSprite *sprite)
{
	void *psi;

	hge=hgeCreate(HGE_VERSION);

	psi=hge->Resource_Load(filename);
	if(!psi) return;
	memcpy(&info, psi, sizeof(hgeParticleSystemInfo));
	hge->Resource_Free(psi);
	info.sprite=sprite;

	vecLocation.x=vecPrevLocation.x=0.0f;
	vecLocation.y=vecPrevLocation.y=0.0f;
	fTx=fTy=0;
	fScale = 1.0f;

	fEmissionResidue=0.0f;
	nParticlesAlive=0;
	fAge=-2.0;

	rectBoundingBox.Clear();
	bUpdateBoundingBox=false;
}

hgeParticleSystem::hgeParticleSystem(hgeParticleSystemInfo *psi)
{
	hge=hgeCreate(HGE_VERSION);

	memcpy(&info, psi, sizeof(hgeParticleSystemInfo));

	vecLocation.x=vecPrevLocation.x=0.0f;
	vecLocation.y=vecPrevLocation.y=0.0f;
	fTx=fTy=0;
	fScale = 1.0f;

	fEmissionResidue=0.0f;
	nParticlesAlive=0;
	fAge=-2.0;

	rectBoundingBox.Clear();
	bUpdateBoundingBox=false;
}

hgeParticleSystem::hgeParticleSystem(const hgeParticleSystem &ps)
{
	memcpy(this, &ps, sizeof(hgeParticleSystem));
	hge=hgeCreate(HGE_VERSION);
}

void hgeParticleSystem::Update(float fDeltaTime)
{
	int i;
	float ang;
	hgeParticle *par;
	hgeVector vecAccel, vecAccel2;

	if(fAge >= 0)
	{
		fAge += fDeltaTime;
		if(fAge >= info.fLifetime) fAge = -2.0f;
	}

	// update all alive particles

	if(bUpdateBoundingBox) rectBoundingBox.Clear();
	par=particles;

	for(i=0; i<nParticlesAlive; i++)
	{
		par->fAge += fDeltaTime;
		if(par->fAge >= par->fTerminalAge)
		{
			nParticlesAlive--;
			memcpy(par, &particles[nParticlesAlive], sizeof(hgeParticle));
			i--;
			continue;
		}

		vecAccel = par->vecLocation-vecLocation;
		vecAccel.Normalize();
		vecAccel2 = vecAccel;
		vecAccel *= par->fRadialAccel;

		// vecAccel2.Rotate(M_PI_2);
		// the following is faster
		ang = vecAccel2.x;
		vecAccel2.x = -vecAccel2.y;
		vecAccel2.y = ang;

		vecAccel2 *= par->fTangentialAccel;
		par->vecVelocity += (vecAccel+vecAccel2)*fDeltaTime;
		par->vecVelocity.y += par->fGravity*fDeltaTime;

		par->vecLocation += par->vecVelocity*fDeltaTime;

		par->fSpin += par->fSpinDelta*fDeltaTime;
		par->fSize += par->fSizeDelta*fDeltaTime;
		par->colColor += par->colColorDelta*fDeltaTime;

		if(bUpdateBoundingBox) rectBoundingBox.Encapsulate(par->vecLocation.x, par->vecLocation.y);

		par++;
	}

	// generate new particles

	if(fAge != -2.0f)
	{
		float fParticlesNeeded = info.nEmission*fDeltaTime + fEmissionResidue;
		int nParticlesCreated = (unsigned int)fParticlesNeeded;
		fEmissionResidue=fParticlesNeeded-nParticlesCreated;

		par=&particles[nParticlesAlive];

		for(i=0; i<nParticlesCreated; i++)
		{
			if(nParticlesAlive>=MAX_PARTICLES) break;

			par->fAge = 0.0f;
			par->fTerminalAge = hge->Random_Float(info.fParticleLifeMin, info.fParticleLifeMax);

			par->vecLocation = vecPrevLocation+(vecLocation-vecPrevLocation)*hge->Random_Float(0.0f, 1.0f);
			par->vecLocation.x += hge->Random_Float(-2.0f, 2.0f);
			par->vecLocation.y += hge->Random_Float(-2.0f, 2.0f);

			ang=info.fDirection-M_PI_2+hge->Random_Float(0,info.fSpread)-info.fSpread/2.0f;
			if(info.bRelative) ang += (vecPrevLocation-vecLocation).Angle()+M_PI_2;
			par->vecVelocity.x = cosf(ang);
			par->vecVelocity.y = sinf(ang);
			par->vecVelocity *= hge->Random_Float(info.fSpeedMin, info.fSpeedMax);

			par->fGravity = hge->Random_Float(info.fGravityMin, info.fGravityMax);
			par->fRadialAccel = hge->Random_Float(info.fRadialAccelMin, info.fRadialAccelMax);
			par->fTangentialAccel = hge->Random_Float(info.fTangentialAccelMin, info.fTangentialAccelMax);

			par->fSize = hge->Random_Float(info.fSizeStart, info.fSizeStart+(info.fSizeEnd-info.fSizeStart)*info.fSizeVar);
			par->fSizeDelta = (info.fSizeEnd-par->fSize) / par->fTerminalAge;

			par->fSpin = hge->Random_Float(info.fSpinStart, info.fSpinStart+(info.fSpinEnd-info.fSpinStart)*info.fSpinVar);
			par->fSpinDelta = (info.fSpinEnd-par->fSpin) / par->fTerminalAge;

			par->colColor.r = hge->Random_Float(info.colColorStart.r, info.colColorStart.r+(info.colColorEnd.r-info.colColorStart.r)*info.fColorVar);
			par->colColor.g = hge->Random_Float(info.colColorStart.g, info.colColorStart.g+(info.colColorEnd.g-info.colColorStart.g)*info.fColorVar);
			par->colColor.b = hge->Random_Float(info.colColorStart.b, info.colColorStart.b+(info.colColorEnd.b-info.colColorStart.b)*info.fColorVar);
			par->colColor.a = hge->Random_Float(info.colColorStart.a, info.colColorStart.a+(info.colColorEnd.a-info.colColorStart.a)*info.fAlphaVar);

			par->colColorDelta.r = (info.colColorEnd.r-par->colColor.r) / par->fTerminalAge;
			par->colColorDelta.g = (info.colColorEnd.g-par->colColor.g) / par->fTerminalAge;
			par->colColorDelta.b = (info.colColorEnd.b-par->colColor.b) / par->fTerminalAge;
			par->colColorDelta.a = (info.colColorEnd.a-par->colColor.a) / par->fTerminalAge;

			if(bUpdateBoundingBox) rectBoundingBox.Encapsulate(par->vecLocation.x, par->vecLocation.y);

			nParticlesAlive++;
			par++;
		}
	}

	vecPrevLocation=vecLocation;
}

void hgeParticleSystem::MoveTo(float x, float y, bool bMoveParticles)
{
	int i;
	float dx,dy;
	
	if(bMoveParticles)
	{
		dx=x-vecLocation.x;
		dy=y-vecLocation.y;

		for(i=0;i<nParticlesAlive;i++)
		{
			particles[i].vecLocation.x += dx;
			particles[i].vecLocation.y += dy;
		}

		vecPrevLocation.x=vecPrevLocation.x + dx;
		vecPrevLocation.y=vecPrevLocation.y + dy;
	}
	else
	{
		if(fAge==-2.0) { vecPrevLocation.x=x; vecPrevLocation.y=y; }
		else { vecPrevLocation.x=vecLocation.x;	vecPrevLocation.y=vecLocation.y; }
	}

	vecLocation.x=x;
	vecLocation.y=y;
}

void hgeParticleSystem::FireAt(float x, float y)
{
	Stop();
	MoveTo(x,y);
	Fire();
}

void hgeParticleSystem::Fire()
{
	if(info.fLifetime==-1.0f) fAge=-1.0f;
	else fAge=0.0f;
}

void hgeParticleSystem::Stop(bool bKillParticles)
{
	fAge=-2.0f;
	if(bKillParticles) 
	{
		nParticlesAlive=0;
		rectBoundingBox.Clear();
	}
}

void hgeParticleSystem::Render()
{
	int i;
	DWORD col;
	hgeParticle *par=particles;

	col=info.sprite->GetColor();

	for(i=0; i<nParticlesAlive; i++)
	{
		if(info.colColorStart.r < 0)
			info.sprite->SetColor(SETA(info.sprite->GetColor(),par->colColor.a*255));
		else
			info.sprite->SetColor(par->colColor.GetHWColor());
		info.sprite->RenderEx(par->vecLocation.x*fScale+fTx, par->vecLocation.y*fScale+fTy, par->fSpin*par->fAge, par->fSize*fScale);
		par++;
	}

	info.sprite->SetColor(col);
}


hgeRect *hgeParticleSystem::GetBoundingBox(hgeRect *rect) const
{
	*rect = rectBoundingBox;

	rect->x1 *= fScale;
	rect->y1 *= fScale;
	rect->x2 *= fScale;
	rect->y2 *= fScale;

	return rect;
}
