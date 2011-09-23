/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hge_tut05 - Using distortion mesh
*/


// Copy the files "particles.png", "menu.wav",
// "font1.fnt", "font1.png" and "trail.psi" from
// the folder "precompiled" to the folder with
// executable file. Also copy hge.dll and bass.dll
// to the same folder.


#include "..\..\include\hge.h"
#include "..\..\include\hgefont.h"
#include "..\..\include\hgedistort.h"

#include <math.h>


// Pointer to the HGE interface.
// Helper classes require this to work.
HGE *hge=0;

HTEXTURE			tex;

// Pointers to the HGE objects we will use
hgeDistortionMesh*	dis;
hgeFont*			fnt;

// Some "gameplay" variables
const int nRows=16;
const int nCols=16;
const float cellw=512.0f/(nCols-1);
const float cellh=512.0f/(nRows-1);

const float meshx=144;
const float meshy=44;


bool FrameFunc()
{
	float dt=hge->Timer_GetDelta();
	static float t=0.0f;
	static int trans=0;

	int i, j, col;
	float r, a, dx, dy;

	t+=dt;

	// Process keys
	switch(hge->Input_GetKey())
	{
		case HGEK_ESCAPE:
			return true;

		case HGEK_SPACE:
			if(++trans > 2) trans=0;
			dis->Clear(0xFF000000);
			break;
	}
	
	// Calculate new displacements and coloring for one of the three effects
	switch(trans)
	{
		case 0:	for(i=1;i<nRows-1;i++)
					for(j=1;j<nCols-1;j++)
					{
						dis->SetDisplacement(j,i,cosf(t*10+(i+j)/2)*5,sinf(t*10+(i+j)/2)*5,HGEDISP_NODE);
					}
				break;

		case 1:	for(i=0;i<nRows;i++)
					for(j=1;j<nCols-1;j++)
					{
						dis->SetDisplacement(j,i,cosf(t*5+j/2)*15,0,HGEDISP_NODE);
						col=int((cosf(t*5+(i+j)/2)+1)*35);
						dis->SetColor(j,i,0xFF<<24 | col<<16 | col<<8 | col);
					}
				break;

		case 2:	for(i=0;i<nRows;i++)
					for(j=0;j<nCols;j++)
					{
						r=sqrtf(powf(j-(float)nCols/2,2)+powf(i-(float)nRows/2,2));
						a=r*cosf(t*2)*0.1f;
						dx=sinf(a)*(i*cellh-256)+cosf(a)*(j*cellw-256);
						dy=cosf(a)*(i*cellh-256)-sinf(a)*(j*cellw-256);
						dis->SetDisplacement(j,i,dx,dy,HGEDISP_CENTER);
						col=int((cos(r+t*4)+1)*40);
						dis->SetColor(j,i,0xFF<<24 | col<<16 | (col/2)<<8);
					}
					break;
	}

	return false;
}


bool RenderFunc()
{
	// Render graphics
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);
	dis->Render(meshx, meshy);
	fnt->printf(5, 5, HGETEXT_LEFT, "dt:%.3f\nFPS:%d\n\nUse your\nSPACE!", hge->Timer_GetDelta(), hge->Timer_GetFPS());
	hge->Gfx_EndScene();

	return false;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	hge = hgeCreate(HGE_VERSION);

	hge->System_SetState(HGE_LOGFILE, "hge_tut05.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "HGE Tutorial 05 - Using distortion mesh");
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_SCREENWIDTH, 800);
	hge->System_SetState(HGE_SCREENHEIGHT, 600);
	hge->System_SetState(HGE_SCREENBPP, 32);
	hge->System_SetState(HGE_USESOUND, false);

	if(hge->System_Initiate()) {

		// Load sound and texture
		tex=hge->Texture_Load("texture.jpg");
		if(!tex)
		{
			// If one of the data files is not found, display
			// an error message and shutdown.
			MessageBox(NULL, "Can't load TEXTURE.JPG", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
			hge->System_Shutdown();
			hge->Release();
			return 0;
		}

		// Create a distortion mesh
		dis=new hgeDistortionMesh(nCols, nRows);
		dis->SetTexture(tex);
		dis->SetTextureRect(0,0,512,512);
		dis->SetBlendMode(BLEND_COLORADD | BLEND_ALPHABLEND | BLEND_ZWRITE);
		dis->Clear(0xFF000000);

		// Load a font
		fnt=new hgeFont("font1.fnt");

		// Let's rock now!
		hge->System_Start();

		// Delete created objects and free loaded resources
		delete fnt;
		delete dis;
		hge->Texture_Free(tex);
	}

	// Clean up and shutdown
	hge->System_Shutdown();
	hge->Release();
	return 0;
}
