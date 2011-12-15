/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hge_tut08 - The Big Calm
*/


// Copy the files "font2.fnt", "font2.png",
// and "objects.png" from the folder "precompiled" to
// the folder with executable file. Also copy hge.dll
// to the same folder.


// smaller sun & moon, underwater
// moon shape, hide stars behind the moon


#include <math.h>

#include "..\..\include\hge.h"
#include "..\..\include\hgecolor.h"
#include "..\..\include\hgesprite.h"
#include "..\..\include\hgedistort.h"
#include "..\..\include\hgefont.h"

// Pointer to the HGE interface (helper classes require this to work)

HGE			*hge=0;
hgeFont		*fnt=0;

// Simulation constants

#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600
#define NUM_STARS		100
#define SEA_SUBDIVISION 16

#define SKY_HEIGHT		(SCREEN_HEIGHT*0.6f)
#define STARS_HEIGHT	(SKY_HEIGHT*0.9f)
#define ORBITS_RADIUS	(SCREEN_WIDTH*0.43f)

hgeU32 skyTopColors[] = {0xFF15092A, 0xFF6C6480, 0xFF89B9D0};
hgeU32 skyBtmColors[] = {0xFF303E57, 0xFFAC7963, 0xFFCAD7DB};
hgeU32 seaTopColors[] = {0xFF3D546B, 0xFF927E76, 0xFF86A2AD};
hgeU32 seaBtmColors[] = {0xFF1E394C, 0xFF2F4E64, 0xFF2F4E64};

int seq[]={0, 0, 1, 2, 2, 2, 1, 0, 0};

// Simulation resource handles

HTEXTURE	texObjects;

hgeSprite	*sky;
hgeSprite	*sun;
hgeSprite	*moon;
hgeSprite	*glow;
hgeSprite	*seaglow;
hgeSprite	*star;

hgeDistortionMesh *sea;

hgeColor	colWhite;

// Simulation state variables

float time;		// 0-24 hrs
float speed;	// hrs per second

int	  seq_id;
float seq_residue;

float starX[NUM_STARS];  // x
float starY[NUM_STARS];  // y
float starS[NUM_STARS];  // scale
float starA[NUM_STARS];  // alpha

float seaP[SEA_SUBDIVISION]; // phase shift array

hgeColor colSkyTop;
hgeColor colSkyBtm;
hgeColor colSeaTop;
hgeColor colSeaBtm;

hgeColor colSun;
hgeColor colSunGlow;
hgeColor colMoon;
hgeColor colMoonGlow;
hgeColor colSeaGlow;

float sunX, sunY, sunS, sunGlowS;
float moonX, moonY, moonS, moonGlowS;
float seaGlowX, seaGlowSX, seaGlowSY;

// Simulation methods

bool InitSimulation();
void DoneSimulation();
void UpdateSimulation();
void RenderSimulation();


///////////////////////// Implementation ///////////////////////////


bool FrameFunc()
{
	// Process keys
	
	switch(hge->Input_GetKey())
	{
		case HGEK_0:		speed=0.0f; break;
		case HGEK_1:		speed=0.1f; break;
		case HGEK_2:		speed=0.2f; break;
		case HGEK_3:		speed=0.4f; break;
		case HGEK_4:		speed=0.8f; break;
		case HGEK_5:		speed=1.6f; break;
		case HGEK_6:		speed=3.2f; break;
		case HGEK_7:		speed=6.4f; break;
		case HGEK_8:		speed=12.8f; break;
		case HGEK_9:		speed=25.6f; break;
		case HGEK_ESCAPE:	return true;
	}

	// Update scene

	UpdateSimulation();

	return false;
}


bool RenderFunc()
{
	int hrs, mins, secs;
	float tmp;
	
	// Calculate display time

	hrs=(int)floorf(time);
	tmp=(time-hrs)*60.0f;
	mins=(int)floorf(tmp);
	secs=(int)floorf((tmp-mins)*60.0f);

	// Render scene
	
	hge->Gfx_BeginScene();
	RenderSimulation();	
	fnt->printf(7, 7, HGETEXT_LEFT, "Keys 1-9 to adjust simulation speed, 0 - real time\nFPS: %d", hge->Timer_GetFPS());
	fnt->printf(SCREEN_WIDTH-50, 7, HGETEXT_LEFT, "%02d:%02d:%02d", hrs, mins, secs);
	hge->Gfx_EndScene();

	return false;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	hge = hgeCreate(HGE_VERSION);

	// Set desired system states and initialize HGE

	hge->System_SetState(HGE_LOGFILE, "hge_tut08.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "HGE Tutorial 08 - The Big Calm");
	hge->System_SetState(HGE_USESOUND, false);
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_SCREENWIDTH, SCREEN_WIDTH);
	hge->System_SetState(HGE_SCREENHEIGHT, SCREEN_HEIGHT);
	hge->System_SetState(HGE_SCREENBPP, 32);

	if(hge->System_Initiate())
	{
		fnt=new hgeFont("font2.fnt");
		
		if(!InitSimulation())
		{
			// If one of the data files is not found, display an error message and shutdown
			MessageBox(NULL, "Can't load resources. See log for details.", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
			hge->System_Shutdown();
			hge->Release();
			return 0;
		}

		hge->System_Start();

		DoneSimulation();
		delete fnt;
	}

	hge->System_Shutdown();
	hge->Release();
	return 0;
}


float GetTime()
{
	SYSTEMTIME SysTime;
	float tmp;

	GetLocalTime(&SysTime);
	tmp=SysTime.wSecond;
	tmp=SysTime.wMinute+tmp/60.0f;
	tmp=SysTime.wHour+tmp/60.0f;

	return tmp;
}


bool InitSimulation()
{
	// Load texture
	
	texObjects=hge->Texture_Load("objects.png");
	if(!texObjects) return false;

	// Create sprites

	sky=new hgeSprite(0, 0, 0, SCREEN_WIDTH, SKY_HEIGHT);
	sea=new hgeDistortionMesh(SEA_SUBDIVISION, SEA_SUBDIVISION);
	sea->SetTextureRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT-SKY_HEIGHT);

	sun=new hgeSprite(texObjects,81,0,114,114);
	sun->SetHotSpot(57,57);
	moon=new hgeSprite(texObjects,0,0,81,81);
	moon->SetHotSpot(40,40);
	star=new hgeSprite(texObjects,72,81,9,9);
	star->SetHotSpot(5,5);

	glow=new hgeSprite(texObjects,128,128,128,128);
	glow->SetHotSpot(64,64);
	glow->SetBlendMode(BLEND_COLORADD | BLEND_ALPHABLEND | BLEND_NOZWRITE);
	seaglow=new hgeSprite(texObjects,128,224,128,32);
	seaglow->SetHotSpot(64,0);
	seaglow->SetBlendMode(BLEND_COLORADD | BLEND_ALPHAADD | BLEND_NOZWRITE);

	// Initialize simulation state

	colWhite.SetHWColor(0xFFFFFFFF);
	time=GetTime();
	speed=0.0f;

	for(int i=0;i<NUM_STARS;i++) // star positions
	{
		starX[i]=hge->Random_Float(0, SCREEN_WIDTH);
		starY[i]=hge->Random_Float(0, STARS_HEIGHT);
		starS[i]=hge->Random_Float(0.1f, 0.7f);
	}

	for(int i=0;i<SEA_SUBDIVISION;i++) // sea waves phase shifts
	{
		seaP[i]=i+hge->Random_Float(-15.0f, 15.0f);
	}

	// Systems are ready now!

	return true;
}


void DoneSimulation()
{
	// Delete sprites
	
	delete seaglow;
	delete glow;
	delete star;
	delete moon;
	delete sun;
		
	delete sky;
	delete sea;

	// Free texture

	hge->Texture_Free(texObjects);
}


void UpdateSimulation()
{
	int i, j, k;
	float zenith, a, dy, fTime;
	float posX, s1, s2;
	const float cellw=SCREEN_WIDTH/(SEA_SUBDIVISION-1);
	hgeColor col1, col2;
	hgeU32 dwCol1, dwCol2;

	// Update time of day

	if(speed==0.0f) time=GetTime();
	else
	{
		time+=hge->Timer_GetDelta()*speed;
		if(time>=24.0f) time-=24.0f;
	}

	seq_id=(int)(time/3);
	seq_residue=time/3-seq_id;
	zenith=-(time/12.0f*M_PI-M_PI_2);

	// Interpolate sea and sky colors

	col1.SetHWColor(skyTopColors[seq[seq_id]]);
	col2.SetHWColor(skyTopColors[seq[seq_id+1]]);
	colSkyTop=col2*seq_residue + col1*(1.0f-seq_residue);

	col1.SetHWColor(skyBtmColors[seq[seq_id]]);
	col2.SetHWColor(skyBtmColors[seq[seq_id+1]]);
	colSkyBtm=col2*seq_residue + col1*(1.0f-seq_residue);
	
	col1.SetHWColor(seaTopColors[seq[seq_id]]);
	col2.SetHWColor(seaTopColors[seq[seq_id+1]]);
	colSeaTop=col2*seq_residue + col1*(1.0f-seq_residue);

	col1.SetHWColor(seaBtmColors[seq[seq_id]]);
	col2.SetHWColor(seaBtmColors[seq[seq_id+1]]);
	colSeaBtm=col2*seq_residue + col1*(1.0f-seq_residue);

	// Update stars

	if(seq_id>=6 || seq_id<2)
		for(int i=0; i<NUM_STARS; i++)
		{
			a=1.0f-starY[i]/STARS_HEIGHT;
			a*=hge->Random_Float(0.6f, 1.0f);
			if(seq_id>=6) a*=sinf((time-18.0f)/6.0f*M_PI_2);
			else a*=sinf((1.0f-time/6.0f)*M_PI_2);
			starA[i]=a;
		}

	// Calculate sun position, scale and colors

	     if(seq_id==2) a=sinf(seq_residue*M_PI_2);
	else if(seq_id==5) a=cosf(seq_residue*M_PI_2);
	else if(seq_id>2 && seq_id<5) a=1.0f;
	else a=0.0f;
	
	colSun.SetHWColor(0xFFEAE1BE);
	colSun=colSun*(1-a)+colWhite*a;

	a=(cosf(time/6.0f*M_PI)+1.0f)/2.0f;
	if(seq_id>=2 && seq_id<=6)
	{
		colSunGlow=colWhite*a;
		colSunGlow.a=1.0f;
	}
	else colSunGlow.SetHWColor(0xFF000000);

	sunX=SCREEN_WIDTH*0.5f+cosf(zenith)*ORBITS_RADIUS;
	sunY=SKY_HEIGHT*1.2f+sinf(zenith)*ORBITS_RADIUS;
	sunS=1.0f-0.3f*sinf((time-6.0f)/12.0f*M_PI);
	sunGlowS=3.0f*(1.0f-a)+3.0f;

	// Calculate moon position, scale and colors

	if(seq_id>=6) a=sinf((time-18.0f)/6.0f*M_PI_2);
	else a=sinf((1.0f-time/6.0f)*M_PI_2);
	colMoon.SetHWColor(0x20FFFFFF);
	colMoon=colMoon*(1-a)+colWhite*a;

	colMoonGlow=colWhite;
	colMoonGlow.a=0.5f*a;

	moonX=SCREEN_WIDTH*0.5f+cosf(zenith-M_PI)*ORBITS_RADIUS;
	moonY=SKY_HEIGHT*1.2f+sinf(zenith-M_PI)*ORBITS_RADIUS;
	moonS=1.0f-0.3f*sinf((time+6.0f)/12.0f*M_PI);
	moonGlowS=a*0.4f+0.5f;

	// Calculate sea glow

	if(time>19.0f || time<4.5f) // moon
	{
		a=0.2f; // intensity
		if(time>19.0f && time<20.0f) a*=(time-19.0f);
		else if(time>3.5f && time<4.5f) a*=1.0f-(time-3.5f);

		colSeaGlow=colMoonGlow;
		colSeaGlow.a=a;
		seaGlowX=moonX;
		seaGlowSX=moonGlowS*3.0f;
		seaGlowSY=moonGlowS*2.0f;
	}
	else if(time>6.5f && time<19.0f) // sun
	{
		a=0.3f; // intensity
		if(time<7.5f) a*=(time-6.5f);
		else if(time>18.0f) a*=1.0f-(time-18.0f);

		colSeaGlow=colSunGlow;
		colSeaGlow.a=a;
		seaGlowX=sunX;
		seaGlowSX=sunGlowS;
		seaGlowSY=sunGlowS*0.6f;
	}
	else colSeaGlow.a=0.0f;

	// Move waves and update sea color

	for(i=1; i<SEA_SUBDIVISION-1; i++)
	{
		a=float(i)/(SEA_SUBDIVISION-1);
		col1=colSeaTop*(1-a)+colSeaBtm*a;
		dwCol1=col1.GetHWColor();
		fTime=2.0f*hge->Timer_GetTime();
		a*=20;

		for(j=0; j<SEA_SUBDIVISION; j++)
		{
			sea->SetColor(j, i, dwCol1);

			dy=a*sinf(seaP[i]+(float(j)/(SEA_SUBDIVISION-1)-0.5f)*M_PI*16.0f-fTime);
			sea->SetDisplacement(j, i, 0.0f, dy, HGEDISP_NODE);
		}
	}

	dwCol1=colSeaTop.GetHWColor();
	dwCol2=colSeaBtm.GetHWColor();

	for(j=0; j<SEA_SUBDIVISION; j++)
	{
		sea->SetColor(j, 0, dwCol1);
		sea->SetColor(j, SEA_SUBDIVISION-1, dwCol2);
	}

	// Calculate light path

	if(time>19.0f || time<5.0f) // moon
	{
		a=0.12f; // intensity
		if(time>19.0f && time<20.0f) a*=(time-19.0f);
		else if(time>4.0f && time<5.0f) a*=1.0f-(time-4.0f);
		posX=moonX;
	}
	else if(time>7.0f && time<17.0f) // sun
	{
		a=0.14f; // intensity
		if(time<8.0f) a*=(time-7.0f);
		else if(time>16.0f) a*=1.0f-(time-16.0f);
		posX=sunX;
	}
	else a=0.0f;

	if(a!=0.0f)
	{
		k=(int)floorf(posX/cellw);
		s1=(1.0f-(posX-k*cellw)/cellw);
		s2=(1.0f-((k+1)*cellw-posX)/cellw);

		if(s1>0.7f) s1=0.7f;
		if(s2>0.7f) s2=0.7f;

		s1*=a;
		s2*=a;
	
		for(i=0; i<SEA_SUBDIVISION; i+=2)
		{
			a=sinf(float(i)/(SEA_SUBDIVISION-1)*M_PI_2);

			col1.SetHWColor(sea->GetColor(k,i));
			col1+=colSun*s1*(1-a);
			col1.Clamp();
			sea->SetColor(k, i, col1.GetHWColor());
			
			col1.SetHWColor(sea->GetColor(k+1,i));
			col1+=colSun*s2*(1-a);
			col1.Clamp();
			sea->SetColor(k+1, i, col1.GetHWColor());
		}
	}
}


void RenderSimulation()
{
	// Render sky

	sky->SetColor(colSkyTop.GetHWColor(), 0);
	sky->SetColor(colSkyTop.GetHWColor(), 1);
	sky->SetColor(colSkyBtm.GetHWColor(), 2);
	sky->SetColor(colSkyBtm.GetHWColor(), 3);
	sky->Render(0, 0);

	// Render stars

	if(seq_id>=6 || seq_id<2)
		for(int i=0; i<NUM_STARS; i++)
		{
			star->SetColor((hgeU32(starA[i]*255.0f)<<24) | 0xFFFFFF);
			star->RenderEx(starX[i], starY[i], 0.0f, starS[i]);
		}

	// Render sun
	
	glow->SetColor(colSunGlow.GetHWColor());
	glow->RenderEx(sunX, sunY, 0.0f, sunGlowS);
	sun->SetColor(colSun.GetHWColor());
	sun->RenderEx(sunX, sunY, 0.0f, sunS);

	// Render moon

	glow->SetColor(colMoonGlow.GetHWColor());
	glow->RenderEx(moonX, moonY, 0.0f, moonGlowS);
	moon->SetColor(colMoon.GetHWColor());
	moon->RenderEx(moonX, moonY, 0.0f, moonS);

	// Render sea

	sea->Render(0, SKY_HEIGHT);
	seaglow->SetColor(colSeaGlow.GetHWColor());
	seaglow->RenderEx(seaGlowX, SKY_HEIGHT, 0.0f, seaGlowSX, seaGlowSY);
}