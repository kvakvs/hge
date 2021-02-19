/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Common core implementation header
//

#pragma once

#include <hge.h>
#include <cstdio>
#include <impl/hgegapi.h>
#include <string>
#include <queue>
#include <list>


#define HGE_SPLASH_ENABLE

namespace hgeImpl {

    const int D3DFVF_HGEVERTEX = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
    const int VERTEX_BUFFER_SIZE = 4000;


    typedef BOOL (WINAPI *GetSystemPowerStatusFunc)(LPSYSTEM_POWER_STATUS);

// HTARGET internal structure
    struct CRenderTargetList {
        int width;
        int height;
        hgeGAPITexture *pTex;
        hgeGAPISurface *pDepth;
        CRenderTargetList *next;
    };

    // HTEXTURE internal structure
    struct CTextureList {
        HTEXTURE tex;
        int width;
        int height;
        CTextureList *next;
    };

    // Contains info about attached resource pack
    struct ResourcePackInfo {
        std::string filename;
        std::string password;

        ResourcePackInfo(const char *filename, const char *password)
                : filename(filename),
                  password(password ? password : "") {}
    };

    struct CStreamList {
        HSTREAM hstream;
        void *data;
        CStreamList *next;
    };

    void prepare_demo();

    void finish_demo();

    bool demo_render_frame();


    //
    // HGE Interface implementation
    //
    class HGE_Impl : public HGE {
    public:
        void HGE_CALL Release() override;

        bool HGE_CALL System_Initiate() override;

        void HGE_CALL System_Shutdown() override;

        bool HGE_CALL System_Start() override;

        void HGE_CALL System_SetStateBool(hgeBoolState state, bool value) override;

        void HGE_CALL System_SetStateFunc(hgeFuncState state, hgeCallback value) override;

        void HGE_CALL System_SetStateHwnd(hgeHwndState state, HWND value) override;

        void HGE_CALL System_SetStateInt(hgeIntState state, int value) override;

        void HGE_CALL System_SetStateString(hgeStringState state, const char *value) override;

        bool HGE_CALL System_GetStateBool(hgeBoolState) override;

        hgeCallback HGE_CALL System_GetStateFunc(hgeFuncState) override;

        HWND HGE_CALL System_GetStateHwnd(hgeHwndState) override;

        int HGE_CALL System_GetStateInt(hgeIntState) override;

        const char *HGE_CALL System_GetStateString(hgeStringState) override;

        const char *HGE_CALL System_GetErrorMessage() const override;

        // NOLINTNEXTLINE
        void HGE_CALL System_Log(const char *format, ...) override;

        bool HGE_CALL System_Launch(const char *url) override;

        // NOLINTNEXTLINE
        void HGE_CALL System_Snapshot(const char *filename = nullptr) override;

        // NOLINTNEXTLINE
        void *HGE_CALL Resource_Load(const char *filename, uint32_t *size = nullptr) override;

        void HGE_CALL Resource_Free(void *res) override;

        // NOLINTNEXTLINE
        bool HGE_CALL Resource_AttachPack(
                const char *filename, const char *password = nullptr) override;

        void HGE_CALL Resource_RemovePack(const char *filename) override;

        void HGE_CALL Resource_RemoveAllPacks() override;

        // NOLINTNEXTLINE
        const char *HGE_CALL Resource_MakePath(const char *filename = nullptr) override;

        // NOLINTNEXTLINE
        const char *HGE_CALL Resource_EnumFiles(const char *wildcard = nullptr) override;

        // NOLINTNEXTLINE
        const char *HGE_CALL Resource_EnumFolders(const char *wildcard = nullptr) override;

        void HGE_CALL Ini_SetInt(const char *section, const char *name, int value) override;

        int HGE_CALL Ini_GetInt(const char *section, const char *name, int def_val) override;

        void HGE_CALL Ini_SetFloat(const char *section, const char *name, float value) override;

        float HGE_CALL Ini_GetFloat(const char *section, const char *name, float def_val) override;

        void HGE_CALL Ini_SetString(const char *section, const char *name,
                                    const char *value) override;

        const char *HGE_CALL Ini_GetString(const char *section, const char *name,
                                           const char *def_val) override;

        // NOLINTNEXTLINE
        void HGE_CALL Random_Seed(int seed = 0) override;

        int HGE_CALL Random_Int(int min, int max) override;

        float HGE_CALL Random_Float(float min, float max) override;

        float HGE_CALL Timer_GetTime() override;

        float HGE_CALL Timer_GetDelta() override;

        int HGE_CALL Timer_GetFPS() override;

        // NOLINTNEXTLINE
        HEFFECT HGE_CALL Effect_Load(const char *filename, uint32_t size = 0) override;

        void HGE_CALL Effect_Free(HEFFECT eff) override;

        HCHANNEL HGE_CALL Effect_Play(HEFFECT eff) override;

        // NOLINTNEXTLINE
        HCHANNEL HGE_CALL Effect_PlayEx(HEFFECT eff, int volume = 100, int pan = 0,
                                        float pitch = 1.0f, bool loop = false) override;

        // NOLINTNEXTLINE
        HMUSIC HGE_CALL Music_Load(const char *filename, uint32_t size = 0) override;

        void HGE_CALL Music_Free(HMUSIC mus) override;

        // NOLINTNEXTLINE
        HCHANNEL HGE_CALL Music_Play(HMUSIC mus, bool loop, int volume = 100, int order = 0,
                                     int row = 0) override;

        void HGE_CALL Music_SetAmplification(HMUSIC music, int ampl) override;

        int HGE_CALL Music_GetAmplification(HMUSIC music) override;

        int HGE_CALL Music_GetLength(HMUSIC music) override;

        void HGE_CALL Music_SetPos(HMUSIC music, int order, int row) override;

        bool HGE_CALL Music_GetPos(HMUSIC music, int *order, int *row) override;

        void HGE_CALL Music_SetInstrVolume(HMUSIC music, int instr, int volume) override;

        int HGE_CALL Music_GetInstrVolume(HMUSIC music, int instr) override;

        void HGE_CALL Music_SetChannelVolume(HMUSIC music, int channel, int volume) override;

        int HGE_CALL Music_GetChannelVolume(HMUSIC music, int channel) override;

        // NOLINTNEXTLINE
        HSTREAM HGE_CALL Stream_Load(const char *filename, uint32_t size = 0) override;

        void HGE_CALL Stream_Free(HSTREAM stream) override;

        // NOLINTNEXTLINE
        HCHANNEL HGE_CALL Stream_Play(HSTREAM stream, bool loop, int volume = 100) override;

        void HGE_CALL Channel_SetPanning(HCHANNEL chn, int pan) override;

        void HGE_CALL Channel_SetVolume(HCHANNEL chn, int volume) override;

        void HGE_CALL Channel_SetPitch(HCHANNEL chn, float pitch) override;

        void HGE_CALL Channel_Pause(HCHANNEL chn) override;

        void HGE_CALL Channel_Resume(HCHANNEL chn) override;

        void HGE_CALL Channel_Stop(HCHANNEL chn) override;

        void HGE_CALL Channel_PauseAll() override;

        void HGE_CALL Channel_ResumeAll() override;

        void HGE_CALL Channel_StopAll() override;

        bool HGE_CALL Channel_IsPlaying(HCHANNEL chn) override;

        float HGE_CALL Channel_GetLength(HCHANNEL chn) override;

        float HGE_CALL Channel_GetPos(HCHANNEL chn) override;

        void HGE_CALL Channel_SetPos(HCHANNEL chn, float fSeconds) override;

        // NOLINTNEXTLINE
        void HGE_CALL Channel_SlideTo(HCHANNEL channel, float time, int volume,
                                      int pan = -101, float pitch = -1) override;

        bool HGE_CALL Channel_IsSliding(HCHANNEL channel) override;

        void HGE_CALL Input_GetMousePos(float *x, float *y) override;

        void HGE_CALL Input_SetMousePos(float x, float y) override;

        int HGE_CALL Input_GetMouseWheel() override;

        bool HGE_CALL Input_IsMouseOver() override;

        bool HGE_CALL Input_KeyDown(int key) override;

        bool HGE_CALL Input_KeyUp(int key) override;

        bool HGE_CALL Input_GetKeyState(int key) override;

        const char *HGE_CALL Input_GetKeyName(int key) override;

        int HGE_CALL Input_GetKey() override;

        int HGE_CALL Input_GetChar() override;

        bool HGE_CALL Input_GetEvent(hgeInputEvent *event) override;

        // NOLINTNEXTLINE
        bool HGE_CALL Gfx_BeginScene(HTARGET target = 0) override;

        void HGE_CALL Gfx_EndScene() override;

        void HGE_CALL Gfx_Clear(hgeColor32 color) override;

        // NOLINTNEXTLINE
        void HGE_CALL Gfx_RenderLine(float x1, float y1, float x2, float y2,
                                     hgeColor32 color, float z = 0.5f) override;

        void HGE_CALL Gfx_RenderTriple(const hgeTriple *triple) override;

        void HGE_CALL Gfx_RenderQuad(const hgeQuad *quad) override;

        hgeVertex *HGE_CALL Gfx_StartBatch(int prim_type, HTEXTURE tex, hgeBlendMode blend,
                                           int *max_prim) override;

        void HGE_CALL Gfx_FinishBatch(int nprim) override;

        // NOLINTNEXTLINE
        void HGE_CALL Gfx_SetClipping(int x = 0, int y = 0, int w = 0, int h = 0) override;

        // NOLINTNEXTLINE
        void HGE_CALL Gfx_SetTransform(float x = 0, float y = 0, float dx = 0, float dy = 0,
                                       float rot = 0, float hscale = 0, float vscale = 0) override;

        HSHADER HGE_CALL Shader_Create(const char *filename) override;

        void HGE_CALL Shader_Free(HSHADER shader) override;

        void HGE_CALL Gfx_SetShader(HSHADER shader) override;

        HTARGET HGE_CALL Target_Create(int width, int height, bool zbuffer) override;

        void HGE_CALL Target_Free(HTARGET target) override;

        HTEXTURE HGE_CALL Target_GetTexture(HTARGET target) override;

        HTEXTURE HGE_CALL Texture_Create(int width, int height) override;

        // NOLINTNEXTLINE
        HTEXTURE HGE_CALL Texture_Load(const char *filename, uint32_t size = 0,
                                       bool bMipmap = false) override;

        void HGE_CALL Texture_Free(HTEXTURE tex) override;

        // NOLINTNEXTLINE
        int HGE_CALL Texture_GetWidth(HTEXTURE tex, bool bOriginal = false) override;

        // NOLINTNEXTLINE
        int HGE_CALL Texture_GetHeight(HTEXTURE tex, bool bOriginal = false) override;

        // NOLINTNEXTLINE
        uint32_t *HGE_CALL Texture_Lock(HTEXTURE tex, bool bReadOnly = true, int left = 0,
                                        int top = 0, int width = 0, int height = 0) override;

        void HGE_CALL Texture_Unlock(HTEXTURE tex) override;

        //-----------------------------------------------
        // Interaction with outer layer in system.cpp
        //-----------------------------------------------
        LRESULT window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        static HGE_Impl *interface_get();

    private:
        //------------------------------------------------------------
        // Implementation
        //------------------------------------------------------------
        void focus_change(bool bAct);

        void post_error(char const *error);

    private:
        HINSTANCE h_instance_;
        HWND hwnd_;

        bool active_;
        std::string error_;

        // Path to our program (GetModuleFilename)
        std::string app_path_;

        // System States
        bool (*proc_frame_func_)();

        bool (*proc_render_func_)();

        bool (*proc_focus_lost_func_)();

        bool (*proc_focus_gain_func_)();

        bool (*proc_gfx_restore_func_)();

        bool (*proc_exit_func_)();

        std::string icon_;
        std::string win_title_;
        int screen_width_;
        int screen_height_;
        int screen_bpp_;
        bool windowed_;
        bool z_buffer_;
        bool texture_filter_;

        // Path to INI file if not empty, otherwise returns default value on INI operations
        std::string ini_file_;

        std::string log_file_;

        bool use_sound_;
        int sample_rate_;
        int fx_volume_;
        int mus_volume_;
        int stream_volume_;
        int hgefps_;
        bool hide_mouse_;
        bool dont_suspend_;
        HWND hwnd_parent_;

#ifdef HGE_SPLASH_ENABLE
        bool splash_screen_enabled_;
#endif

        //-----------------------------------------------
        // Power
        //-----------------------------------------------
        int power_status_;
        HMODULE krnl32_;
        GetSystemPowerStatusFunc get_system_power_status_;

        void init_power_status();

        void update_power_status();

        void done_power_status() const;


        //-----------------------------------------------
        // Graphics
        //-----------------------------------------------
        D3DPRESENT_PARAMETERS *d3dpp_;

        D3DPRESENT_PARAMETERS d3dpp_windowed_;
        RECT rect_windowed_;
        LONG style_windowed_;

        D3DPRESENT_PARAMETERS d3dpp_fullscreen_;
        RECT rect_fullscreen_;
        LONG style_fullscreen_;

        hgeGAPI *d3d_;
        hgeGAPIDevice *d3d_device_;
        hgeGAPIVertexBuffer *vertex_buf_;
        hgeGAPIIndexBuffer *index_buf_;

        hgeGAPISurface *screen_surf_;
        hgeGAPISurface *screen_depth_;
        CRenderTargetList *targets_;
        CRenderTargetList *cur_target_;

        D3DXMATRIX view_matrix_;
        D3DXMATRIX proj_matrix_;

        CTextureList *textures_;
        hgeVertex *vert_array_;

        int n_prim_;
        int cur_prim_type_;
        int cur_blend_mode_;
        HTEXTURE cur_texture_;
        HSHADER cur_shader_;

        bool gfx_init();

        void gfx_done();

        bool gfx_restore();

        void adjust_window();

        void resize(int width, int height);

        bool init_lost();

        void render_batch(bool bEndScene = false);

        static int format_id(D3DFORMAT fmt);

        void set_blend_mode(hgeBlendMode blend);

        void set_projection_matrix(int width, int height);


        //-----------------------------------------------
        // Audio
        //-----------------------------------------------
        HINSTANCE hBass;
        bool is_silent_;
        CStreamList *sound_streams_;

        bool sound_init();

        void sound_done();

        void set_mus_volume(int vol) const;

        void set_stream_volume(int vol) const;

        void set_fx_volume(int vol) const;


        // Input
        int v_key_;
        int input_char_;
        int zpos_;
        float xpos_;
        float ypos_;
        bool mouse_over_;
        bool is_captured_;

        struct KeyState {
            bool pressed: 1;
            bool released: 1;
        };
        KeyState key_table_[256];

//        CInputEventList *ev_queue_;
        std::queue<hgeInputEvent> ev_queue_;

        void update_mouse();

        void input_init();

        void clear_queue();

        void build_event(int type, int key, int scan, int flags, int x, int y);


        //-----------------------------------------------
        // Resources
        //-----------------------------------------------
        std::list<ResourcePackInfo> res_list_;
        HANDLE h_search_;
        WIN32_FIND_DATA search_data_;


        //-----------------------------------------------
        // Timer
        //-----------------------------------------------
        float time_;
        float delta_time_;
        uint32_t fixed_delta_;
        int fps_;
        uint32_t t0_;
        uint32_t t0_fps_;
        uint32_t dt_;
        int cfps_;

    private:
        HGE_Impl();
    };

    extern HGE_Impl *pHGE;

} // namespace
