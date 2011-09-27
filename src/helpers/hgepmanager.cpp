/*
 ** Haaf's Game Engine 1.7
 ** Copyright (C) 2003-2007, Relish Games
 ** hge.relishgames.com
 **
 ** hgeParticleManager helper class implementation
 */

#include <hgeparticle.h>



hgeParticleManager::hgeParticleManager()
{
	m_num_systems = 0;
	m_tx = m_ty = 0.0f;
}



hgeParticleManager::~hgeParticleManager()
{
	int i;
	for (i = 0; i < m_num_systems; i++)
		delete psList[i];
}



void hgeParticleManager::Update(float dt)
{
	int i;
	for (i = 0; i < m_num_systems; i++)
	{
		psList[i]->Update(dt);
		if (psList[i]->GetAge() == -2.0f && psList[i]->GetParticlesAlive() == 0)
		{
			delete psList[i];
			psList[i] = psList[m_num_systems - 1];
			m_num_systems--;
			i--;
		}
	}
}



void hgeParticleManager::Render()
{
	int i;
	for (i = 0; i < m_num_systems; i++)
		psList[i]->Render();
}



hgeParticleSystem* hgeParticleManager::SpawnPS(hgeParticleSystemInfo *psi,
		float x, float y)
{
	if (m_num_systems == HGE_MAX_PSYSTEMS)
		return 0;
	psList[m_num_systems] = new hgeParticleSystem(psi);
	psList[m_num_systems]->FireAt(x, y);
	psList[m_num_systems]->Transpose(m_tx, m_ty);
	m_num_systems++;
	return psList[m_num_systems - 1];
}



bool hgeParticleManager::IsPSAlive(hgeParticleSystem *ps) const
{
	int i;
	for (i = 0; i < m_num_systems; i++)
		if (psList[i] == ps)
			return true;
	return false;
}



void hgeParticleManager::Transpose(float x, float y)
{
	int i;
	for (i = 0; i < m_num_systems; i++)
		psList[i]->Transpose(x, y);
	m_tx = x;
	m_ty = y;
}



void hgeParticleManager::KillPS(hgeParticleSystem *ps)
{
	int i;
	for (i = 0; i < m_num_systems; i++)
	{
		if (psList[i] == ps)
		{
			delete psList[i];
			psList[i] = psList[m_num_systems - 1];
			m_num_systems--;
			return;
		}
	}
}



void hgeParticleManager::KillAll()
{
	int i;
	for (i = 0; i < m_num_systems; i++)
		delete psList[i];
	m_num_systems = 0;
}
