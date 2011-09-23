/*
** Haaf's Game Engine 1.81
** Copyright (C) 2008, Relish Games
** hge.relishgames.com
**
** Particle systems editor
*/


#include "particleed.h"
#include <windows.h>
#include <string>

using namespace std;

bool HandleKeys(int key)
{
	switch(key)
	{
		case 0:
			return false;

		case HGEK_ESCAPE:
			cmdSavePreset(state.nPreset);
			return true;

		case HGEK_TAB:
			state.bIFace = !state.bIFace;
			break;

		case HGEK_LBUTTON:
			if(state.mx>=168 && state.mx<=631) state.ps->Fire();
			break;

		case HGEK_SPACE:
			state.ps->Fire();
			break;

		case HGEK_1:
		case HGEK_2:
		case HGEK_3:
		case HGEK_4:
		case HGEK_5:
		case HGEK_6:
		case HGEK_7:
		case HGEK_8:
		case HGEK_9: cmdChangePreset(key-HGEK_1); break;
	}

	return false;
}

bool DoCommands(int id)
{
	switch(id)
	{
		// Presets & stuff
	
		case 0:	return false;
		case CMD_EXIT: cmdSavePreset(state.nPreset); return true;
		case CMD_HELP: state.bHelp=ButtonGetState(CMD_HELP); break;
		case CMD_BOUNDINGBOX: state.bBBox=ButtonGetState(CMD_BOUNDINGBOX); break;

		case CMD_PRESET1:
		case CMD_PRESET2:
		case CMD_PRESET3:
		case CMD_PRESET4:
		case CMD_PRESET5:
		case CMD_PRESET6:
		case CMD_PRESET7:
		case CMD_PRESET8:
		case CMD_PRESET9: cmdChangePreset(id-CMD_PRESET1); break;

		// System parameters

		case CMD_SYS_LIFETIME:
			state.ps->info.fLifetime=SliderGetValue(CMD_SYS_LIFETIME);
			GetTextCtrl(CMD_SYS_TLIFETIME)->printf("%.1f",state.ps->info.fLifetime);
			ButtonSetState(CMD_SYS_LIFECONT, false);
			state.ps->Stop();
			break;
		
		case CMD_SYS_LIFECONT:
			if(ButtonGetState(CMD_SYS_LIFECONT))
			{
				state.ps->info.fLifetime=-1.0f;
				state.ps->Fire();
			}
			else
			{
				state.ps->info.fLifetime=SliderGetValue(CMD_SYS_LIFETIME);
				state.ps->Stop();
			}
			break;

		case CMD_SYS_EMISSION:
			state.ps->info.nEmission=int(SliderGetValue(CMD_SYS_EMISSION));
			GetTextCtrl(CMD_SYS_TEMISSION)->printf("%d",state.ps->info.nEmission);
			break;

		case CMD_SYS_PARLIFETIMELOCK:
			if(ButtonGetState(CMD_SYS_PARLIFETIMELOCK))
			{
				SliderSetValue(CMD_SYS_PARLIFETIMEMAX, SliderGetValue(CMD_SYS_PARLIFETIMEMIN));
				state.ps->info.fParticleLifeMax=state.ps->info.fParticleLifeMin;
			}
			break;

		case CMD_SYS_PARLIFETIMEMIN:
			state.ps->info.fParticleLifeMin=SliderGetValue(CMD_SYS_PARLIFETIMEMIN);
			if(ButtonGetState(CMD_SYS_PARLIFETIMELOCK))
			{
				SliderSetValue(CMD_SYS_PARLIFETIMEMAX, state.ps->info.fParticleLifeMin);
				state.ps->info.fParticleLifeMax=state.ps->info.fParticleLifeMin;
			}
			break;
		
		case CMD_SYS_PARLIFETIMEMAX:
			state.ps->info.fParticleLifeMax=SliderGetValue(CMD_SYS_PARLIFETIMEMAX);
			if(ButtonGetState(CMD_SYS_PARLIFETIMELOCK))
			{
				SliderSetValue(CMD_SYS_PARLIFETIMEMIN, state.ps->info.fParticleLifeMax);
				state.ps->info.fParticleLifeMin=state.ps->info.fParticleLifeMax;
			}
			break;

		case CMD_SYS_TEXTURE: sprParticles->SetFrame(sprParticles->GetFrame()+1); break;

		case CMD_SYS_BLENDADDITIVE:
			ButtonSetState(CMD_SYS_BLENDADDITIVE, true);
			ButtonSetState(CMD_SYS_BLENDBLEND, false);
			state.ps->info.sprite->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_ZWRITE);
			break;

		case CMD_SYS_BLENDBLEND:
			ButtonSetState(CMD_SYS_BLENDADDITIVE, false);
			ButtonSetState(CMD_SYS_BLENDBLEND, true);
			state.ps->info.sprite->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_ZWRITE);
			break;

		// Particle movement

		case CMD_PM_DIRECTION:
			state.ps->info.fDirection=SliderGetValue(CMD_PM_DIRECTION);
			GetTextCtrl(CMD_PM_TDIRECTION)->printf("%d",int(state.ps->info.fDirection*180/M_PI));
			break;
		
		case CMD_PM_RELATIVE:
			state.ps->info.bRelative=ButtonGetState(CMD_PM_RELATIVE);
			break;

		case CMD_PM_SPREAD:
			state.ps->info.fSpread=SliderGetValue(CMD_PM_SPREAD);
			GetTextCtrl(CMD_PM_TSPREAD)->printf("%d",int(state.ps->info.fSpread*180/M_PI));
			break;

		case CMD_PM_STARTSPEEDLOCK:
			if(ButtonGetState(CMD_PM_STARTSPEEDLOCK))
			{
				SliderSetValue(CMD_PM_STARTSPEEDMAX, SliderGetValue(CMD_PM_STARTSPEEDMIN));
				state.ps->info.fSpeedMax=state.ps->info.fSpeedMin;
			}
			break;

		case CMD_PM_STARTSPEEDMIN:
			state.ps->info.fSpeedMin=SliderGetValue(CMD_PM_STARTSPEEDMIN);
			if(ButtonGetState(CMD_PM_STARTSPEEDLOCK))
			{
				SliderSetValue(CMD_PM_STARTSPEEDMAX, state.ps->info.fSpeedMin);
				state.ps->info.fSpeedMax=state.ps->info.fSpeedMin;
			}
			break;
		
		case CMD_PM_STARTSPEEDMAX:
			state.ps->info.fSpeedMax=SliderGetValue(CMD_PM_STARTSPEEDMAX);
			if(ButtonGetState(CMD_PM_STARTSPEEDLOCK))
			{
				SliderSetValue(CMD_PM_STARTSPEEDMIN, state.ps->info.fSpeedMax);
				state.ps->info.fSpeedMin=state.ps->info.fSpeedMax;
			}
			break;

		case CMD_PM_GRAVITYLOCK:
			if(ButtonGetState(CMD_PM_GRAVITYLOCK))
			{
				SliderSetValue(CMD_PM_GRAVITYMAX, SliderGetValue(CMD_PM_GRAVITYMIN));
				state.ps->info.fGravityMax=state.ps->info.fGravityMin;
			}
			break;

		case CMD_PM_GRAVITYMIN:
			state.ps->info.fGravityMin=SliderGetValue(CMD_PM_GRAVITYMIN);
			if(ButtonGetState(CMD_PM_GRAVITYLOCK))
			{
				SliderSetValue(CMD_PM_GRAVITYMAX, state.ps->info.fGravityMin);
				state.ps->info.fGravityMax=state.ps->info.fGravityMin;
			}
			break;
		
		case CMD_PM_GRAVITYMAX:
			state.ps->info.fGravityMax=SliderGetValue(CMD_PM_GRAVITYMAX);
			if(ButtonGetState(CMD_PM_GRAVITYLOCK))
			{
				SliderSetValue(CMD_PM_GRAVITYMIN, state.ps->info.fGravityMax);
				state.ps->info.fGravityMin=state.ps->info.fGravityMax;
			}
			break;

		case CMD_PM_RADIALLOCK:
			if(ButtonGetState(CMD_PM_RADIALLOCK))
			{
				SliderSetValue(CMD_PM_RADIALMAX, SliderGetValue(CMD_PM_RADIALMIN));
				state.ps->info.fRadialAccelMax=state.ps->info.fRadialAccelMin;
			}
			break;

		case CMD_PM_RADIALMIN:
			state.ps->info.fRadialAccelMin=SliderGetValue(CMD_PM_RADIALMIN);
			if(ButtonGetState(CMD_PM_RADIALLOCK))
			{
				SliderSetValue(CMD_PM_RADIALMAX, state.ps->info.fRadialAccelMin);
				state.ps->info.fRadialAccelMax=state.ps->info.fRadialAccelMin;
			}
			break;
		
		case CMD_PM_RADIALMAX:
			state.ps->info.fRadialAccelMax=SliderGetValue(CMD_PM_RADIALMAX);
			if(ButtonGetState(CMD_PM_RADIALLOCK))
			{
				SliderSetValue(CMD_PM_RADIALMIN, state.ps->info.fRadialAccelMax);
				state.ps->info.fRadialAccelMin=state.ps->info.fRadialAccelMax;
			}
			break;

		case CMD_PM_TANGENTIALLOCK:
			if(ButtonGetState(CMD_PM_TANGENTIALLOCK))
			{
				SliderSetValue(CMD_PM_TANGENTIALMAX, SliderGetValue(CMD_PM_TANGENTIALMIN));
				state.ps->info.fTangentialAccelMax=state.ps->info.fTangentialAccelMin;
			}
			break;

		case CMD_PM_TANGENTIALMIN:
			state.ps->info.fTangentialAccelMin=SliderGetValue(CMD_PM_TANGENTIALMIN);
			if(ButtonGetState(CMD_PM_TANGENTIALLOCK))
			{
				SliderSetValue(CMD_PM_TANGENTIALMAX, state.ps->info.fTangentialAccelMin);
				state.ps->info.fTangentialAccelMax=state.ps->info.fTangentialAccelMin;
			}
			break;
		
		case CMD_PM_TANGENTIALMAX:
			state.ps->info.fTangentialAccelMax=SliderGetValue(CMD_PM_TANGENTIALMAX);
			if(ButtonGetState(CMD_PM_TANGENTIALLOCK))
			{
				SliderSetValue(CMD_PM_TANGENTIALMIN, state.ps->info.fTangentialAccelMax);
				state.ps->info.fTangentialAccelMin=state.ps->info.fTangentialAccelMax;
			}
			break;

		// Particle appearance
		
		case CMD_PA_SIZELOCK:
			if(ButtonGetState(CMD_PA_SIZELOCK))
			{
				SliderSetValue(CMD_PA_SIZEEND, SliderGetValue(CMD_PA_SIZESTART));
				state.ps->info.fSizeEnd=state.ps->info.fSizeStart;
			}
			break;

		case CMD_PA_SIZESTART:
			state.ps->info.fSizeStart=SliderGetValue(CMD_PA_SIZESTART)/PARTICLE_SIZE;
			if(ButtonGetState(CMD_PA_SIZELOCK))
			{
				SliderSetValue(CMD_PA_SIZEEND, state.ps->info.fSizeStart*PARTICLE_SIZE);
				state.ps->info.fSizeEnd=state.ps->info.fSizeStart;
			}
			break;
		
		case CMD_PA_SIZEEND:
			state.ps->info.fSizeEnd=SliderGetValue(CMD_PA_SIZEEND)/PARTICLE_SIZE;
			if(ButtonGetState(CMD_PA_SIZELOCK))
			{
				SliderSetValue(CMD_PA_SIZESTART, state.ps->info.fSizeEnd*PARTICLE_SIZE);
				state.ps->info.fSizeStart=state.ps->info.fSizeEnd;
			}
			break;

		case CMD_PA_SIZEVAR:
			state.ps->info.fSizeVar=SliderGetValue(CMD_PA_SIZEVAR);
			break;

		case CMD_PA_SPINLOCK:
			if(ButtonGetState(CMD_PA_SPINLOCK))
			{
				SliderSetValue(CMD_PA_SPINEND, SliderGetValue(CMD_PA_SPINSTART));
				state.ps->info.fSpinEnd=state.ps->info.fSpinStart;
			}
			break;

		case CMD_PA_SPINSTART:
			state.ps->info.fSpinStart=SliderGetValue(CMD_PA_SPINSTART);
			if(ButtonGetState(CMD_PA_SPINLOCK))
			{
				SliderSetValue(CMD_PA_SPINEND, state.ps->info.fSpinStart);
				state.ps->info.fSpinEnd=state.ps->info.fSpinStart;
			}
			break;
		
		case CMD_PA_SPINEND:
			state.ps->info.fSpinEnd=SliderGetValue(CMD_PA_SPINEND);
			if(ButtonGetState(CMD_PA_SPINLOCK))
			{
				SliderSetValue(CMD_PA_SPINSTART, state.ps->info.fSpinEnd);
				state.ps->info.fSpinStart=state.ps->info.fSpinEnd;
			}
			break;

		case CMD_PA_SPINVAR:
			state.ps->info.fSpinVar=SliderGetValue(CMD_PA_SPINVAR);
			break;

		case CMD_PA_ALPHALOCK:
			if(ButtonGetState(CMD_PA_ALPHALOCK))
			{
				SliderSetValue(CMD_PA_ALPHAEND, SliderGetValue(CMD_PA_ALPHASTART));
				state.ps->info.colColorEnd.a=state.ps->info.colColorStart.a;
			}
			break;

		case CMD_PA_ALPHASTART:
			state.ps->info.colColorStart.a=SliderGetValue(CMD_PA_ALPHASTART);
			if(ButtonGetState(CMD_PA_ALPHALOCK))
			{
				SliderSetValue(CMD_PA_ALPHAEND, state.ps->info.colColorStart.a);
				state.ps->info.colColorEnd.a=state.ps->info.colColorStart.a;
			}
			break;
		
		case CMD_PA_ALPHAEND:
			state.ps->info.colColorEnd.a=SliderGetValue(CMD_PA_ALPHAEND);
			if(ButtonGetState(CMD_PA_ALPHALOCK))
			{
				SliderSetValue(CMD_PA_ALPHASTART, state.ps->info.colColorEnd.a);
				state.ps->info.colColorStart.a=state.ps->info.colColorEnd.a;
			}
			break;

		case CMD_PA_ALPHAVAR:
			state.ps->info.fAlphaVar=SliderGetValue(CMD_PA_ALPHAVAR);
			break;

		case CMD_PA_RSTART:
		case CMD_PA_GSTART:
		case CMD_PA_BSTART:
		case CMD_PA_REND:
		case CMD_PA_GEND:
		case CMD_PA_BEND: cmdChangeColor(id); break;

		case CMD_PA_RGBVAR:
			state.ps->info.fColorVar=SliderGetValue(CMD_PA_RGBVAR);
			break;

	}

	return false;
}

void cmdChangePreset(int n)
{
	state.ps->Stop();
	ButtonSetState(CMD_PRESET1+state.nPreset, false);
	ButtonSetState(CMD_PRESET1+n, true);
	cmdSavePreset(state.nPreset);
	cmdLoadPreset(n);
	state.nPreset=n;
}

void cmdSavePreset(int n)
{
	HANDLE hF;
	DWORD size;
	char filename[_MAX_PATH];

	if(!state.ps) return;

	ZeroMemory(filename, _MAX_PATH);
	GetModuleFileName(GetModuleHandle(NULL), filename, _MAX_PATH);
	string s(filename);
	s=s.substr(0,s.rfind('\\'))+"\\"+"particle"+char('1'+n)+".psi";
	strcpy(filename, s.c_str());

	state.ps->info.sprite=(hgeSprite*)(sprParticles->GetFrame() | sprParticles->GetBlendMode()<<16);
	hF = CreateFile( filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if(hF == INVALID_HANDLE_VALUE) return;
	WriteFile(hF, &state.ps->info, sizeof(hgeParticleSystemInfo), &size, NULL );
 	CloseHandle(hF);
	state.ps->info.sprite=sprParticles;
}

void cmdLoadPreset(int n)
{
	HANDLE hF;
	DWORD size, col1, col2;
	char filename[_MAX_PATH];

	if(!state.ps) return;

	ZeroMemory(filename, _MAX_PATH);
	GetModuleFileName(GetModuleHandle(NULL), filename, _MAX_PATH);
	string s(filename);
	s=s.substr(0,s.rfind('\\'))+"\\"+"particle"+char('1'+n)+".psi";
	strcpy(filename, s.c_str());

	hF = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if(hF == INVALID_HANDLE_VALUE) return;
	ReadFile(hF, &state.ps->info, sizeof(hgeParticleSystemInfo), &size, NULL );
 	CloseHandle(hF);
	sprParticles->SetFrame((DWORD)state.ps->info.sprite & 0xFFFF);
	sprParticles->SetBlendMode((DWORD)state.ps->info.sprite >> 16);
	state.ps->info.sprite=sprParticles;

	// System parameters
	
	if(state.ps->info.fLifetime==-1.0f)
	{
		SliderSetValue(CMD_SYS_LIFETIME, 5.0f);
		ButtonSetState(CMD_SYS_LIFECONT, true);
		GetTextCtrl(CMD_SYS_TLIFETIME)->printf("%.1f", 5.0f);
		state.ps->Fire();
	}
	else
	{
		SliderSetValue(CMD_SYS_LIFETIME, state.ps->info.fLifetime);
		ButtonSetState(CMD_SYS_LIFECONT, false);
		GetTextCtrl(CMD_SYS_TLIFETIME)->printf("%.1f", state.ps->info.fLifetime);
	}

	SliderSetValue(CMD_SYS_EMISSION, (float)state.ps->info.nEmission);
	GetTextCtrl(CMD_SYS_TEMISSION)->printf("%d", state.ps->info.nEmission);

	SliderSetValue(CMD_SYS_PARLIFETIMEMIN, state.ps->info.fParticleLifeMin);
	SliderSetValue(CMD_SYS_PARLIFETIMEMAX, state.ps->info.fParticleLifeMax);
	ButtonSetState(CMD_SYS_PARLIFETIMELOCK, false);

	if(sprParticles->GetBlendMode() & BLEND_ALPHABLEND)
	{
		ButtonSetState(CMD_SYS_BLENDADDITIVE, false);
		ButtonSetState(CMD_SYS_BLENDBLEND, true);
	}
	else
	{
		ButtonSetState(CMD_SYS_BLENDADDITIVE, true);
		ButtonSetState(CMD_SYS_BLENDBLEND, false);
	}

	// Particle movement

	SliderSetValue(CMD_PM_DIRECTION, state.ps->info.fDirection);
	GetTextCtrl(CMD_PM_TDIRECTION)->printf("%d",int(state.ps->info.fDirection*180/M_PI));
	ButtonSetState(CMD_PM_RELATIVE, state.ps->info.bRelative);

	SliderSetValue(CMD_PM_SPREAD, state.ps->info.fSpread);
	GetTextCtrl(CMD_PM_TSPREAD)->printf("%d",int(state.ps->info.fSpread*180/M_PI));

	SliderSetValue(CMD_PM_STARTSPEEDMIN, state.ps->info.fSpeedMin);
	SliderSetValue(CMD_PM_STARTSPEEDMAX, state.ps->info.fSpeedMax);
	ButtonSetState(CMD_PM_STARTSPEEDLOCK, false);

	SliderSetValue(CMD_PM_GRAVITYMIN, state.ps->info.fGravityMin);
	SliderSetValue(CMD_PM_GRAVITYMAX, state.ps->info.fGravityMax);
	ButtonSetState(CMD_PM_GRAVITYLOCK, false);

	SliderSetValue(CMD_PM_RADIALMIN, state.ps->info.fRadialAccelMin);
	SliderSetValue(CMD_PM_RADIALMAX, state.ps->info.fRadialAccelMax);
	ButtonSetState(CMD_PM_RADIALLOCK, false);

	SliderSetValue(CMD_PM_TANGENTIALMIN, state.ps->info.fTangentialAccelMin);
	SliderSetValue(CMD_PM_TANGENTIALMAX, state.ps->info.fTangentialAccelMax);
	ButtonSetState(CMD_PM_TANGENTIALLOCK, false);

	// Particle appearance

	SliderSetValue(CMD_PA_SIZESTART, state.ps->info.fSizeStart*PARTICLE_SIZE);
	SliderSetValue(CMD_PA_SIZEEND, state.ps->info.fSizeEnd*PARTICLE_SIZE);
	SliderSetValue(CMD_PA_SIZEVAR, state.ps->info.fSizeVar);
	ButtonSetState(CMD_PA_SIZELOCK, false);

	SliderSetValue(CMD_PA_SPINSTART, state.ps->info.fSpinStart);
	SliderSetValue(CMD_PA_SPINEND, state.ps->info.fSpinEnd);
	SliderSetValue(CMD_PA_SPINVAR, state.ps->info.fSpinVar);
	ButtonSetState(CMD_PA_SPINLOCK, false);

	SliderSetValue(CMD_PA_ALPHASTART, state.ps->info.colColorStart.a);
	SliderSetValue(CMD_PA_ALPHAEND, state.ps->info.colColorEnd.a);
	SliderSetValue(CMD_PA_ALPHAVAR, state.ps->info.fAlphaVar);
	ButtonSetState(CMD_PA_ALPHALOCK, false);

	SliderSetValue(CMD_PA_RSTART, state.ps->info.colColorStart.r);
	SliderSetValue(CMD_PA_GSTART, state.ps->info.colColorStart.g);
	SliderSetValue(CMD_PA_BSTART, state.ps->info.colColorStart.b);
	SliderSetValue(CMD_PA_REND, state.ps->info.colColorEnd.r);
	SliderSetValue(CMD_PA_GEND, state.ps->info.colColorEnd.g);
	SliderSetValue(CMD_PA_BEND, state.ps->info.colColorEnd.b);
	SliderSetValue(CMD_PA_RGBVAR, state.ps->info.fColorVar);

	col1=state.ps->info.colColorStart.GetHWColor() | 0xFF000000;
	col2=state.ps->info.colColorEnd.GetHWColor() | 0xFF000000;
	sprColor->SetColor(col1,0); sprColor->SetColor(col1,1);
	sprColor->SetColor(col2,2); sprColor->SetColor(col2,3);
}

void cmdChangeColor(int id)
{
	DWORD col1, col2;
	float val=SliderGetValue(id);

	switch(id)
	{
		case CMD_PA_RSTART: state.ps->info.colColorStart.r=val; break;
		case CMD_PA_GSTART: state.ps->info.colColorStart.g=val; break;
		case CMD_PA_BSTART: state.ps->info.colColorStart.b=val; break;
		case CMD_PA_REND: state.ps->info.colColorEnd.r=val; break;
		case CMD_PA_GEND: state.ps->info.colColorEnd.g=val; break;
		case CMD_PA_BEND:  state.ps->info.colColorEnd.b=val; break;
	}

	col1=state.ps->info.colColorStart.GetHWColor() | 0xFF000000;
	col2=state.ps->info.colColorEnd.GetHWColor() | 0xFF000000;
	sprColor->SetColor(col1,0); sprColor->SetColor(col1,1);
	sprColor->SetColor(col2,2); sprColor->SetColor(col2,3);
}