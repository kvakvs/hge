/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once

#include <windows.h>

#define HGE_VERSION 0x190

#include <impl/hgedefs.h>
#include <impl/hgehandle.h>
#include <impl/hgekeys.h>
#include <impl/hgecolor.h>

/*
** HGE System state constants
*/
enum hgeBoolState : int32_t {
    HGE_WINDOWED = 1,
    // bool     run in window?      (default: false)
    HGE_ZBUFFER = 2,
    // bool     use z-buffer?       (default: false)
    HGE_TEXTUREFILTER = 3,
    // bool     texture filtering?  (default: true)

    HGE_USESOUND = 4,
    // bool     use BASS for sound? (default: true)

    HGE_DONTSUSPEND = 5,
    // bool     focus lost:suspend? (default: false)
    HGE_HIDEMOUSE = 6,
    // bool     hide system cursor? (default: true)

    HGE_SHOWSPLASH = 7,
    // bool     hide system cursor? (default: true)
};

enum hgeFuncState : int32_t {
    HGE_FRAMEFUNC = 8,
    // bool*()  frame function      (default: NULL) (you MUST set this)
    HGE_RENDERFUNC = 9,
    // bool*()  render function     (default: NULL)
    HGE_FOCUSLOSTFUNC = 10,
    // bool*()  focus lost function (default: NULL)
    HGE_FOCUSGAINFUNC = 11,
    // bool*()  focus gain function (default: NULL)
    HGE_GFXRESTOREFUNC = 12,
    // bool*()  exit function       (default: NULL)
    HGE_EXITFUNC = 13,
    // bool*()  exit function       (default: NULL)
};

enum hgeHwndState : int32_t {
    HGE_HWND = 15,
    // int      window handle: read only
    HGE_HWNDPARENT = 16,
    // int      parent win handle   (default: 0)
};

enum hgeIntState : int32_t {
    HGE_SCREENWIDTH = 17,
    // int      screen width        (default: 800)
    HGE_SCREENHEIGHT = 18,
    // int      screen height       (default: 600)
    HGE_SCREENBPP = 19,
    // int      screen bitdepth     (default: 32) (desktop bpp in windowed mode)

    HGE_SAMPLERATE = 20,
    // int      sample rate         (default: 44100)
    HGE_FXVOLUME = 21,
    // int      global fx volume    (default: 100)
    HGE_MUSVOLUME = 22,
    // int      global music volume (default: 100)
    HGE_STREAMVOLUME = 23,
    // int      global music volume (default: 100)

    HGE_FPS = 24,
    // int      fixed fps           (default: HGEFPS_UNLIMITED)

    HGE_POWERSTATUS = 25,
    // int      battery life percent + status
};

enum hgeStringState : int32_t {
    HGE_ICON = 26,
    // char*    icon resource       (default: NULL)
    HGE_TITLE = 27,
    // char*    window title        (default: "HGE")

    HGE_INIFILE = 28,
    // char*    ini file            (default: NULL) (meaning no file)
    HGE_LOGFILE = 29,
    // char*    log file            (default: NULL) (meaning no file)
};

//
// Callback protoype used by HGE
//
typedef bool (*hgeCallback)();


//
// HGE_FPS system state special constants
//
enum {
    HGEFPS_UNLIMITED = 0,
    HGEFPS_VSYNC = -1,
};


//
// HGE_POWERSTATUS system state special constants
//
enum {
    HGEPWR_AC = -1,
    HGEPWR_UNSUPPORTED = -2,
};


//
// HGE Primitive type constants
//
enum {
    HGEPRIM_LINES = 2,
    HGEPRIM_TRIPLES = 3,
    HGEPRIM_QUADS = 4,
};


//
// HGE Vertex structure
//
struct hgeVertex {
    float x, y; // screen position
    float z; // Z-buffer depth 0..1
    uint32_t col; // color
    float tx, ty; // texture coordinates
};


//
// HGE Triple structure, represents a textured triangle
//
struct hgeTriple {
    hgeVertex v[3];
    HTEXTURE tex;
    hgeBlendMode blend;
};


//
// HGE Quad structure
//
struct hgeQuad {
    hgeVertex v[4];
    HTEXTURE tex;
    hgeBlendMode blend;
};


//
// HGE Interface class
// Using PImpl pattern HGE class hides internal implementation detail from the user program.
// The actual implementation is in hge_impl.h and not exported from the DLL.
//
class HGE {
public:
    virtual ~HGE() = default;

    virtual void HGE_CALL Release() = 0;

    virtual bool HGE_CALL System_Initiate() = 0;

    virtual void HGE_CALL System_Shutdown() = 0;

    virtual bool HGE_CALL System_Start() = 0;

    virtual const char *HGE_CALL System_GetErrorMessage() const = 0;

    // NOLINTNEXTLINE
    virtual void HGE_CALL System_Log(const char *format, ...) = 0;

    virtual bool HGE_CALL System_Launch(const char *url) = 0;

    virtual void HGE_CALL System_Snapshot(const char *filename = nullptr) = 0;

private:
    virtual void HGE_CALL System_SetStateBool(hgeBoolState state, bool value) = 0;

    virtual void HGE_CALL System_SetStateFunc(hgeFuncState state, hgeCallback value) = 0;

    virtual void HGE_CALL System_SetStateHwnd(hgeHwndState state, HWND value) = 0;

    virtual void HGE_CALL System_SetStateInt(hgeIntState state, int value) = 0;

    virtual void HGE_CALL System_SetStateString(hgeStringState state, const char *value) = 0;

    virtual bool HGE_CALL System_GetStateBool(hgeBoolState state) = 0;

    virtual hgeCallback HGE_CALL System_GetStateFunc(hgeFuncState state) = 0;

    virtual HWND HGE_CALL System_GetStateHwnd(hgeHwndState state) = 0;

    virtual int HGE_CALL System_GetStateInt(hgeIntState state) = 0;

    virtual const char *HGE_CALL System_GetStateString(hgeStringState state) = 0;

public:
    void System_SetState(const hgeBoolState state, const bool value) {
      System_SetStateBool(state, value);
    }

    void System_SetState(const hgeFuncState state, const hgeCallback value) {
      System_SetStateFunc(state, value);
    }

    void System_SetState(const hgeHwndState state, HWND value) {
      System_SetStateHwnd(state, value);
    }

    void System_SetState(const hgeIntState state, const int value) {
      System_SetStateInt(state, value);
    }

    void System_SetState(const hgeStringState state, const char *value) {
      System_SetStateString(state, value);
    }

    bool System_GetState(const hgeBoolState state) {
      return System_GetStateBool(state);
    }

    hgeCallback System_GetState(const hgeFuncState state) {
      return System_GetStateFunc(state);
    }

    HWND System_GetState(const hgeHwndState state) {
      return System_GetStateHwnd(state);
    }

    int System_GetState(const hgeIntState state) {
      return System_GetStateInt(state);
    }

    const char *System_GetState(const hgeStringState state) {
      return System_GetStateString(state);
    }

    virtual void *HGE_CALL Resource_Load(const char *filename,
                                         uint32_t *size = nullptr) = 0;

    virtual void HGE_CALL Resource_Free(void *res) = 0;

    virtual bool HGE_CALL Resource_AttachPack(const char *filename,
                                              const char *password = nullptr) = 0;

    virtual void HGE_CALL Resource_RemovePack(const char *filename) = 0;

    virtual void HGE_CALL Resource_RemoveAllPacks() = 0;

    virtual const char *HGE_CALL Resource_MakePath(const char *filename = nullptr) = 0;

    virtual const char *HGE_CALL Resource_EnumFiles(const char *wildcard = nullptr) = 0;

    virtual const char *HGE_CALL Resource_EnumFolders(const char *wildcard = nullptr) = 0;

    virtual void HGE_CALL Ini_SetInt(const char *section, const char *name,
                                     int value) = 0;

    virtual int HGE_CALL Ini_GetInt(const char *section, const char *name,
                                    int def_val) = 0;

    virtual void HGE_CALL Ini_SetFloat(const char *section, const char *name,
                                       float value) = 0;

    virtual float HGE_CALL Ini_GetFloat(const char *section, const char *name,
                                        float def_val) = 0;

    virtual void HGE_CALL Ini_SetString(const char *section, const char *name,
                                        const char *value) = 0;

    virtual const char *HGE_CALL Ini_GetString(const char *section, const char *name,
                                               const char *def_val) = 0;

    virtual void HGE_CALL Random_Seed(int seed = 0) = 0;

    virtual int HGE_CALL Random_Int(int min, int max) = 0;

    virtual float HGE_CALL Random_Float(float min, float max) = 0;

    virtual float HGE_CALL Timer_GetTime() = 0;

    virtual float HGE_CALL Timer_GetDelta() = 0;

    virtual int HGE_CALL Timer_GetFPS() = 0;

    virtual HEFFECT HGE_CALL Effect_Load(const char *filename, uint32_t size = 0) = 0;

    virtual void HGE_CALL Effect_Free(HEFFECT eff) = 0;

    virtual HCHANNEL HGE_CALL Effect_Play(HEFFECT eff) = 0;

    virtual HCHANNEL HGE_CALL Effect_PlayEx(HEFFECT eff, int volume = 100, int pan = 0,
                                            float pitch = 1.0f, bool loop = false) = 0;

    virtual HMUSIC HGE_CALL Music_Load(const char *filename, uint32_t size = 0) = 0;

    virtual void HGE_CALL Music_Free(HMUSIC mus) = 0;

    virtual HCHANNEL HGE_CALL Music_Play(HMUSIC mus, bool loop, int volume = 100, int order = -1,
                                         int row = -1) = 0;

    virtual void HGE_CALL Music_SetAmplification(HMUSIC music, int ampl) = 0;

    virtual int HGE_CALL Music_GetAmplification(HMUSIC music) = 0;

    virtual int HGE_CALL Music_GetLength(HMUSIC music) = 0;

    virtual void HGE_CALL Music_SetPos(HMUSIC music, int order, int row) = 0;

    virtual bool HGE_CALL Music_GetPos(HMUSIC music, int *order, int *row) = 0;

    virtual void HGE_CALL Music_SetInstrVolume(HMUSIC music, int instr, int volume) = 0;

    virtual int HGE_CALL Music_GetInstrVolume(HMUSIC music, int instr) = 0;

    virtual void HGE_CALL Music_SetChannelVolume(HMUSIC music, int channel, int volume) = 0;

    virtual int HGE_CALL Music_GetChannelVolume(HMUSIC music, int channel) = 0;

    virtual HSTREAM HGE_CALL Stream_Load(const char *filename, uint32_t size = 0) = 0;

    virtual void HGE_CALL Stream_Free(HSTREAM stream) = 0;

    virtual HCHANNEL HGE_CALL Stream_Play(HSTREAM stream, bool loop, int volume = 100) = 0;

    virtual void HGE_CALL Channel_SetPanning(HCHANNEL chn, int pan) = 0;

    virtual void HGE_CALL Channel_SetVolume(HCHANNEL chn, int volume) = 0;

    virtual void HGE_CALL Channel_SetPitch(HCHANNEL chn, float pitch) = 0;

    virtual void HGE_CALL Channel_Pause(HCHANNEL chn) = 0;

    virtual void HGE_CALL Channel_Resume(HCHANNEL chn) = 0;

    virtual void HGE_CALL Channel_Stop(HCHANNEL chn) = 0;

    virtual void HGE_CALL Channel_PauseAll() = 0;

    virtual void HGE_CALL Channel_ResumeAll() = 0;

    virtual void HGE_CALL Channel_StopAll() = 0;

    virtual bool HGE_CALL Channel_IsPlaying(HCHANNEL chn) = 0;

    virtual float HGE_CALL Channel_GetLength(HCHANNEL chn) = 0;

    virtual float HGE_CALL Channel_GetPos(HCHANNEL chn) = 0;

    virtual void HGE_CALL Channel_SetPos(HCHANNEL chn, float fSeconds) = 0;

    virtual void HGE_CALL Channel_SlideTo(HCHANNEL channel, float time,
                                          int volume, int pan = -101,
                                          float pitch = -1) = 0;

    virtual bool HGE_CALL Channel_IsSliding(HCHANNEL channel) = 0;

    virtual void HGE_CALL Input_GetMousePos(float *x, float *y) = 0;

    virtual void HGE_CALL Input_SetMousePos(float x, float y) = 0;

    virtual int HGE_CALL Input_GetMouseWheel() = 0;

    virtual bool HGE_CALL Input_IsMouseOver() = 0;

    virtual bool HGE_CALL Input_KeyDown(int key) = 0;

    virtual bool HGE_CALL Input_KeyUp(int key) = 0;

    virtual bool HGE_CALL Input_GetKeyState(int key) = 0;

    virtual const char *HGE_CALL Input_GetKeyName(int key) = 0;

    virtual int HGE_CALL Input_GetKey() = 0;

    virtual int HGE_CALL Input_GetChar() = 0;

    virtual bool HGE_CALL Input_GetEvent(hgeInputEvent *event) = 0;

    virtual bool HGE_CALL Gfx_BeginScene(HTARGET target = 0) = 0;

    virtual void HGE_CALL Gfx_EndScene() = 0;

    virtual void HGE_CALL Gfx_Clear(hgeColor32 color) = 0;

    virtual void HGE_CALL Gfx_RenderLine(float x1, float y1, float x2, float y2,
                                         hgeColor32 color, float z = 0.5f) = 0;

    virtual void HGE_CALL Gfx_RenderTriple(const hgeTriple *triple) = 0;

    virtual void HGE_CALL Gfx_RenderQuad(const hgeQuad *quad) = 0;

    virtual hgeVertex *HGE_CALL Gfx_StartBatch(int prim_type, HTEXTURE tex, hgeBlendMode blend,
                                               int *max_prim) = 0;

    virtual void HGE_CALL Gfx_FinishBatch(int nprim) = 0;

    virtual void HGE_CALL Gfx_SetClipping(int x = 0, int y = 0, int w = 0, int h = 0) = 0;

    virtual void HGE_CALL Gfx_SetTransform(float x = 0, float y = 0, float dx = 0, float dy = 0,
                                           float rot = 0, float hscale = 0, float vscale = 0) = 0;

    virtual HSHADER HGE_CALL Shader_Create(const char *filename) = 0;

    virtual void HGE_CALL Shader_Free(HSHADER shader) = 0;

    virtual void HGE_CALL Gfx_SetShader(HSHADER shader) = 0;

    virtual HTARGET HGE_CALL Target_Create(int width, int height, bool zbuffer) = 0;

    virtual void HGE_CALL Target_Free(HTARGET target) = 0;

    virtual HTEXTURE HGE_CALL Target_GetTexture(HTARGET target) = 0;

    virtual HTEXTURE HGE_CALL Texture_Create(int width, int height) = 0;

    virtual HTEXTURE HGE_CALL Texture_Load(const char *filename, uint32_t size = 0,
                                           bool bMipmap = false) = 0;

    virtual void HGE_CALL Texture_Free(HTEXTURE tex) = 0;

    virtual int HGE_CALL Texture_GetWidth(HTEXTURE tex, bool bOriginal = false) = 0;

    virtual int HGE_CALL Texture_GetHeight(HTEXTURE tex, bool bOriginal = false) = 0;

    virtual uint32_t *HGE_CALL Texture_Lock(HTEXTURE tex, bool bReadOnly = true, int left = 0,
                                            int top = 0, int width = 0, int height = 0) = 0;

    virtual void HGE_CALL Texture_Unlock(HTEXTURE tex) = 0;
};

extern "C" {
HGE_EXPORT HGE *HGE_CALL hgeCreate(int ver);
}

// Used to mark mutable/out parameters, declares intent to write to that variable
#define hgeMUTABLE /* mutable */
#define hgeOUT /* mutable, used to return data into a pointer */
