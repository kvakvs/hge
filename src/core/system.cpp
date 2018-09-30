/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core system functions
*/


#include "hge_impl.h"


#define LOWORDINT(n) ((int)((signed short)(LOWORD(n))))
#define HIWORDINT(n) ((int)((signed short)(HIWORD(n))))


const char* WINDOW_CLASS_NAME = "HGE__WNDCLASS";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


int nRef = 0;
HGE_Impl* pHGE = nullptr;


BOOL APIENTRY DllMain(HANDLE, hgeU32, LPVOID) {
    return TRUE;
}


HGE* HGE_CALL hgeCreate(const int ver) {
    if (ver == HGE_VERSION) {
        return static_cast<HGE*>(HGE_Impl::_Interface_Get());
    }
    return nullptr;
}


HGE_Impl* HGE_Impl::_Interface_Get() {
    if (!pHGE) {
        pHGE = new HGE_Impl();
    }

    nRef++;

    return pHGE;
}


void HGE_CALL HGE_Impl::Release() {
    nRef--;

    if (!nRef) {
        if (pHGE->hwnd) {
            pHGE->System_Shutdown();
        }
        Resource_RemoveAllPacks();
        delete pHGE;
        pHGE = nullptr;
    }
}


bool HGE_CALL HGE_Impl::System_Initiate() {
    OSVERSIONINFO os_ver;
    SYSTEMTIME tm;
    MEMORYSTATUS mem_st;
    WNDCLASS winclass;

    // Log system info

    System_Log("HGE Started..\n");

    System_Log("HGE version: %X.%X", HGE_VERSION >> 8, HGE_VERSION & 0xFF);
    GetLocalTime(&tm);
    System_Log("Date: %02d.%02d.%d, %02d:%02d:%02d\n", tm.wDay, tm.wMonth, tm.wYear, tm.wHour,
               tm.wMinute, tm.wSecond);

    System_Log("Application: %s", szWinTitle);
    os_ver.dwOSVersionInfoSize = sizeof(os_ver);
    GetVersionEx(&os_ver);
    System_Log("OS: Windows %ld.%ld.%ld", os_ver.dwMajorVersion, os_ver.dwMinorVersion,
               os_ver.dwBuildNumber);

    GlobalMemoryStatus(&mem_st);
    System_Log("Memory: %ldK total, %ldK free\n", mem_st.dwTotalPhys / 1024L,
               mem_st.dwAvailPhys / 1024L);


    // Register window class

    winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    winclass.lpfnWndProc = WindowProc;
    winclass.cbClsExtra = 0;
    winclass.cbWndExtra = 0;
    winclass.hInstance = hInstance;
    winclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    winclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    winclass.lpszMenuName = nullptr;
    winclass.lpszClassName = WINDOW_CLASS_NAME;
    if (szIcon) {
        winclass.hIcon = LoadIcon(hInstance, szIcon);
    }
    else {
        winclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    }

    if (!RegisterClass(&winclass)) {
        _PostError("Can't register window class");
        return false;
    }

    // Create window
    // Thanks to Bryan for window rectangle fix in later Windows systems 7,8
    // http://i-am-bryan.com/webs/tutorials/hge/fix-hge-window-sizing-bug/

    // Take approximately center position
    const auto scr_width = GetSystemMetrics(SM_CXSCREEN);
    const auto scr_height = GetSystemMetrics(SM_CYSCREEN);
    rectW.left = (scr_width - (rectW.right - rectW.left)) / 2;
    rectW.top = (scr_height - (rectW.bottom - rectW.top)) / 2;
    rectW.right = rectW.left + nScreenWidth;
    rectW.top = rectW.top + nScreenHeight;

    styleW = WS_BORDER | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | 
        WS_VISIBLE;

    //Fullscreen
    rectFS.left = 0;
    rectFS.top = 0;
    rectFS.right = nScreenWidth;
    rectFS.bottom = nScreenHeight;
    styleFS = WS_POPUP | WS_VISIBLE; //WS_POPUP

    if (hwndParent) {
        rectW.left = 0;
        rectW.top = 0;
        rectW.right = nScreenWidth;
        rectW.bottom = nScreenHeight;
        styleW = WS_CHILD | WS_VISIBLE;
        bWindowed = true;
    }

         // Fix for styled windows in Windows versions newer than XP
    AdjustWindowRect(&rectW, styleW, false);

    if (bWindowed)
        hwnd = CreateWindowEx(0, WINDOW_CLASS_NAME, szWinTitle, styleW,
                              rectW.left, rectW.top, rectW.right - rectW.left,
                              rectW.bottom - rectW.top,
                              hwndParent, nullptr, hInstance, nullptr);
    else
        hwnd = CreateWindowEx(WS_EX_TOPMOST, WINDOW_CLASS_NAME, szWinTitle, styleFS,
                              0, 0, 0, 0,
                              nullptr, nullptr, hInstance, nullptr);
    if (!hwnd) {
        _PostError("Can't create window");
        return false;
    }

    ShowWindow(hwnd, SW_SHOW);

    // Init subsystems

    timeBeginPeriod(1);
    Random_Seed();
    _InitPowerStatus();
    _InputInit();
    if (!_GfxInit()) {
        System_Shutdown();
        return false;
    }
    if (!_SoundInit()) {
        System_Shutdown();
        return false;
    }

    System_Log("Init done.\n");

    fTime = 0.0f;
    t0 = t0fps = timeGetTime();
    dt = cfps = 0;
    nFPS = 0;

    // Show splash

#ifdef HGE_SPLASH_ENABLE

    if (pHGE->bDMO) {
        Sleep(200);
        const auto func = static_cast<bool(*)()>(pHGE->System_GetStateFunc(HGE_FRAMEFUNC));
        const auto rfunc = static_cast<bool(*)()>(pHGE->System_GetStateFunc(HGE_RENDERFUNC));
        const auto hwnd_tmp = hwndParent;
        hwndParent = nullptr;
        pHGE->System_SetStateFunc(HGE_FRAMEFUNC, DFrame);
        pHGE->System_SetStateFunc(HGE_RENDERFUNC, nullptr);
        DInit();
        pHGE->System_Start();
        DDone();
        hwndParent = hwnd_tmp;
        pHGE->System_SetStateFunc(HGE_FRAMEFUNC, func);
        pHGE->System_SetStateFunc(HGE_RENDERFUNC, rfunc);
    }

#endif

    // Done

    return true;
}

void HGE_CALL HGE_Impl::System_Shutdown() {
    System_Log("\nFinishing..");

    timeEndPeriod(1);
    if (hSearch) {
        FindClose(hSearch);
        hSearch = nullptr;
    }
    _ClearQueue();
    _SoundDone();
    _GfxDone();
    _DonePowerStatus();

    if (hwnd) {
        //ShowWindow(hwnd, SW_HIDE);
        //SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
        //ShowWindow(hwnd, SW_SHOW);
        DestroyWindow(hwnd);
        hwnd = nullptr;
    }

    if (hInstance) {
        UnregisterClass(WINDOW_CLASS_NAME, hInstance);
    }

    System_Log("The End.");
}


bool HGE_CALL HGE_Impl::System_Start() {
    MSG msg;

    if (!hwnd) {
        _PostError("System_Start: System_Initiate wasn't called");
        return false;
    }

    if (!procFrameFunc) {
        _PostError("System_Start: No frame function defined");
        return false;
    }

    bActive = true;

    // MAIN LOOP

    for (;;) {

        // Process window messages if not in "child mode"
        // (if in "child mode" the parent application will do this for us)

        if (!hwndParent) {
            if (PeekMessage(&msg, nullptr, 0, 0,PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    break;
                }
                // TranslateMessage(&msg);
                DispatchMessage(&msg);
                continue;
            }
        }

        // Check if mouse is over HGE window for Input_IsMouseOver

        _UpdateMouse();

        // If HGE window is focused or we have the "don't suspend" state - process the main loop

        if (bActive || bDontSuspend) {
            // Ensure we have at least 1ms time step
            // to not confuse user's code with 0

            do {
                dt = timeGetTime() - t0;
            }
            while (dt < 1);

            // If we reached the time for the next frame
            // or we just run in unlimited FPS mode, then
            // do the stuff

            if (dt >= nFixedDelta) {
                // fDeltaTime = time step in seconds returned by Timer_GetDelta

                fDeltaTime = dt / 1000.0f;

                // Cap too large time steps usually caused by lost focus to avoid jerks

                if (fDeltaTime > 0.2f) {
                    fDeltaTime = nFixedDelta ? nFixedDelta / 1000.0f : 0.01f;
                }

                // Update time counter returned Timer_GetTime

                fTime += fDeltaTime;

                // Store current time for the next frame
                // and count FPS

                t0 = timeGetTime();
                if (t0 - t0fps <= 1000) {
                    cfps++;
                }
                else {
                    nFPS = cfps;
                    cfps = 0;
                    t0fps = t0;
                    _UpdatePowerStatus();
                }

                // Do user's stuff

                if (procFrameFunc()) {
                    break;
                }
                if (procRenderFunc) {
                    procRenderFunc();
                }

                // If if "child mode" - return after processing single frame

                if (hwndParent) {
                    break;
                }

                // Clean up input events that were generated by
                // WindowProc and weren't handled by user's code

                _ClearQueue();

                // If we use VSYNC - we could afford a little
                // sleep to lower CPU usage

                // if(!bWindowed && nHGEFPS==HGEFPS_VSYNC) Sleep(1);
            }

                // If we have a fixed frame rate and the time
                // for the next frame isn't too close, sleep a bit

            else {
                if (nFixedDelta && dt + 3 < nFixedDelta) {
                    Sleep(1);
                }
            }
        }

            // If main loop is suspended - just sleep a bit
            // (though not too much to allow instant window
            // redraw if requested by OS)

        else {
            Sleep(1);
        }
    }

    _ClearQueue();

    bActive = false;

    return true;
}

void HGE_CALL HGE_Impl::System_SetStateBool(const hgeBoolState state, const bool value) {
    switch (state) {
    case HGE_WINDOWED:
        if (VertArray || hwndParent) {
            break;
        }
        if (pD3DDevice && bWindowed != value) {
            if (d3dppW.BackBufferFormat == D3DFMT_UNKNOWN 
                || d3dppFS.BackBufferFormat == D3DFMT_UNKNOWN) {
                break;
            }

            if (bWindowed) {
                GetWindowRect(hwnd, &rectW);
            }
            bWindowed = value;
            if (bWindowed) {
                d3dpp = &d3dppW;
            }
            else {
                d3dpp = &d3dppFS;
            }

            if (_format_id(d3dpp->BackBufferFormat) < 4) {
                nScreenBPP = 16;
            }
            else {
                nScreenBPP = 32;
            }

            _GfxRestore();
            _AdjustWindow();
        }
        else {
            bWindowed = value;
        }
        break;

    case HGE_ZBUFFER:
        if (!pD3DDevice) {
            bZBuffer = value;
        }
        break;

    case HGE_TEXTUREFILTER:
        bTextureFilter = value;
        if (pD3DDevice) {
            _render_batch();
            if (bTextureFilter) {
#if HGE_DIRECTX_VER == 8
                pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
                pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
#endif
#if HGE_DIRECTX_VER == 9
                pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
#endif
            }
            else {
#if HGE_DIRECTX_VER == 8
                pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
                pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_POINT);
#endif
#if HGE_DIRECTX_VER == 9
                pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
#endif
            }
        }
        break;

    case HGE_USESOUND:
        if (bUseSound != value) {
            bUseSound = value;
            if (bUseSound && hwnd) {
                _SoundInit();
            }
            if (!bUseSound && hwnd) {
                _SoundDone();
            }
        }
        break;

    case HGE_HIDEMOUSE:
        bHideMouse = value;
        break;

    case HGE_DONTSUSPEND:
        bDontSuspend = value;
        break;

#ifdef HGE_SPLASH_ENABLE
    case HGE_SHOWSPLASH:
        bDMO = value;
        break;
#endif
    }
}

void HGE_CALL HGE_Impl::System_SetStateFunc(const hgeFuncState state,
                                            const hgeCallback value) {
    switch (state) {
    case HGE_FRAMEFUNC:
        procFrameFunc = value;
        break;
    case HGE_RENDERFUNC:
        procRenderFunc = value;
        break;
    case HGE_FOCUSLOSTFUNC:
        procFocusLostFunc = value;
        break;
    case HGE_FOCUSGAINFUNC:
        procFocusGainFunc = value;
        break;
    case HGE_GFXRESTOREFUNC:
        procGfxRestoreFunc = value;
        break;
    case HGE_EXITFUNC:
        procExitFunc = value;
        break;
    }
}

void HGE_CALL HGE_Impl::System_SetStateHwnd(const hgeHwndState state,
                                            const HWND value) {
    switch (state) {
    case HGE_HWNDPARENT:
        if (!hwnd) {
            hwndParent = value;
        }
        break;
    }
}

void HGE_CALL HGE_Impl::System_SetStateInt(const hgeIntState state, const int value) {
    switch (state) {
    case HGE_SCREENWIDTH:
        if (!pD3DDevice) {
            nScreenWidth = value;
        }
        break;

    case HGE_SCREENHEIGHT:
        if (!pD3DDevice) {
            nScreenHeight = value;
        }
        break;

    case HGE_SCREENBPP:
        if (!pD3DDevice) {
            nScreenBPP = value;
        }
        break;

    case HGE_SAMPLERATE:
        if (!hBass) {
            nSampleRate = value;
        }
        break;

    case HGE_FXVOLUME:
        nFXVolume = value;
        _SetFXVolume(nFXVolume);
        break;

    case HGE_MUSVOLUME:
        nMusVolume = value;
        _SetMusVolume(nMusVolume);
        break;

    case HGE_STREAMVOLUME:
        nStreamVolume = value;
        _SetStreamVolume(nStreamVolume);
        break;

    case HGE_FPS:
        if (VertArray) {
            break;
        }

        if (pD3DDevice) {
            if ((nHGEFPS >= 0 && value < 0) || (nHGEFPS < 0 && value >= 0)) {
                if (value == HGEFPS_VSYNC) {
#if HGE_DIRECTX_VER == 8
                    d3dppW.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
                    d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
#endif
#if HGE_DIRECTX_VER == 9
                    d3dppW.SwapEffect = D3DSWAPEFFECT_COPY;
                    d3dppW.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
                    d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
#endif
                }
                else {
                    d3dppW.SwapEffect = D3DSWAPEFFECT_COPY;
#if HGE_DIRECTX_VER == 8
                    d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif
#if HGE_DIRECTX_VER == 9
                    d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif
                }
                //if(procFocusLostFunc) procFocusLostFunc();
                _GfxRestore();
                //if(procFocusGainFunc) procFocusGainFunc();
            }
        }
        nHGEFPS = value;
        if (nHGEFPS > 0) {
            nFixedDelta = int(1000.0f / value);
        }
        else {
            nFixedDelta = 0;
        }
        break;
    }
}

void HGE_CALL HGE_Impl::System_SetStateString(const hgeStringState state, const char* value) {
    switch (state) {
    case HGE_ICON:
        szIcon = value;
        if (pHGE->hwnd) {
            SetClassLong(pHGE->hwnd, GCL_HICON,
                         reinterpret_cast<LONG>(LoadIcon(pHGE->hInstance, szIcon)));
        }
        break;
    case HGE_TITLE:
        strcpy(szWinTitle, value);
        if (pHGE->hwnd) {
            SetWindowText(pHGE->hwnd, szWinTitle);
        }
        break;
    case HGE_INIFILE:
        if (value) {
            strcpy(szIniFile, Resource_MakePath(value));
        }
        else {
            szIniFile[0] = 0;
        }
        break;
    case HGE_LOGFILE:
        if (value) {
            strcpy(szLogFile, Resource_MakePath(value));
            FILE* hf = fopen(szLogFile, "w");
            if (!hf) {
                szLogFile[0] = 0;
            }
            else {
                fclose(hf);
            }
        }
        else {
            szLogFile[0] = 0;
        }
        break;
    }
}

bool HGE_CALL HGE_Impl::System_GetStateBool(const hgeBoolState state) {
    switch (state) {
    case HGE_WINDOWED:
        return bWindowed;
    case HGE_ZBUFFER:
        return bZBuffer;
    case HGE_TEXTUREFILTER:
        return bTextureFilter;
    case HGE_USESOUND:
        return bUseSound;
    case HGE_DONTSUSPEND:
        return bDontSuspend;
    case HGE_HIDEMOUSE:
        return bHideMouse;

#ifdef HGE_SPLASH_ENABLE
    case HGE_SHOWSPLASH:
        return bDMO;
#endif
    }

    return false;
}

hgeCallback HGE_CALL HGE_Impl::System_GetStateFunc(const hgeFuncState state) {
    switch (state) {
    case HGE_FRAMEFUNC:
        return procFrameFunc;
    case HGE_RENDERFUNC:
        return procRenderFunc;
    case HGE_FOCUSLOSTFUNC:
        return procFocusLostFunc;
    case HGE_FOCUSGAINFUNC:
        return procFocusGainFunc;
    case HGE_EXITFUNC:
        return procExitFunc;
    }

    return nullptr;
}

HWND HGE_CALL HGE_Impl::System_GetStateHwnd(const hgeHwndState state) {
    switch (state) {
    case HGE_HWND:
        return hwnd;
    case HGE_HWNDPARENT:
        return hwndParent;
    }

    return nullptr;
}

int HGE_CALL HGE_Impl::System_GetStateInt(const hgeIntState state) {
    switch (state) {
    case HGE_SCREENWIDTH:
        return nScreenWidth;
    case HGE_SCREENHEIGHT:
        return nScreenHeight;
    case HGE_SCREENBPP:
        return nScreenBPP;
    case HGE_SAMPLERATE:
        return nSampleRate;
    case HGE_FXVOLUME:
        return nFXVolume;
    case HGE_MUSVOLUME:
        return nMusVolume;
    case HGE_STREAMVOLUME:
        return nStreamVolume;
    case HGE_FPS:
        return nHGEFPS;
    case HGE_POWERSTATUS:
        return nPowerStatus;
    }

    return 0;
}

const char* HGE_CALL HGE_Impl::System_GetStateString(const hgeStringState state) {
    switch (state) {
    case HGE_ICON:
        return szIcon;
    case HGE_TITLE:
        return szWinTitle;
    case HGE_INIFILE: {
        if (szIniFile[0]) {
            return szIniFile;
        }
        return nullptr;
    }
    case HGE_LOGFILE: {
        if (szLogFile[0]) {
            return szLogFile;
        }
        return nullptr;
    }
    }

    return nullptr;
}

char* HGE_CALL HGE_Impl::System_GetErrorMessage() {
    return szError;
}

void HGE_CALL HGE_Impl::System_Log(const char* szFormat, ...) {
    va_list ap;

    if (!szLogFile[0]) {
        return;
    }

    FILE* hf = fopen(szLogFile, "a");
    if (!hf) {
        return;
    }

    va_start(ap, szFormat);
    vfprintf(hf, szFormat, ap);
    va_end(ap);

    fprintf(hf, "\n");

    fclose(hf);
}

bool HGE_CALL HGE_Impl::System_Launch(const char* url) {
    const auto shell_execute = reinterpret_cast<hgeU32>(
        ShellExecute(pHGE->hwnd, nullptr, url, nullptr, nullptr, SW_SHOWMAXIMIZED)
    );
    if (shell_execute > 32) {
        return true;
    }
    return false;
}

void HGE_CALL HGE_Impl::System_Snapshot(const char* filename) {
    hgeGAPISurface* pSurf;
    char tempname[_MAX_PATH];

    if (!filename) {
        int i = 0;
        char* shotname = Resource_EnumFiles("shot???.bmp");
        while (shotname) {
            i++;
            shotname = Resource_EnumFiles();
        }
        sprintf(tempname, "shot%03d.bmp", i);
        filename = Resource_MakePath(tempname);
    }

    if (pD3DDevice) {
#if HGE_DIRECTX_VER == 8
        pD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pSurf);
#endif
#if HGE_DIRECTX_VER == 9
        pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurf);
#endif
        D3DXSaveSurfaceToFile(filename, D3DXIFF_BMP, pSurf, nullptr, nullptr);
        pSurf->Release();
    }
}

//////// Implementation ////////


HGE_Impl::HGE_Impl()
    : styleW(0), styleFS(0), nPrim(0), CurPrimType(0), CurBlendMode(0), CurTexture(0),
      CurShader(0), t0(0), t0fps(0), dt(0), cfps(0) {
    hInstance = GetModuleHandle(nullptr);
    hwnd = nullptr;
    bActive = false;
    szError[0] = 0;

    pD3D = nullptr;
    pD3DDevice = nullptr;
    d3dpp = nullptr;
    pTargets = nullptr;
    pCurTarget = nullptr;
    pScreenSurf = nullptr;
    pScreenDepth = nullptr;
    pVB = nullptr;
    pIB = nullptr;
    VertArray = nullptr;
    textures = nullptr;

    hBass = nullptr;
    bSilent = false;
    streams = nullptr;

    hSearch = nullptr;
    res = nullptr;

    queue = nullptr;
    Char = VKey = Zpos = 0;
    Xpos = Ypos = 0.0f;
    bMouseOver = false;
    bCaptured = false;

    nHGEFPS = HGEFPS_UNLIMITED;
    fTime = 0.0f;
    fDeltaTime = 0.0f;
    nFPS = 0;

    procFrameFunc = nullptr;
    procRenderFunc = nullptr;
    procFocusLostFunc = nullptr;
    procFocusGainFunc = nullptr;
    procGfxRestoreFunc = nullptr;
    procExitFunc = nullptr;
    szIcon = nullptr;
    strcpy(szWinTitle, "HGE");
    nScreenWidth = 800;
    nScreenHeight = 600;
    nScreenBPP = 32;
    bWindowed = false;
    bZBuffer = false;
    bTextureFilter = true;
    szLogFile[0] = 0;
    szIniFile[0] = 0;
    bUseSound = true;
    nSampleRate = 44100;
    nFXVolume = 100;
    nMusVolume = 100;
    nStreamVolume = 100;
    nFixedDelta = 0;
    bHideMouse = true;
    bDontSuspend = false;
    hwndParent = nullptr;

    nPowerStatus = HGEPWR_UNSUPPORTED;
    hKrnl32 = nullptr;
    lpfnGetSystemPowerStatus = nullptr;

#ifdef HGE_SPLASH_ENABLE
    bDMO = true;
#endif


    GetModuleFileName(GetModuleHandle(nullptr), szAppPath, sizeof(szAppPath));
    int i;
    for (i = strlen(szAppPath) - 1; i > 0; i--)
        if (szAppPath[i] == '\\') {
            break;
        }
    szAppPath[i + 1] = 0;
}

void HGE_Impl::_PostError(char* error) {
    System_Log(error);
    strcpy(szError, error);
}

void HGE_Impl::_FocusChange(const bool b_act) {
    bActive = b_act;

    if (bActive) {
        if (procFocusGainFunc) {
            procFocusGainFunc();
        }
    }
    else {
        if (procFocusLostFunc) {
            procFocusLostFunc();
        }
    }
}

LRESULT CALLBACK WindowProc(const HWND hwnd, const UINT msg, WPARAM wparam, LPARAM lparam) {
    bool b_activating = false;
    switch (msg) {
    case WM_CREATE:
        return FALSE;

    case WM_PAINT:
        if (pHGE->pD3D && pHGE->procRenderFunc && pHGE->bWindowed) {
            pHGE->procRenderFunc();
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return FALSE;

        /*
                case WM_ACTIVATEAPP:
                    bActivating = (wparam == TRUE);
                    if(pHGE->pD3D && pHGE->bActive != bActivating) pHGE->_FocusChange(bActivating);
                    return FALSE;
        */
    case WM_ACTIVATE:
        // tricky: we should catch WA_ACTIVE and WA_CLICKACTIVE,
        // but only if HIWORD(wParam) (fMinimized) == FALSE (0)
        b_activating = (LOWORD(wparam) != WA_INACTIVE) && (HIWORD(wparam) == 0);
        if (pHGE->pD3D && pHGE->bActive != b_activating) {
            pHGE->_FocusChange(b_activating);
        }
        return FALSE;


    case WM_SETCURSOR:
        if (pHGE->bActive && LOWORD(lparam) == HTCLIENT && pHGE->bHideMouse) {
            SetCursor(nullptr);
        }
        else {
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
        }
        return FALSE;

    case WM_SYSKEYDOWN: {
        if (wparam == VK_F4) {
            if (pHGE->procExitFunc && !pHGE->procExitFunc()) {
                return FALSE;
            }
            return DefWindowProc(hwnd, msg, wparam, lparam);
        }
        if (wparam == VK_RETURN) {
            pHGE->System_SetState(HGE_WINDOWED, !pHGE->System_GetState(HGE_WINDOWED));
            return FALSE;
        }
        pHGE->_BuildEvent(INPUT_KEYDOWN, wparam, HIWORD(lparam) & 0xFF,
                          (lparam & 0x40000000) ? HGEINP_REPEAT : 0, -1, -1);
        return FALSE;
    }

    case WM_KEYDOWN:
        pHGE->_BuildEvent(INPUT_KEYDOWN, wparam, HIWORD(lparam) & 0xFF,
                          (lparam & 0x40000000) ? HGEINP_REPEAT : 0, -1, -1);
        return FALSE;
    case WM_SYSKEYUP:
        pHGE->_BuildEvent(INPUT_KEYUP, wparam, HIWORD(lparam) & 0xFF, 0, -1, -1);
        return FALSE;
    case WM_KEYUP:
        pHGE->_BuildEvent(INPUT_KEYUP, wparam, HIWORD(lparam) & 0xFF, 0, -1, -1);
        return FALSE;

    case WM_LBUTTONDOWN:
        SetFocus(hwnd);
        pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_LBUTTON, 0, 0, LOWORDINT(lparam),
                          HIWORDINT(lparam));
        return FALSE;
    case WM_MBUTTONDOWN:
        SetFocus(hwnd);
        pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_MBUTTON, 0, 0, LOWORDINT(lparam),
                          HIWORDINT(lparam));
        return FALSE;
    case WM_RBUTTONDOWN:
        SetFocus(hwnd);
        pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_RBUTTON, 0, 0, LOWORDINT(lparam),
                          HIWORDINT(lparam));
        return FALSE;

    case WM_LBUTTONDBLCLK:
        pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_LBUTTON, 0, HGEINP_REPEAT, LOWORDINT(lparam),
                          HIWORDINT(lparam));
        return FALSE;
    case WM_MBUTTONDBLCLK:
        pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_MBUTTON, 0, HGEINP_REPEAT, LOWORDINT(lparam),
                          HIWORDINT(lparam));
        return FALSE;
    case WM_RBUTTONDBLCLK:
        pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_RBUTTON, 0, HGEINP_REPEAT, LOWORDINT(lparam),
                          HIWORDINT(lparam));
        return FALSE;

    case WM_LBUTTONUP:
        pHGE->_BuildEvent(INPUT_MBUTTONUP, HGEK_LBUTTON, 0, 0, LOWORDINT(lparam),
                          HIWORDINT(lparam));
        return FALSE;
    case WM_MBUTTONUP:
        pHGE->_BuildEvent(INPUT_MBUTTONUP, HGEK_MBUTTON, 0, 0, LOWORDINT(lparam),
                          HIWORDINT(lparam));
        return FALSE;
    case WM_RBUTTONUP:
        pHGE->_BuildEvent(INPUT_MBUTTONUP, HGEK_RBUTTON, 0, 0, LOWORDINT(lparam),
                          HIWORDINT(lparam));
        return FALSE;

    case WM_MOUSEMOVE:
        pHGE->_BuildEvent(INPUT_MOUSEMOVE, 0, 0, 0, LOWORDINT(lparam), HIWORDINT(lparam));
        return FALSE;
    case 0x020A: // WM_MOUSEWHEEL, GET_WHEEL_DELTA_WPARAM(wparam);
        pHGE->_BuildEvent(INPUT_MOUSEWHEEL, short(HIWORD(wparam)) / 120, 0, 0, LOWORDINT(lparam),
                          HIWORDINT(lparam));
        return FALSE;

    case WM_SIZE:
        if (pHGE->pD3D && wparam == SIZE_RESTORED) {
            pHGE->_Resize(LOWORD(lparam), HIWORD(lparam));
        }
        //return FALSE;
        break;

    case WM_SYSCOMMAND:
        if (wparam == SC_CLOSE) {
            if (pHGE->procExitFunc && !pHGE->procExitFunc()) {
                return FALSE;
            }
            pHGE->bActive = false;
            return DefWindowProc(hwnd, msg, wparam, lparam);
        }
        break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}
