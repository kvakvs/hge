/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Common core implementation header
*/


#ifndef HGE_IMPL_H
#define HGE_IMPL_H

#include <hge.h>
#include <stdio.h>
#include <hge_gapi.h>


#define DEMO

#define D3DFVF_HGEVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define VERTEX_BUFFER_SIZE 4000


typedef BOOL (WINAPI *GetSystemPowerStatusFunc)(LPSYSTEM_POWER_STATUS);


struct CRenderTargetList
{
    int                 width;
    int                 height;
    hgeGAPITexture *    pTex;
    hgeGAPISurface *    pDepth;
    CRenderTargetList * next;
};

struct CTextureList
{
    HTEXTURE            tex;
    int                 width;
    int                 height;
    CTextureList*       next;
};

struct CResourceList
{
    char                filename[_MAX_PATH];
    char                password[64];
    CResourceList*      next;
};

struct CStreamList
{
    HSTREAM             hstream;
    void*               data;
    CStreamList*        next;
};

struct CInputEventList
{
    hgeInputEvent       event;
    CInputEventList*    next;
};


void DInit();
void DDone();
bool DFrame();


/*
** HGE Interface implementation
*/
class HGE_Impl : public HGE
{
public:
    virtual void        HGE_CALL    Release();

    virtual bool        HGE_CALL    System_Initiate();
    virtual void        HGE_CALL    System_Shutdown();
    virtual bool        HGE_CALL    System_Start();
    virtual void        HGE_CALL    System_SetStateBool  (hgeBoolState   state, bool        value);
    virtual void        HGE_CALL    System_SetStateFunc  (hgeFuncState   state, hgeCallback value);
    virtual void        HGE_CALL    System_SetStateHwnd  (hgeHwndState   state, HWND        value);
    virtual void        HGE_CALL    System_SetStateInt   (hgeIntState    state, int         value);
    virtual void        HGE_CALL    System_SetStateString(hgeStringState state, const char *value);
    virtual bool        HGE_CALL    System_GetStateBool  (hgeBoolState  );
    virtual hgeCallback HGE_CALL    System_GetStateFunc  (hgeFuncState  );
    virtual HWND        HGE_CALL    System_GetStateHwnd  (hgeHwndState  );
    virtual int         HGE_CALL    System_GetStateInt   (hgeIntState   );
    virtual const char* HGE_CALL    System_GetStateString(hgeStringState);
    virtual char*       HGE_CALL    System_GetErrorMessage();
    virtual void        HGE_CALL    System_Log(const char *format, ...);
    virtual bool        HGE_CALL    System_Launch(const char *url);
    virtual void        HGE_CALL    System_Snapshot(const char *filename=0);

    virtual void*       HGE_CALL    Resource_Load(const char *filename, uint32_t *size=0);
    virtual void        HGE_CALL    Resource_Free(void *res);
    virtual bool        HGE_CALL    Resource_AttachPack(const char *filename, const char *password=0);
    virtual void        HGE_CALL    Resource_RemovePack(const char *filename);
    virtual void        HGE_CALL    Resource_RemoveAllPacks();
    virtual char*       HGE_CALL    Resource_MakePath(const char *filename=0);
    virtual char*       HGE_CALL    Resource_EnumFiles(const char *wildcard=0);
    virtual char*       HGE_CALL    Resource_EnumFolders(const char *wildcard=0);

    virtual void        HGE_CALL    Ini_SetInt(const char *section, const char *name, int value);
    virtual int         HGE_CALL    Ini_GetInt(const char *section, const char *name, int def_val);
    virtual void        HGE_CALL    Ini_SetFloat(const char *section, const char *name, float value);
    virtual float       HGE_CALL    Ini_GetFloat(const char *section, const char *name, float def_val);
    virtual void        HGE_CALL    Ini_SetString(const char *section, const char *name, const char *value);
    virtual char*       HGE_CALL    Ini_GetString(const char *section, const char *name, const char *def_val);

    virtual void        HGE_CALL    Random_Seed(int seed=0);
    virtual int         HGE_CALL    Random_Int(int min, int max);
    virtual float       HGE_CALL    Random_Float(float min, float max);

    virtual float       HGE_CALL    Timer_GetTime();
    virtual float       HGE_CALL    Timer_GetDelta();
    virtual int         HGE_CALL    Timer_GetFPS();

    virtual HEFFECT     HGE_CALL    Effect_Load(const char *filename, uint32_t size=0);
    virtual void        HGE_CALL    Effect_Free(HEFFECT eff);
    virtual HCHANNEL    HGE_CALL    Effect_Play(HEFFECT eff);
    virtual HCHANNEL    HGE_CALL    Effect_PlayEx(HEFFECT eff, int volume=100, int pan=0, float pitch=1.0f, bool loop=false);

    virtual HMUSIC      HGE_CALL    Music_Load(const char *filename, uint32_t size=0);
    virtual void        HGE_CALL    Music_Free(HMUSIC mus);
    virtual HCHANNEL    HGE_CALL    Music_Play(HMUSIC mus, bool loop, int volume = 100, int order = 0, int row = 0);
    virtual void        HGE_CALL    Music_SetAmplification(HMUSIC music, int ampl);
    virtual int         HGE_CALL    Music_GetAmplification(HMUSIC music);
    virtual int         HGE_CALL    Music_GetLength(HMUSIC music);
    virtual void        HGE_CALL    Music_SetPos(HMUSIC music, int order, int row);
    virtual bool        HGE_CALL    Music_GetPos(HMUSIC music, int *order, int *row);
    virtual void        HGE_CALL    Music_SetInstrVolume(HMUSIC music, int instr, int volume);
    virtual int         HGE_CALL    Music_GetInstrVolume(HMUSIC music, int instr);
    virtual void        HGE_CALL    Music_SetChannelVolume(HMUSIC music, int channel, int volume);
    virtual int         HGE_CALL    Music_GetChannelVolume(HMUSIC music, int channel);

    virtual HSTREAM     HGE_CALL    Stream_Load(const char *filename, uint32_t size=0);
    virtual void        HGE_CALL    Stream_Free(HSTREAM stream);
    virtual HCHANNEL    HGE_CALL    Stream_Play(HSTREAM stream, bool loop, int volume = 100);

    virtual void        HGE_CALL    Channel_SetPanning(HCHANNEL chn, int pan);
    virtual void        HGE_CALL    Channel_SetVolume(HCHANNEL chn, int volume);
    virtual void        HGE_CALL    Channel_SetPitch(HCHANNEL chn, float pitch);
    virtual void        HGE_CALL    Channel_Pause(HCHANNEL chn);
    virtual void        HGE_CALL    Channel_Resume(HCHANNEL chn);
    virtual void        HGE_CALL    Channel_Stop(HCHANNEL chn);
    virtual void        HGE_CALL    Channel_PauseAll();
    virtual void        HGE_CALL    Channel_ResumeAll();
    virtual void        HGE_CALL    Channel_StopAll();
    virtual bool        HGE_CALL    Channel_IsPlaying(HCHANNEL chn);
    virtual float       HGE_CALL    Channel_GetLength(HCHANNEL chn);
    virtual float       HGE_CALL    Channel_GetPos(HCHANNEL chn);
    virtual void        HGE_CALL    Channel_SetPos(HCHANNEL chn, float fSeconds);
    virtual void        HGE_CALL    Channel_SlideTo(HCHANNEL channel, float time, int volume, int pan = -101, float pitch = -1);
    virtual bool        HGE_CALL    Channel_IsSliding(HCHANNEL channel);

    virtual void        HGE_CALL    Input_GetMousePos(float *x, float *y);
    virtual void        HGE_CALL    Input_SetMousePos(float x, float y);
    virtual int         HGE_CALL    Input_GetMouseWheel();
    virtual bool        HGE_CALL    Input_IsMouseOver();
    virtual bool        HGE_CALL    Input_KeyDown(int key);
    virtual bool        HGE_CALL    Input_KeyUp(int key);
    virtual bool        HGE_CALL    Input_GetKeyState(int key);
    virtual char*       HGE_CALL    Input_GetKeyName(int key);
    virtual int         HGE_CALL    Input_GetKey();
    virtual int         HGE_CALL    Input_GetChar();
    virtual bool        HGE_CALL    Input_GetEvent(hgeInputEvent *event);

    virtual bool        HGE_CALL    Gfx_BeginScene(HTARGET target=0);
    virtual void        HGE_CALL    Gfx_EndScene();
    virtual void        HGE_CALL    Gfx_Clear(uint32_t color);
    virtual void        HGE_CALL    Gfx_RenderLine(float x1, float y1, float x2, float y2, uint32_t color=0xFFFFFFFF, float z=0.5f);
    virtual void        HGE_CALL    Gfx_RenderTriple(const hgeTriple *triple);
    virtual void        HGE_CALL    Gfx_RenderQuad(const hgeQuad *quad);
    virtual hgeVertex*  HGE_CALL    Gfx_StartBatch(int prim_type, HTEXTURE tex, int blend, int *max_prim);
    virtual void        HGE_CALL    Gfx_FinishBatch(int nprim);
    virtual void        HGE_CALL    Gfx_SetClipping(int x=0, int y=0, int w=0, int h=0);
    virtual void        HGE_CALL    Gfx_SetTransform(float x=0, float y=0, float dx=0, float dy=0, float rot=0, float hscale=0, float vscale=0); 

#if HGE_DIRECTX_VER >= 9
	virtual HSHADER		HGE_CALL	Shader_Create(const char *filename);
	virtual void		HGE_CALL	Shader_Free(HSHADER shader);
	virtual void		HGE_CALL	Gfx_SetShader(HSHADER shader);
#endif

    virtual HTARGET     HGE_CALL    Target_Create(int width, int height, bool zbuffer);
    virtual void        HGE_CALL    Target_Free(HTARGET target);
    virtual HTEXTURE    HGE_CALL    Target_GetTexture(HTARGET target);

    virtual HTEXTURE    HGE_CALL    Texture_Create(int width, int height);
    virtual HTEXTURE    HGE_CALL    Texture_Load(const char *filename, uint32_t size=0, bool bMipmap=false);
    virtual void        HGE_CALL    Texture_Free(HTEXTURE tex);
    virtual int         HGE_CALL    Texture_GetWidth(HTEXTURE tex, bool bOriginal=false);
    virtual int         HGE_CALL    Texture_GetHeight(HTEXTURE tex, bool bOriginal=false);
    virtual uint32_t*      HGE_CALL    Texture_Lock(HTEXTURE tex, bool bReadOnly=true, int left=0, int top=0, int width=0, int height=0);
    virtual void        HGE_CALL    Texture_Unlock(HTEXTURE tex);

    //////// Implementation ////////

    static HGE_Impl*    _Interface_Get();
    void                _FocusChange(bool bAct);
    void                _PostError(char *error);


    HINSTANCE           hInstance;
    HWND                hwnd;

    bool                bActive;
    char                szError[256];
    char                szAppPath[_MAX_PATH];
    char                szIniString[256];


    // System States
    bool                (*procFrameFunc)();
    bool                (*procRenderFunc)();
    bool                (*procFocusLostFunc)();
    bool                (*procFocusGainFunc)();
    bool                (*procGfxRestoreFunc)();
    bool                (*procExitFunc)();
    const char*         szIcon;
    char                szWinTitle[256];
    int                 nScreenWidth;
    int                 nScreenHeight;
    int                 nScreenBPP;
    bool                bWindowed;
    bool                bZBuffer;
    bool                bTextureFilter;
    char                szIniFile[_MAX_PATH];
    char                szLogFile[_MAX_PATH];
    bool                bUseSound;
    int                 nSampleRate;
    int                 nFXVolume;
    int                 nMusVolume;
    int                 nStreamVolume;
    int                 nHGEFPS;
    bool                bHideMouse;
    bool                bDontSuspend;
    HWND                hwndParent;

    #ifdef DEMO
    bool                bDMO;
    #endif


    // Power
    int                         nPowerStatus;
    HMODULE                     hKrnl32;
    GetSystemPowerStatusFunc    lpfnGetSystemPowerStatus;

    void                _InitPowerStatus();
    void                _UpdatePowerStatus();
    void                _DonePowerStatus();


    // Graphics
    D3DPRESENT_PARAMETERS*  d3dpp;

    D3DPRESENT_PARAMETERS   d3dppW;
    RECT                    rectW;
    LONG                    styleW;

    D3DPRESENT_PARAMETERS   d3dppFS;
    RECT                    rectFS;
    LONG                    styleFS;

    hgeGAPI *               pD3D;
    hgeGAPIDevice *         pD3DDevice;
    hgeGAPIVertexBuffer *   pVB;
    hgeGAPIIndexBuffer *    pIB;

    hgeGAPISurface *    pScreenSurf;
    hgeGAPISurface *    pScreenDepth;
    CRenderTargetList*  pTargets;
    CRenderTargetList*  pCurTarget;

    D3DXMATRIX          matView;
    D3DXMATRIX          matProj;

    CTextureList*       textures;
    hgeVertex*          VertArray;

    int                 nPrim;
    int                 CurPrimType;
    int                 CurBlendMode;
    HTEXTURE            CurTexture;
#if HGE_DIRECTX_VER >= 9
	HSHADER				CurShader;
#endif

    bool                _GfxInit();
    void                _GfxDone();
    bool                _GfxRestore();
    void                _AdjustWindow();
    void                _Resize(int width, int height);
    bool                _init_lost();
    void                _render_batch(bool bEndScene=false);
    int                 _format_id(D3DFORMAT fmt);
    void                _SetBlendMode(int blend);
    void                _SetProjectionMatrix(int width, int height);
    

    // Audio
    HINSTANCE           hBass;
    bool                bSilent;
    CStreamList*        streams;
    bool                _SoundInit();
    void                _SoundDone();
    void                _SetMusVolume(int vol);
    void                _SetStreamVolume(int vol);
    void                _SetFXVolume(int vol);


    // Input
    int                 VKey;
    int                 Char;
    int                 Zpos;
    float               Xpos;
    float               Ypos;
    bool                bMouseOver;
    bool                bCaptured;
    char                keyz[256];
    CInputEventList*    queue;
    void                _UpdateMouse();
    void                _InputInit();
    void                _ClearQueue();
    void                _BuildEvent(int type, int key, int scan, int flags, int x, int y);


    // Resources
    char                szTmpFilename[_MAX_PATH];
    CResourceList*      res;
    HANDLE              hSearch;
    WIN32_FIND_DATA     SearchData;


    // Timer
    float               fTime;
    float               fDeltaTime;
    uint32_t               nFixedDelta;
    int                 nFPS;
    uint32_t               t0, t0fps, dt;
    int                 cfps;


private:
    HGE_Impl();
};

extern HGE_Impl*        pHGE;

#endif

