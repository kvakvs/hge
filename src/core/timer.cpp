/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: timer
*/


#include "hge_impl.h"


float HGE_CALL HGE_Impl::Timer_GetTime()
{
	return fTime;
}

float HGE_CALL HGE_Impl::Timer_GetDelta()
{
	return fDeltaTime;
}


int HGE_CALL HGE_Impl::Timer_GetFPS()
{
	return nFPS;
}

