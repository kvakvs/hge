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
	return m_time;
}



float HGE_CALL HGE_Impl::Timer_GetDelta()
{
	return m_time_delta;
}



int HGE_CALL HGE_Impl::Timer_GetFPS()
{
	return m_time_fps;
}

