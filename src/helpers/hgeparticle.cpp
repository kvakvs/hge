/*
 ** Haaf's Game Engine 1.7
 ** Copyright (C) 2003-2007, Relish Games
 ** hge.relishgames.com
 **
 ** hgeParticleSystem helper class implementation
 */
#include <hgeparticle.h>

HGE *hgeParticleSystem::m_hge = 0;



hgeParticleSystem::hgeParticleSystem(hgeConstString filename, hgeSprite *sprite)
{
	void *psi;

	m_hge = hgeCreate(HGE_VERSION);

	psi = m_hge->Resource_Load(filename);
	if (!psi)
		return;
	memcpy(&info, psi, sizeof(hgeParticleSystemInfo));
	m_hge->Resource_Free(psi);
	info.sprite = sprite;

	m_location.x = m_prev_location.x = 0.0f;
	m_location.y = m_prev_location.y = 0.0f;
	m_tx = m_ty = 0;
	m_scale = 1.0f;

	m_emission_residue = 0.0f;
	m_num_particles_alive = 0;
	m_age = -2.0;

	m_bbox.Clear();
	m_update_bbox_flag = false;
}



hgeParticleSystem::hgeParticleSystem(hgeParticleSystemInfo *psi)
{
	m_hge = hgeCreate(HGE_VERSION);

	memcpy(&info, psi, sizeof(hgeParticleSystemInfo));

	m_location.x = m_prev_location.x = 0.0f;
	m_location.y = m_prev_location.y = 0.0f;
	m_tx = m_ty = 0;
	m_scale = 1.0f;

	m_emission_residue = 0.0f;
	m_num_particles_alive = 0;
	m_age = -2.0;

	m_bbox.Clear();
	m_update_bbox_flag = false;
}



hgeParticleSystem::hgeParticleSystem(const hgeParticleSystem &ps)
{
	memcpy(this, &ps, sizeof(hgeParticleSystem));
	m_hge = hgeCreate(HGE_VERSION);
}



void hgeParticleSystem::Update(float fDeltaTime)
{
	int i;
	float ang;
	hgeParticle *par;
	hgeVector vecAccel, vecAccel2;

	if (m_age >= 0)
	{
		m_age += fDeltaTime;
		if (m_age >= info.fLifetime)
			m_age = -2.0f;
	}

	// update all alive particles

	if (m_update_bbox_flag)
		m_bbox.Clear();
	par = m_particles;

	for (i = 0; i < m_num_particles_alive; i++)
	{
		par->fAge += fDeltaTime;
		if (par->fAge >= par->fTerminalAge)
		{
			m_num_particles_alive--;
			memcpy(par, &m_particles[m_num_particles_alive], sizeof(hgeParticle));
			i--;
			continue;
		}

		vecAccel = par->vecLocation - m_location;
		vecAccel.Normalize();
		vecAccel2 = vecAccel;
		vecAccel *= par->fRadialAccel;

		// vecAccel2.Rotate(M_PI_2);
		// the following is faster
		ang = vecAccel2.x;
		vecAccel2.x = -vecAccel2.y;
		vecAccel2.y = ang;

		vecAccel2 *= par->fTangentialAccel;
		par->vecVelocity += (vecAccel + vecAccel2) * fDeltaTime;
		par->vecVelocity.y += par->fGravity * fDeltaTime;

		par->vecLocation += par->vecVelocity * fDeltaTime;

		par->fSpin += par->fSpinDelta * fDeltaTime;
		par->fSize += par->fSizeDelta * fDeltaTime;
		par->colColor += par->colColorDelta * fDeltaTime;

		if (m_update_bbox_flag)
			m_bbox.Encapsulate(par->vecLocation.x, par->vecLocation.y);

		par++;
	}

	// generate new particles

	if (m_age != -2.0f)
	{
		float fParticlesNeeded = info.nEmission * fDeltaTime + m_emission_residue;
		int nParticlesCreated = (unsigned int) fParticlesNeeded;
		m_emission_residue = fParticlesNeeded - nParticlesCreated;

		par = &m_particles[m_num_particles_alive];

		for (i = 0; i < nParticlesCreated; i++)
		{
			if (m_num_particles_alive >= HGE_MAX_PARTICLES)
				break;

			par->fAge = 0.0f;
			par->fTerminalAge = m_hge->Random_Float(info.fParticleLifeMin,
					info.fParticleLifeMax);

			par->vecLocation = m_prev_location
					+ (m_location - m_prev_location) * m_hge->Random_Float(0.0f,
							1.0f);
			par->vecLocation.x += m_hge->Random_Float(-2.0f, 2.0f);
			par->vecLocation.y += m_hge->Random_Float(-2.0f, 2.0f);

			ang = info.fDirection - M_PI_2 + m_hge->Random_Float(0, info.fSpread)
					- info.fSpread / 2.0f;
			if (info.bRelative)
				ang += (m_prev_location - m_location).Angle() + M_PI_2;
			par->vecVelocity.x = cosf(ang);
			par->vecVelocity.y = sinf(ang);
			par->vecVelocity *= m_hge->Random_Float(info.fSpeedMin,
					info.fSpeedMax);

			par->fGravity = m_hge->Random_Float(info.fGravityMin,
					info.fGravityMax);
			par->fRadialAccel = m_hge->Random_Float(info.fRadialAccelMin,
					info.fRadialAccelMax);
			par->fTangentialAccel = m_hge->Random_Float(info.fTangentialAccelMin,
					info.fTangentialAccelMax);

			par->fSize = m_hge->Random_Float(info.fSizeStart, info.fSizeStart
					+ (info.fSizeEnd - info.fSizeStart) * info.fSizeVar);
			par->fSizeDelta = (info.fSizeEnd - par->fSize) / par->fTerminalAge;

			par->fSpin = m_hge->Random_Float(info.fSpinStart, info.fSpinStart
					+ (info.fSpinEnd - info.fSpinStart) * info.fSpinVar);
			par->fSpinDelta = (info.fSpinEnd - par->fSpin) / par->fTerminalAge;

			par->colColor.r = m_hge->Random_Float(info.colColorStart.r,
					info.colColorStart.r + (info.colColorEnd.r
							- info.colColorStart.r) * info.fColorVar);
			par->colColor.g = m_hge->Random_Float(info.colColorStart.g,
					info.colColorStart.g + (info.colColorEnd.g
							- info.colColorStart.g) * info.fColorVar);
			par->colColor.b = m_hge->Random_Float(info.colColorStart.b,
					info.colColorStart.b + (info.colColorEnd.b
							- info.colColorStart.b) * info.fColorVar);
			par->colColor.a = m_hge->Random_Float(info.colColorStart.a,
					info.colColorStart.a + (info.colColorEnd.a
							- info.colColorStart.a) * info.fAlphaVar);

			par->colColorDelta.r = (info.colColorEnd.r - par->colColor.r)
					/ par->fTerminalAge;
			par->colColorDelta.g = (info.colColorEnd.g - par->colColor.g)
					/ par->fTerminalAge;
			par->colColorDelta.b = (info.colColorEnd.b - par->colColor.b)
					/ par->fTerminalAge;
			par->colColorDelta.a = (info.colColorEnd.a - par->colColor.a)
					/ par->fTerminalAge;

			if (m_update_bbox_flag)
				m_bbox.Encapsulate(par->vecLocation.x,
						par->vecLocation.y);

			m_num_particles_alive++;
			par++;
		}
	}

	m_prev_location = m_location;
}



void hgeParticleSystem::MoveTo(float x, float y, bool bMoveParticles)
{
	int i;
	float dx, dy;

	if (bMoveParticles)
	{
		dx = x - m_location.x;
		dy = y - m_location.y;

		for (i = 0; i < m_num_particles_alive; i++)
		{
			m_particles[i].vecLocation.x += dx;
			m_particles[i].vecLocation.y += dy;
		}

		m_prev_location.x = m_prev_location.x + dx;
		m_prev_location.y = m_prev_location.y + dy;
	}
	else
	{
		if (m_age == -2.0)
		{
			m_prev_location.x = x;
			m_prev_location.y = y;
		}
		else
		{
			m_prev_location.x = m_location.x;
			m_prev_location.y = m_location.y;
		}
	}

	m_location.x = x;
	m_location.y = y;
}



void hgeParticleSystem::FireAt(float x, float y)
{
	Stop();
	MoveTo(x, y);
	Fire();
}



void hgeParticleSystem::Fire()
{
	if (info.fLifetime == -1.0f)
		m_age = -1.0f;
	else
		m_age = 0.0f;
}



void hgeParticleSystem::Stop(bool bKillParticles)
{
	m_age = -2.0f;
	if (bKillParticles)
	{
		m_num_particles_alive = 0;
		m_bbox.Clear();
	}
}



void hgeParticleSystem::Render()
{
	int i;
	uint32_t col;
	hgeParticle *par = m_particles;

	col = info.sprite->GetColor();

	for (i = 0; i < m_num_particles_alive; i++)
	{
		if (info.colColorStart.r < 0)
			info.sprite->SetColor(
					SETA(info.sprite->GetColor(),par->colColor.a*255));
		else
			info.sprite->SetColor(par->colColor.GetHWColor());
		info.sprite->RenderEx(par->vecLocation.x * m_scale + m_tx,
				par->vecLocation.y * m_scale + m_ty, par->fSpin * par->fAge,
				par->fSize * m_scale);
		par++;
	}

	info.sprite->SetColor(col);
}



hgeRect *hgeParticleSystem::GetBoundingBox(hgeRect *rect) const
{
	*rect = m_bbox;

	rect->x1 *= m_scale;
	rect->y1 *= m_scale;
	rect->x2 *= m_scale;
	rect->y2 *= m_scale;

	return rect;
}
