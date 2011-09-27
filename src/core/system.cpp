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


hgeConstString WINDOW_CLASS_NAME = TXT("HGE__WNDCLASS");

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int nRef = 0;
HGE_Impl* pHGE = 0;



BOOL APIENTRY DllMain(HANDLE, uint32_t, LPVOID)
{
	return TRUE;
}


HGE* HGE_CALL hgeCreate(int ver)
{
	if (ver == HGE_VERSION)
		return (HGE*) HGE_Impl::_Interface_Get();
	else
		return 0;
}



HGE_Impl* HGE_Impl::_Interface_Get()
{
	if (!pHGE)
		pHGE = new HGE_Impl();

	nRef++;

	return pHGE;
}



void HGE_CALL HGE_Impl::Release()
{
	nRef--;

	if (!nRef)
	{
		if (pHGE->m_hwnd)
			pHGE->System_Shutdown();
		Resource_RemoveAllPacks();
		delete pHGE;
		pHGE = 0;
	}
}



bool HGE_CALL HGE_Impl::System_Initiate()
{
	OSVERSIONINFO os_ver;
	SYSTEMTIME tm;
	MEMORYSTATUS mem_st;
	HGE_WINAPI_UNICODE_SUFFIX(WNDCLASS) winclass;
	int width, height;

	// Log system info

	System_Log(TXT("HGE Started..\n"));

	System_Log(TXT("HGE version: %X.%X"), HGE_VERSION >> 8, HGE_VERSION & 0xFF);
	GetLocalTime(&tm);
	System_Log(TXT("Date: %02d.%02d.%d, %02d:%02d:%02d\n"), tm.wDay, tm.wMonth,
			tm.wYear, tm.wHour, tm.wMinute, tm.wSecond);

	System_Log(TXT("Application: %s"), m_window_title);
	os_ver.dwOSVersionInfoSize = sizeof(os_ver);
	GetVersionEx(&os_ver);
	System_Log(TXT("OS: Windows %ld.%ld.%ld"), os_ver.dwMajorVersion,
			os_ver.dwMinorVersion, os_ver.dwBuildNumber);

	GlobalMemoryStatus(&mem_st);
	System_Log(TXT("Memory: %ldK total, %ldK free\n"), mem_st.dwTotalPhys
			/ 1024L, mem_st.dwAvailPhys / 1024L);

	// Register window class

	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = m_hinstance;
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;

	if (m_window_icon)
		winclass.hIcon = HGE_WINAPI_UNICODE_SUFFIX(LoadIcon)(m_hinstance, m_window_icon);
	else
		winclass.hIcon = LoadIconA(NULL, IDI_APPLICATION);

	if (!HGE_WINAPI_UNICODE_SUFFIX(RegisterClass)(&winclass))
	{
		_PostError(TXT("Can't register window class"));
		return false;
	}

	// Create window

	width = m_screen_width + GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
	height = m_screen_height + GetSystemMetrics(SM_CYFIXEDFRAME) * 2
			+ GetSystemMetrics(SM_CYCAPTION);

	m_rect_wnd.left = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	m_rect_wnd.top = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	m_rect_wnd.right = m_rect_wnd.left + width;
	m_rect_wnd.bottom = m_rect_wnd.top + height;
	m_style_wnd = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE; //WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;

	m_rect_fullscreen.left = 0;
	m_rect_fullscreen.top = 0;
	m_rect_fullscreen.right = m_screen_width;
	m_rect_fullscreen.bottom = m_screen_height;
	m_style_fullscreen = WS_POPUP | WS_VISIBLE; //WS_POPUP

	if (m_parent_hwnd)
	{
		m_rect_wnd.left = 0;
		m_rect_wnd.top = 0;
		m_rect_wnd.right = m_screen_width;
		m_rect_wnd.bottom = m_screen_height;
		m_style_wnd = WS_CHILD | WS_VISIBLE;
		m_windowed_flag = true;
	}

	if (m_windowed_flag)
		m_hwnd = HGE_WINAPI_UNICODE_SUFFIX(CreateWindowEx)(0, WINDOW_CLASS_NAME,
				m_window_title, m_style_wnd, m_rect_wnd.left, m_rect_wnd.top, m_rect_wnd.right
						- m_rect_wnd.left, m_rect_wnd.bottom - m_rect_wnd.top, m_parent_hwnd,
				NULL, m_hinstance, NULL);
	else
		m_hwnd = HGE_WINAPI_UNICODE_SUFFIX(CreateWindowEx)(WS_EX_TOPMOST,
				WINDOW_CLASS_NAME, m_window_title, m_style_fullscreen, 0, 0, 0, 0, NULL, NULL,
				m_hinstance, NULL);
	if (!m_hwnd)
	{
		_PostError(TXT("Can't create window"));
		return false;
	}

	ShowWindow(m_hwnd, SW_SHOW);

	// Init subsystems

	timeBeginPeriod(1);
	Random_Seed();
	_InitPowerStatus();
	_InputInit();
	if (!_GfxInit())
	{
		System_Shutdown();
		return false;
	}
	if (!_SoundInit())
	{
		System_Shutdown();
		return false;
	}

	System_Log(TXT("Init done.\n"));

	m_time = 0.0f;
	m_time_t0 = m_time_t0fps = timeGetTime();
	m_time_dt = m_time_cfps = 0;
	m_time_fps = 0;

	// Show splash

#ifdef DEMO

	bool (*func)();
	bool (*rfunc)();
	HWND hwndTmp;

	if (pHGE->m_show_splash_flag)
	{
		Sleep(200);
		func = (bool(*)()) pHGE->System_GetStateFunc(HGE_FRAMEFUNC);
		rfunc = (bool(*)()) pHGE->System_GetStateFunc(HGE_RENDERFUNC);
		hwndTmp = m_parent_hwnd;
		m_parent_hwnd = 0;
		pHGE->System_SetStateFunc(HGE_FRAMEFUNC, DFrame);
		pHGE->System_SetStateFunc(HGE_RENDERFUNC, 0);
		DInit();
		pHGE->System_Start();
		DDone();
		m_parent_hwnd = hwndTmp;
		pHGE->System_SetStateFunc(HGE_FRAMEFUNC, func);
		pHGE->System_SetStateFunc(HGE_RENDERFUNC, rfunc);
	}

#endif

	// Done

	return true;
}



void HGE_CALL HGE_Impl::System_Shutdown()
{
	System_Log(TXT("\nFinishing.."));

	timeEndPeriod(1);
	if (m_res_search_handle)
	{
		FindClose(m_res_search_handle);
		m_res_search_handle = 0;
	}
	_ClearQueue();
	_SoundDone();
	_GfxDone();
	_DonePowerStatus();

	if (m_hwnd)
	{
		//ShowWindow(hwnd, SW_HIDE);
		//SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
		//ShowWindow(hwnd, SW_SHOW);
		DestroyWindow(m_hwnd);
		m_hwnd = 0;
	}

	if (m_hinstance)
		HGE_WINAPI_UNICODE_SUFFIX(UnregisterClass)(WINDOW_CLASS_NAME, m_hinstance);

	System_Log(TXT("The End."));
}


bool HGE_CALL HGE_Impl::System_Start()
{
	MSG msg;

	if (!m_hwnd)
	{
		_PostError(TXT("System_Start: System_Initiate wasn't called"));
		return false;
	}

	if (!m_frame_func)
	{
		_PostError(TXT("System_Start: No frame function defined"));
		return false;
	}

	m_active_flag = true;

	// MAIN LOOP

	for (;;)
	{

		// Process window messages if not in "child mode"
		// (if in "child mode" the parent application will do this for us)

		if (!m_parent_hwnd)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;
				// TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
		}

		// Check if mouse is over HGE window for Input_IsMouseOver

		_UpdateMouse();

		// If HGE window is focused or we have the "don't suspend" state - process the main loop

		if (m_active_flag || m_dont_suspend_flag)
		{
			// Ensure we have at least 1ms time step
			// to not confuse user's code with 0

			do
			{
				m_time_dt = timeGetTime() - m_time_t0;
			} while (m_time_dt < 1);

			// If we reached the time for the next frame
			// or we just run in unlimited FPS mode, then
			// do the stuff

			if (m_time_dt >= m_time_fixed_delta)
			{
				// fDeltaTime = time step in seconds returned by Timer_GetDelta

				m_time_delta = m_time_dt / 1000.0f;

				// Cap too large time steps usually caused by lost focus to avoid jerks

				if (m_time_delta > 0.2f)
				{
					m_time_delta = m_time_fixed_delta ? m_time_fixed_delta / 1000.0f : 0.01f;
				}

				// Update time counter returned Timer_GetTime

				m_time += m_time_delta;

				// Store current time for the next frame
				// and count FPS

				m_time_t0 = timeGetTime();
				if (m_time_t0 - m_time_t0fps <= 1000)
					m_time_cfps++;
				else
				{
					m_time_fps = m_time_cfps;
					m_time_cfps = 0;
					m_time_t0fps = m_time_t0;
					_UpdatePowerStatus();
				}

				// Do user's stuff

				if (m_frame_func())
					break;
				if (m_render_func)
					m_render_func();

				// If if "child mode" - return after processing single frame

				if (m_parent_hwnd)
					break;

				// Clean up input events that were generated by
				// WindowProc and weren't handled by user's code

				_ClearQueue();

				// If we use VSYNC - we could afford a little
				// sleep to lower CPU usage

				// if(!bWindowed && nHGEFPS==HGEFPS_VSYNC) Sleep(1);
			}

			// If we have a fixed frame rate and the time
			// for the next frame isn't too close, sleep a bit

			else
			{
				if (m_time_fixed_delta && m_time_dt + 3 < m_time_fixed_delta)
					Sleep(1);
			}
		}

		// If main loop is suspended - just sleep a bit
		// (though not too much to allow instant window
		// redraw if requested by OS)

		else
			Sleep(1);
	}

	_ClearQueue();

	m_active_flag = false;

	return true;
}



void HGE_CALL HGE_Impl::System_SetStateBool(hgeBoolState state, bool value)
{
	switch (state)
	{
	case HGE_WINDOWED:
		if (m_vert_array || m_parent_hwnd)
			break;
		if (m_d3d_device && m_windowed_flag != value)
		{
			if (m_d3d_pp_wnd.BackBufferFormat == D3DFMT_UNKNOWN
					|| m_d3d_pp_fullscreen.BackBufferFormat == D3DFMT_UNKNOWN)
				break;

			if (m_windowed_flag)
				GetWindowRect(m_hwnd, &m_rect_wnd);
			m_windowed_flag = value;
			if (m_windowed_flag)
				m_d3d_pp = &m_d3d_pp_wnd;
			else
				m_d3d_pp = &m_d3d_pp_fullscreen;

			if (_format_id(m_d3d_pp->BackBufferFormat) < 4)
				m_screen_color_depth = 16;
			else
				m_screen_color_depth = 32;

			_GfxRestore();
			_AdjustWindow();
		}
		else
			m_windowed_flag = value;
		break;

	case HGE_ZBUFFER:
		if (!m_d3d_device)
			m_zbuffer_flag = value;
		break;

	case HGE_TEXTUREFILTER:
		m_texture_filter_flag = value;
		if (m_d3d_device)
		{
			_render_batch();
			if (m_texture_filter_flag)
			{
#if HGE_DIRECTX_VER == 8
				m_d3d_device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
				m_d3d_device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
#endif
#if HGE_DIRECTX_VER == 9
				m_d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
				m_d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
#endif
			}
			else
			{
#if HGE_DIRECTX_VER == 8
				m_d3d_device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
				m_d3d_device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_POINT);
#endif
#if HGE_DIRECTX_VER == 9
				m_d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
				m_d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
#endif
			}
		}
		break;

	case HGE_USESOUND:
		if (m_use_sound_flag != value)
		{
			m_use_sound_flag = value;
			if (m_use_sound_flag && m_hwnd)
				_SoundInit();
			if (!m_use_sound_flag && m_hwnd)
				_SoundDone();
		}
		break;

	case HGE_HIDEMOUSE:
		m_hide_mouse_flag = value;
		break;

	case HGE_DONTSUSPEND:
		m_dont_suspend_flag = value;
		break;

#ifdef DEMO
	case HGE_SHOWSPLASH:
		m_show_splash_flag = value;
		break;
#endif
	}
}



void HGE_CALL HGE_Impl::System_SetStateFunc(hgeFuncState state,
		hgeCallback value)
{
	switch (state)
	{
	case HGE_FRAMEFUNC:
		m_frame_func = value;
		break;
	case HGE_RENDERFUNC:
		m_render_func = value;
		break;
	case HGE_FOCUSLOSTFUNC:
		m_focus_lost_func = value;
		break;
	case HGE_FOCUSGAINFUNC:
		m_focus_gain_func = value;
		break;
	case HGE_GFXRESTOREFUNC:
		m_gfx_restore_func = value;
		break;
	case HGE_EXITFUNC:
		m_exit_func = value;
		break;
	}
}



void HGE_CALL HGE_Impl::System_SetStateHwnd(hgeHwndState state, HWND value)
{
	switch (state)
	{
	case HGE_HWNDPARENT:
		if (!m_hwnd)
			m_parent_hwnd = value;
		break;
	}
}



void HGE_CALL HGE_Impl::System_SetStateInt(hgeIntState state, int value)
{
	switch (state)
	{
	case HGE_SCREENWIDTH:
		if (!m_d3d_device)
			m_screen_width = value;
		break;

	case HGE_SCREENHEIGHT:
		if (!m_d3d_device)
			m_screen_height = value;
		break;

	case HGE_SCREENBPP:
		if (!m_d3d_device)
			m_screen_color_depth = value;
		break;

	case HGE_SAMPLERATE:
		if (!m_bass_dll)
			m_sample_rate = value;
		break;

	case HGE_FXVOLUME:
		m_fx_volume = value;
		_SetFXVolume(m_fx_volume);
		break;

	case HGE_MUSVOLUME:
		m_mus_volume = value;
		_SetMusVolume(m_mus_volume);
		break;

	case HGE_STREAMVOLUME:
		m_stream_volume = value;
		_SetStreamVolume(m_stream_volume);
		break;

	case HGE_FPS:
		if (m_vert_array)
			break;

		if (m_d3d_device)
		{
			if ((m_framerate_limit >= 0 && value < 0) || (m_framerate_limit < 0 && value >= 0))
			{
				if (value == HGEFPS_VSYNC)
				{
#if HGE_DIRECTX_VER == 8
					m_d3d_pp_wnd.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
					m_d3d_pp_fullscreen.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
#endif
#if HGE_DIRECTX_VER == 9
					m_d3d_pp_wnd.SwapEffect = D3DSWAPEFFECT_COPY;
					m_d3d_pp_wnd.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
					m_d3d_pp_fullscreen.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
#endif
				}
				else
				{
					m_d3d_pp_wnd.SwapEffect = D3DSWAPEFFECT_COPY;
#if HGE_DIRECTX_VER == 8
					m_d3d_pp_fullscreen.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif
#if HGE_DIRECTX_VER == 9
					m_d3d_pp_fullscreen.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif
				}
				//if(procFocusLostFunc) procFocusLostFunc();
				_GfxRestore();
				//if(procFocusGainFunc) procFocusGainFunc();
			}
		}
		m_framerate_limit = value;
		if (m_framerate_limit > 0)
			m_time_fixed_delta = int(1000.0f / value);
		else
			m_time_fixed_delta = 0;
		break;
	}
}



void HGE_CALL HGE_Impl::System_SetStateString(hgeStringState state,
		hgeConstString value)
{
	FILE *hf;

	switch (state)
	{
	case HGE_ICON:
		m_window_icon = value;
		if (pHGE->m_hwnd)
		{
			HGE_WINAPI_UNICODE_SUFFIX(SetClassLong)(pHGE->m_hwnd, GCL_HICON,
					(LONG) HGE_WINAPI_UNICODE_SUFFIX(LoadIcon)(pHGE->m_hinstance,
							m_window_icon));
		}
		break;

	case HGE_TITLE:
		hge_strcpy(m_window_title, value);
		if (pHGE->m_hwnd)
		{
			HGE_WINAPI_UNICODE_SUFFIX(SetWindowText)(pHGE->m_hwnd, m_window_title);
		}
		break;

	case HGE_INIFILE:
		if (value)
			hge_strcpy(m_ini_file_path, Resource_MakePath(value));
		else
			m_ini_file_path[0] = 0;
		break;

	case HGE_LOGFILE:
		if (value)
		{
			hge_strcpy(m_log_file_path, Resource_MakePath(value));
			hf = hge_fopen_w(m_log_file_path);
			if (!hf)
				m_log_file_path[0] = 0;
			else
				fclose(hf);
		}
		else
			m_log_file_path[0] = 0;
		break;
	}
}



bool HGE_CALL HGE_Impl::System_GetStateBool(hgeBoolState state)
{
	switch (state)
	{
	case HGE_WINDOWED:		return m_windowed_flag;
	case HGE_ZBUFFER:		return m_zbuffer_flag;
	case HGE_TEXTUREFILTER:	return m_texture_filter_flag;
	case HGE_USESOUND:		return m_use_sound_flag;
	case HGE_DONTSUSPEND:	return m_dont_suspend_flag;
	case HGE_HIDEMOUSE:		return m_hide_mouse_flag;

#ifdef DEMO
	case HGE_SHOWSPLASH:	return m_show_splash_flag;
#endif
	}

	return false;
}



hgeCallback HGE_CALL HGE_Impl::System_GetStateFunc(hgeFuncState state)
{
    switch (state)
	{
        case HGE_FRAMEFUNC:     return m_frame_func;
        case HGE_RENDERFUNC:    return m_render_func;
        case HGE_FOCUSLOSTFUNC: return m_focus_lost_func;
        case HGE_FOCUSGAINFUNC: return m_focus_gain_func;
        case HGE_EXITFUNC:      return m_exit_func;
    }

    return NULL;
}



HWND HGE_CALL HGE_Impl::System_GetStateHwnd(hgeHwndState state)
{
	switch (state)
	{
	case HGE_HWND:			return m_hwnd;
	case HGE_HWNDPARENT:	return m_parent_hwnd;
	}

	return 0;
}



int HGE_CALL HGE_Impl::System_GetStateInt(hgeIntState state)
{
    switch (state)
    {
        case HGE_SCREENWIDTH:   return m_screen_width;
        case HGE_SCREENHEIGHT:  return m_screen_height;
        case HGE_SCREENBPP:     return m_screen_color_depth;
        case HGE_SAMPLERATE:    return m_sample_rate;
        case HGE_FXVOLUME:      return m_fx_volume;
        case HGE_MUSVOLUME:     return m_mus_volume;
        case HGE_STREAMVOLUME:  return m_stream_volume;
        case HGE_FPS:           return m_framerate_limit;
        case HGE_POWERSTATUS:   return m_power_status;
    }

    return 0;
}



hgeConstString HGE_CALL HGE_Impl::System_GetStateString(hgeStringState state)
{
	switch (state)
	{
	case HGE_ICON:		return (hgeConstString) m_window_icon;
	case HGE_TITLE:		return m_window_title;

	case HGE_INIFILE:
		if (m_ini_file_path[0])
			return m_ini_file_path;
		else
			return 0;
	case HGE_LOGFILE:
		if (m_log_file_path[0])
			return m_log_file_path;
		else
			return 0;
	}

	return NULL;
}



hgeConstString HGE_CALL HGE_Impl::System_GetErrorMessage()
{
	return m_error_str;
}



void HGE_CALL HGE_Impl::System_Log(hgeConstString szFormat, ...)
{
	FILE *hf = NULL;
	va_list ap;

	if (!m_log_file_path[0])
		return;

	hf = hge_fopen_ab(m_log_file_path);
	if (!hf)
		return;

	va_start(ap, szFormat);
	hge_vfprintf(hf, szFormat, ap);
	va_end(ap);

	hge_fprintf(hf, TXT("\n") );

	fclose(hf);
}



bool HGE_CALL HGE_Impl::System_Launch(hgeConstString url)
{
	if ((uint32_t) HGE_WINAPI_UNICODE_SUFFIX(ShellExecute)(pHGE->m_hwnd, NULL,
			url, NULL, NULL, SW_SHOWMAXIMIZED ) > 32)
	{
		return true;
	}
	return false;
}



void HGE_CALL HGE_Impl::System_Snapshot(hgeConstString filename)
{
	hgeGAPISurface * pSurf;
	hgeChar * shotname, tempname[_MAX_PATH];
	int i;

	if (!filename)
	{
		i = 0;
		shotname = Resource_EnumFiles(TXT("shot???.bmp"));
		while (shotname)
		{
			i++;
			shotname = Resource_EnumFiles();
		}
		hge_sprintf(tempname, sizeof tempname, TXT("shot%03d.bmp"), i);
		filename = Resource_MakePath(tempname);
	}

	if (m_d3d_device)
	{
#if HGE_DIRECTX_VER == 8
		m_d3d_device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pSurf);
#endif
#if HGE_DIRECTX_VER == 9
		m_d3d_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurf);
#endif
		HGE_WINAPI_UNICODE_SUFFIX(D3DXSaveSurfaceToFile)(filename, D3DXIFF_BMP,
				pSurf, NULL, NULL);
		pSurf->Release();
	}
}


//////// Implementation ////////


HGE_Impl::HGE_Impl()
{
	m_hinstance = GetModuleHandle(0);
	m_hwnd = 0;
	m_active_flag = false;
	m_error_str[0] = 0;

	m_d3d = 0;
	m_d3d_device = 0;
	m_d3d_pp = 0;
	m_targets_list = 0;
	m_current_target = 0;
	m_screen_surf = 0;
	m_depth_surf = 0;
	m_vertex_buf = 0;
	m_index_buf = 0;
	m_vert_array = 0;
	m_texture_list = 0;

	m_bass_dll = 0;
	m_nosound_flag = false;
	m_streams = 0;

	m_res_search_handle = 0;
	m_res_list = 0;

	m_input_queue = 0;
	m_input_char = m_input_vkey = m_input_zpos = 0;
	m_input_xpos = m_input_ypos = 0.0f;
	m_input_mouseover_flag = false;
	m_input_captured_flag = false;

	m_framerate_limit = HGEFPS_UNLIMITED;
	m_time = 0.0f;
	m_time_delta = 0.0f;
	m_time_fps = 0;

	m_frame_func = 0;
	m_render_func = 0;
	m_focus_lost_func = 0;
	m_focus_gain_func = 0;
	m_gfx_restore_func = 0;
	m_exit_func = 0;
	m_window_icon = 0;
	hge_strcpy(m_window_title, TXT("HGE") );
	m_screen_width = 800;
	m_screen_height = 600;
	m_screen_color_depth = 32;
	m_windowed_flag = false;
	m_zbuffer_flag = false;
	m_texture_filter_flag = true;
	m_log_file_path[0] = 0;
	m_ini_file_path[0] = 0;
	m_use_sound_flag = true;
	m_sample_rate = 44100;
	m_fx_volume = 100;
	m_mus_volume = 100;
	m_stream_volume = 100;
	m_time_fixed_delta = 0;
	m_hide_mouse_flag = true;
	m_dont_suspend_flag = false;
	m_parent_hwnd = 0;

	m_power_status = HGEPWR_UNSUPPORTED;
	m_kernel32_module = NULL;
	m_GetSystemPowerStatus_func = NULL;

#ifdef DEMO
	m_show_splash_flag = true;
#endif

	HGE_WINAPI_UNICODE_SUFFIX(GetModuleFileName)(GetModuleHandle(NULL),
			m_app_path, sizeof(m_app_path));
	int i;
	for (i = hge_strlen(m_app_path) - 1; i > 0; i--)
		if (m_app_path[i] == '\\')
			break;
	m_app_path[i + 1] = 0;
}



void HGE_Impl::_PostError(hgeConstString error)
{
	System_Log(error);
	hge_strcpy(m_error_str, error);
}



void HGE_Impl::_FocusChange(bool bAct)
{
	m_active_flag = bAct;
	if (m_active_flag)
	{
		if (m_focus_gain_func)
			m_focus_gain_func();
	}
	else
	{
		if (m_focus_lost_func)
			m_focus_lost_func();
	}
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	bool bActivating;

	switch (msg)
	{
	case WM_CREATE:
		return FALSE;

	case WM_PAINT:
		if (pHGE->m_d3d && pHGE->m_render_func && pHGE->m_windowed_flag)
			pHGE->m_render_func();
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
		bActivating = (LOWORD(wparam) != WA_INACTIVE) && (HIWORD(wparam) == 0);
		if (pHGE->m_d3d && pHGE->m_active_flag != bActivating)
			pHGE->_FocusChange(bActivating);
		return FALSE;

	case WM_SETCURSOR:
		if (pHGE->m_active_flag && LOWORD(lparam) == HTCLIENT && pHGE->m_hide_mouse_flag)
			SetCursor(NULL);
		else
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		return FALSE;

	case WM_SYSKEYDOWN:
		if (wparam == VK_F4)
		{
			if (pHGE->m_exit_func && !pHGE->m_exit_func())
				return FALSE;
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		else if (wparam == VK_RETURN)
		{
			pHGE->System_SetState(HGE_WINDOWED, !pHGE->System_GetState(
					HGE_WINDOWED));
			return FALSE;
		}
		else
		{
			pHGE->_BuildEvent(INPUT_KEYDOWN, wparam, HIWORD(lparam) & 0xFF,
					(lparam & 0x40000000) ? HGEINP_REPEAT : 0, -1, -1);
			return FALSE;
		}

	case WM_KEYDOWN:
		pHGE->_BuildEvent(INPUT_KEYDOWN, wparam, HIWORD(lparam) & 0xFF, (lparam
				& 0x40000000) ? HGEINP_REPEAT : 0, -1, -1);
		return FALSE;
	case WM_SYSKEYUP:
		pHGE->_BuildEvent(INPUT_KEYUP, wparam, HIWORD(lparam) & 0xFF, 0, -1, -1);
		return FALSE;
	case WM_KEYUP:
		pHGE->_BuildEvent(INPUT_KEYUP, wparam, HIWORD(lparam) & 0xFF, 0, -1, -1);
		return FALSE;

	case WM_LBUTTONDOWN:
		SetFocus(hwnd);
		pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_LBUTTON, 0, 0,
				LOWORDINT(lparam), HIWORDINT(lparam));
		return FALSE;
	case WM_MBUTTONDOWN:
		SetFocus(hwnd);
		pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_MBUTTON, 0, 0,
				LOWORDINT(lparam), HIWORDINT(lparam));
		return FALSE;
	case WM_RBUTTONDOWN:
		SetFocus(hwnd);
		pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_RBUTTON, 0, 0,
				LOWORDINT(lparam), HIWORDINT(lparam));
		return FALSE;

	case WM_LBUTTONDBLCLK:
		pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_LBUTTON, 0, HGEINP_REPEAT,
				LOWORDINT(lparam), HIWORDINT(lparam));
		return FALSE;
	case WM_MBUTTONDBLCLK:
		pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_MBUTTON, 0, HGEINP_REPEAT,
				LOWORDINT(lparam), HIWORDINT(lparam));
		return FALSE;
	case WM_RBUTTONDBLCLK:
		pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_RBUTTON, 0, HGEINP_REPEAT,
				LOWORDINT(lparam), HIWORDINT(lparam));
		return FALSE;

	case WM_LBUTTONUP:
		pHGE->_BuildEvent(INPUT_MBUTTONUP, HGEK_LBUTTON, 0, 0,
				LOWORDINT(lparam), HIWORDINT(lparam));
		return FALSE;
	case WM_MBUTTONUP:
		pHGE->_BuildEvent(INPUT_MBUTTONUP, HGEK_MBUTTON, 0, 0,
				LOWORDINT(lparam), HIWORDINT(lparam));
		return FALSE;
	case WM_RBUTTONUP:
		pHGE->_BuildEvent(INPUT_MBUTTONUP, HGEK_RBUTTON, 0, 0,
				LOWORDINT(lparam), HIWORDINT(lparam));
		return FALSE;

	case WM_MOUSEMOVE:
		pHGE->_BuildEvent(INPUT_MOUSEMOVE, 0, 0, 0, LOWORDINT(lparam),
				HIWORDINT(lparam));
		return FALSE;
	case 0x020A: // WM_MOUSEWHEEL, GET_WHEEL_DELTA_WPARAM(wparam);
		pHGE->_BuildEvent(INPUT_MOUSEWHEEL, short(HIWORD(wparam)) / 120, 0, 0,
				LOWORDINT(lparam), HIWORDINT(lparam));
		return FALSE;

	case WM_SIZE:
		if (pHGE->m_d3d && wparam == SIZE_RESTORED)
			pHGE->_Resize(LOWORD(lparam), HIWORD(lparam));
		//return FALSE;
		break;

	case WM_SYSCOMMAND:
		if (wparam == SC_CLOSE)
		{
			if (pHGE->m_exit_func && !pHGE->m_exit_func())
				return FALSE;
			pHGE->m_active_flag = false;
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

