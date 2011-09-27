/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Common core implementation header
*/
#pragma once
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
    hgeChar				filename[_MAX_PATH];
    char 				password[64];
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
    virtual void        HGE_CALL    System_SetStateString(hgeStringState state, hgeConstString alue);
    virtual bool        HGE_CALL    System_GetStateBool  (hgeBoolState  );
    virtual hgeCallback HGE_CALL    System_GetStateFunc  (hgeFuncState  );
    virtual HWND        HGE_CALL    System_GetStateHwnd  (hgeHwndState  );
    virtual int         HGE_CALL    System_GetStateInt   (hgeIntState   );
    virtual hgeConstString HGE_CALL System_GetStateString(hgeStringState);
    virtual hgeConstString HGE_CALL System_GetErrorMessage();
    virtual void        HGE_CALL    System_Log(hgeConstString format, ...);
    virtual bool        HGE_CALL    System_Launch(hgeConstString url);
    virtual void        HGE_CALL    System_Snapshot(hgeConstString filename=0);

    virtual void*       HGE_CALL    Resource_Load(hgeConstString filename, uint32_t *size=0);
    virtual void        HGE_CALL    Resource_Free(void *res);
    virtual bool        HGE_CALL    Resource_AttachPack(hgeConstString filename, const char * password=0);
    virtual void        HGE_CALL    Resource_RemovePack(hgeConstString filename);
    virtual void        HGE_CALL    Resource_RemoveAllPacks();
    virtual hgeString   HGE_CALL    Resource_MakePath(hgeConstString filename=0);
    virtual hgeString   HGE_CALL    Resource_EnumFiles(hgeConstString wildcard=0);
    virtual hgeString   HGE_CALL    Resource_EnumFolders(hgeConstString wildcard=0);

    virtual void        HGE_CALL    Ini_SetInt(hgeConstString section, hgeConstString name, int value);
    virtual int         HGE_CALL    Ini_GetInt(hgeConstString section, hgeConstString name, int def_val);
    virtual void        HGE_CALL    Ini_SetFloat(hgeConstString section, hgeConstString name, float value);
    virtual float       HGE_CALL    Ini_GetFloat(hgeConstString section, hgeConstString name, float def_val);
    virtual void        HGE_CALL    Ini_SetString(hgeConstString section, hgeConstString name, hgeConstString value);
    virtual hgeString   HGE_CALL    Ini_GetString(hgeConstString section, hgeConstString name, hgeConstString def_val);

    virtual void        HGE_CALL    Random_Seed(int seed=0);
    virtual int         HGE_CALL    Random_Int(int min, int max);
    virtual float       HGE_CALL    Random_Float(float min, float max);

    virtual float       HGE_CALL    Timer_GetTime();
    virtual float       HGE_CALL    Timer_GetDelta();
    virtual int         HGE_CALL    Timer_GetFPS();

    virtual HEFFECT     HGE_CALL    Effect_Load(hgeConstString filename, uint32_t size=0);
    virtual void        HGE_CALL    Effect_Free(HEFFECT eff);
    virtual HCHANNEL    HGE_CALL    Effect_Play(HEFFECT eff);
    virtual HCHANNEL    HGE_CALL    Effect_PlayEx(HEFFECT eff, int volume=100, int pan=0, float pitch=1.0f, bool loop=false);

    virtual HMUSIC      HGE_CALL    Music_Load(hgeConstString filename, uint32_t size=0);
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

    virtual HSTREAM     HGE_CALL    Stream_Load(hgeConstString filename, uint32_t size=0);
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
    virtual hgeString   HGE_CALL    Input_GetKeyName(int key);
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
	virtual HSHADER		HGE_CALL	Shader_Create(hgeConstString filename);
	virtual void		HGE_CALL	Shader_Free(HSHADER shader);
	virtual void		HGE_CALL	Gfx_SetShader(HSHADER shader);
#endif

    virtual HTARGET     HGE_CALL    Target_Create(int width, int height, bool zbuffer);
    virtual void        HGE_CALL    Target_Free(HTARGET target);
    virtual HTEXTURE    HGE_CALL    Target_GetTexture(HTARGET target);

    virtual HTEXTURE    HGE_CALL    Texture_Create(int width, int height);
    virtual HTEXTURE    HGE_CALL    Texture_Load(hgeConstString filename, uint32_t size=0, bool bMipmap=false);
    virtual void        HGE_CALL    Texture_Free(HTEXTURE tex);
    virtual int         HGE_CALL    Texture_GetWidth(HTEXTURE tex, bool bOriginal=false);
    virtual int         HGE_CALL    Texture_GetHeight(HTEXTURE tex, bool bOriginal=false);
    virtual uint32_t*   HGE_CALL    Texture_Lock(HTEXTURE tex, bool bReadOnly=true, int left=0, int top=0, int width=0, int height=0);
    virtual void        HGE_CALL    Texture_Unlock(HTEXTURE tex);

    //////// Implementation ////////

    static HGE_Impl*    _Interface_Get();
    void                _FocusChange(bool bAct);
    void                _PostError( hgeConstString error );

    HINSTANCE           m_hinstance;
    HWND                m_hwnd;

    bool                m_active_flag;
    hgeChar				m_error_str[256];
    hgeChar				m_app_path[_MAX_PATH];
    hgeChar				m_ini_string[256];


    // System States
    bool                (*m_frame_func)();
    bool                (*m_render_func)();
    bool                (*m_focus_lost_func)();
    bool                (*m_focus_gain_func)();
    bool                (*m_gfx_restore_func)();
    bool                (*m_exit_func)();
    const hgeChar		* m_window_icon;
    hgeChar				m_window_title[256];
    int                 m_screen_width;
    int                 m_screen_height;
    int                 m_screen_color_depth;
    bool                m_windowed_flag;
    bool                m_zbuffer_flag;
    bool                m_texture_filter_flag;
    hgeChar				m_ini_file_path[_MAX_PATH];
    hgeChar				m_log_file_path[_MAX_PATH];
    bool                m_use_sound_flag;
    int                 m_sample_rate;
    int                 m_fx_volume;
    int                 m_mus_volume;
    int                 m_stream_volume;
    int                 m_framerate_limit;
    bool                m_hide_mouse_flag;
    bool                m_dont_suspend_flag;
    HWND                m_parent_hwnd;

    #ifdef DEMO
    bool                m_show_splash_flag;
    #endif


    // Power
    int                         m_power_status;
    HMODULE                     m_kernel32_module;
    GetSystemPowerStatusFunc    m_GetSystemPowerStatus_func;

    void                _InitPowerStatus();
    void                _UpdatePowerStatus();
    void                _DonePowerStatus();


    // Graphics
    D3DPRESENT_PARAMETERS*  m_d3d_pp;

    D3DPRESENT_PARAMETERS   m_d3d_pp_wnd;
    RECT                    m_rect_wnd;
    LONG                    m_style_wnd;

    D3DPRESENT_PARAMETERS   m_d3d_pp_fullscreen;
    RECT                    m_rect_fullscreen;
    LONG                    m_style_fullscreen;

    hgeGAPI *               m_d3d;
    hgeGAPIDevice *         m_d3d_device;
    hgeGAPIVertexBuffer *   m_vertex_buf;
    hgeGAPIIndexBuffer *    m_index_buf;

    hgeGAPISurface *    m_screen_surf;
    hgeGAPISurface *    m_depth_surf;
    CRenderTargetList*  m_targets_list;
    CRenderTargetList*  m_current_target;

    D3DXMATRIX          m_view_matrix;
    D3DXMATRIX          m_proj_matrix;

    CTextureList*       m_texture_list;
    hgeVertex*          m_vert_array;

    int                 m_prim;
    int                 m_current_prim_type;
    int                 m_current_blend_mode;
    HTEXTURE            m_current_texture;
#if HGE_DIRECTX_VER >= 9
	HSHADER				m_current_shader;
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
    HINSTANCE           m_bass_dll;
    bool                m_nosound_flag;
    CStreamList*        m_streams;
    bool                _SoundInit();
    void                _SoundDone();
    void                _SetMusVolume(int vol);
    void                _SetStreamVolume(int vol);
    void                _SetFXVolume(int vol);


    // Input
    int                 m_input_vkey;
    int                 m_input_char;
    int                 m_input_zpos;
    float               m_input_xpos;
    float               m_input_ypos;
    bool                m_input_mouseover_flag;
    bool                m_input_captured_flag;
    char                m_input_keys_table[256];
    CInputEventList*    m_input_queue;
    void                _UpdateMouse();
    void                _InputInit();
    void                _ClearQueue();
    void                _BuildEvent(int type, int key, int scan, int flags, int x, int y);


    // Resources
    hgeChar				m_res_temp_filename[_MAX_PATH];
    CResourceList*		m_res_list;
    HANDLE				m_res_search_handle;
    HGE_WINAPI_UNICODE_SUFFIX(WIN32_FIND_DATA)	m_res_search_data;


    // Timer
    float               m_time;
    float               m_time_delta;
    uint32_t            m_time_fixed_delta;
    int                 m_time_fps;
    uint32_t            m_time_t0, m_time_t0fps, m_time_dt;
    int                 m_time_cfps;


private:
    HGE_Impl();
};

extern HGE_Impl*        pHGE;

#endif

