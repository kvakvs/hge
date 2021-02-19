/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Core system functions
//

#include "hge_impl.h"


#define LOWORDINT(n) ((int)((signed short)(LOWORD(n))))
#define HIWORDINT(n) ((int)((signed short)(HIWORD(n))))


const char *WINDOW_CLASS_NAME = "HGE__WNDCLASS";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

namespace hgeImpl {

    int nRef = 0;
    HGE_Impl *pHGE = nullptr;


    BOOL APIENTRY DllMain(HANDLE, uint32_t, LPVOID) {
      return TRUE;
    }


    HGE_Impl *HGE_Impl::interface_get() {
      if (!pHGE) {
        pHGE = new HGE_Impl();
      }

      nRef++;

      return pHGE;
    }


    void HGE_CALL HGE_Impl::Release() {
      nRef--;

      if (!nRef) {
        if (pHGE->hwnd_) {
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

      System_Log("Application: %s", win_title_.c_str());
      os_ver.dwOSVersionInfoSize = sizeof(os_ver);

      // Deprecated? Too bad
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
      winclass.hInstance = h_instance_;
      winclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
      winclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
      winclass.lpszMenuName = nullptr;
      winclass.lpszClassName = WINDOW_CLASS_NAME;
      if (icon_.length() > 0) {
        winclass.hIcon = LoadIcon(h_instance_, icon_.c_str());
      } else {
        winclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
      }

      if (!RegisterClass(&winclass)) {
        post_error("Can't register window class");
        return false;
      }

      // Create window
      // Thanks to Bryan for window rectangle fix in later Windows systems 7,8
      // http://i-am-bryan.com/webs/tutorials/hge/fix-hge-window-sizing-bug/

      // Take approximately center position
      const auto scr_width = GetSystemMetrics(SM_CXSCREEN);
      const auto scr_height = GetSystemMetrics(SM_CYSCREEN);
      rect_windowed_.left = (scr_width - screen_width_) / 2;
      rect_windowed_.top = (scr_height - screen_height_) / 2;
      rect_windowed_.right = rect_windowed_.left + screen_width_;
      rect_windowed_.bottom = rect_windowed_.top + screen_height_;

      style_windowed_ = WS_BORDER
                        | WS_POPUP
                        | WS_CAPTION
                        | WS_SYSMENU
                        | WS_MINIMIZEBOX
                        | WS_VISIBLE;

      //Fullscreen
      rect_fullscreen_.left = 0;
      rect_fullscreen_.top = 0;
      rect_fullscreen_.right = screen_width_;
      rect_fullscreen_.bottom = screen_height_;
      style_fullscreen_ = WS_POPUP | WS_VISIBLE; //WS_POPUP

      if (hwnd_parent_) {
        rect_windowed_.left = 0;
        rect_windowed_.top = 0;
        rect_windowed_.right = screen_width_;
        rect_windowed_.bottom = screen_height_;
        style_windowed_ = WS_CHILD | WS_VISIBLE;
        windowed_ = true;
      }

      // Fix for styled windows in Windows versions newer than XP
      AdjustWindowRect(&rect_windowed_, style_windowed_, false);

      if (windowed_)
        hwnd_ = CreateWindowEx(0, WINDOW_CLASS_NAME, win_title_.c_str(), style_windowed_,
                               rect_windowed_.left, rect_windowed_.top,
                               rect_windowed_.right - rect_windowed_.left,
                               rect_windowed_.bottom - rect_windowed_.top,
                               hwnd_parent_, nullptr, h_instance_, nullptr);
      else
        hwnd_ = CreateWindowEx(WS_EX_TOPMOST, WINDOW_CLASS_NAME,
                               win_title_.c_str(), style_fullscreen_,
                               0, 0, 0, 0,
                               nullptr, nullptr, h_instance_, nullptr);
      if (!hwnd_) {
        post_error("Can't create window");
        return false;
      }

      ShowWindow(hwnd_, SW_SHOW);

      // Init subsystems

      timeBeginPeriod(1);
      Random_Seed();
      init_power_status();
      input_init();
      if (!gfx_init()) {
        System_Shutdown();
        return false;
      }
      if (!sound_init()) {
        System_Shutdown();
        return false;
      }

      System_Log("Init done.\n");

      time_ = 0.0f;
      t0_ = t0_fps_ = timeGetTime();
      dt_ = cfps_ = 0;
      fps_ = 0;

      // Show splash

#ifdef HGE_SPLASH_ENABLE

      if (pHGE->splash_screen_enabled_) {
        Sleep(200);
        const auto func = static_cast<bool (*)()>(pHGE->System_GetStateFunc(HGE_FRAMEFUNC));
        const auto rfunc = static_cast<bool (*)()>(pHGE->System_GetStateFunc(HGE_RENDERFUNC));
        const auto hwnd_tmp = hwnd_parent_;
        hwnd_parent_ = nullptr;
        pHGE->System_SetStateFunc(HGE_FRAMEFUNC, demo_render_frame);
        pHGE->System_SetStateFunc(HGE_RENDERFUNC, nullptr);
        prepare_demo();
        pHGE->System_Start();
        finish_demo();
        hwnd_parent_ = hwnd_tmp;
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
      if (h_search_) {
        FindClose(h_search_);
        h_search_ = nullptr;
      }
      clear_queue();
      sound_done();
      gfx_done();
      done_power_status();

      if (hwnd_) {
        //ShowWindow(hwnd, SW_HIDE);
        //SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
        //ShowWindow(hwnd, SW_SHOW);
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
      }

      if (h_instance_) {
        UnregisterClass(WINDOW_CLASS_NAME, h_instance_);
      }

      System_Log("The End.");
    }


    bool HGE_CALL HGE_Impl::System_Start() {
      MSG msg;

      if (!hwnd_) {
        post_error("System_Start: System_Initiate wasn't called");
        return false;
      }

      if (!proc_frame_func_) {
        post_error("System_Start: No frame function defined");
        return false;
      }

      active_ = true;

      // MAIN LOOP

      for (;;) {

        // Process window messages if not in "child mode"
        // (if in "child mode" the parent application will do this for us)

        if (!hwnd_parent_) {
          if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
              break;
            }
            // TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
          }
        }

        // Check if mouse is over HGE window for Input_IsMouseOver

        update_mouse();

        // If HGE window is focused or we have the "don't suspend" state - process the main loop

        if (active_ || dont_suspend_) {
          // Ensure we have at least 1ms time step
          // to not confuse user's code with 0

          do {
            dt_ = timeGetTime() - t0_;
          } while (dt_ < 1);

          // If we reached the time for the next frame
          // or we just run in unlimited FPS mode, then
          // do the stuff

          if (dt_ >= fixed_delta_) {
            // fDeltaTime = time step in seconds returned by Timer_GetDelta

            delta_time_ = dt_ / 1000.0f;

            // Cap too large time steps usually caused by lost focus to avoid jerks

            if (delta_time_ > 0.2f) {
              delta_time_ = fixed_delta_ ? fixed_delta_ / 1000.0f : 0.01f;
            }

            // Update time counter returned Timer_GetTime

            time_ += delta_time_;

            // Store current time for the next frame
            // and count FPS

            t0_ = timeGetTime();
            if (t0_ - t0_fps_ <= 1000) {
              cfps_++;
            } else {
              fps_ = cfps_;
              cfps_ = 0;
              t0_fps_ = t0_;
              update_power_status();
            }

            // Do user's stuff

            if (proc_frame_func_()) {
              break;
            }
            if (proc_render_func_) {
              proc_render_func_();
            }

            // If if "child mode" - return after processing single frame

            if (hwnd_parent_) {
              break;
            }

            // Clean up input events that were generated by
            // WindowProc and weren't handled by user's code

            clear_queue();

            // If we use VSYNC - we could afford a little
            // sleep to lower CPU usage

            // if(!bWindowed && nHGEFPS==HGEFPS_VSYNC) Sleep(1);
          }

            // If we have a fixed frame rate and the time
            // for the next frame isn't too close, sleep a bit

          else {
            if (fixed_delta_ && dt_ + 3 < fixed_delta_) {
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

      clear_queue();

      active_ = false;

      return true;
    }

    void HGE_CALL HGE_Impl::System_SetStateBool(const hgeBoolState state, const bool value) {
      switch (state) {
        case HGE_WINDOWED:
          if (vert_array_ || hwnd_parent_) {
            break;
          }
          if (d3d_device_ && windowed_ != value) {
            if (d3dpp_windowed_.BackBufferFormat == D3DFMT_UNKNOWN
                || d3dpp_fullscreen_.BackBufferFormat == D3DFMT_UNKNOWN) {
              break;
            }

            if (windowed_) {
              GetWindowRect(hwnd_, &rect_windowed_);
            }
            windowed_ = value;
            if (windowed_) {
              d3dpp_ = &d3dpp_windowed_;
            } else {
              d3dpp_ = &d3dpp_fullscreen_;
            }

            if (format_id(d3dpp_->BackBufferFormat) < 4) {
              screen_bpp_ = 16;
            } else {
              screen_bpp_ = 32;
            }

            gfx_restore();
            adjust_window();
          } else {
            windowed_ = value;
          }
          break;

        case HGE_ZBUFFER:
          if (!d3d_device_) {
            z_buffer_ = value;
          }
          break;

        case HGE_TEXTUREFILTER:
          texture_filter_ = value;
          if (d3d_device_) {
            render_batch();
            if (texture_filter_) {
              d3d_device_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
              d3d_device_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            } else {
              d3d_device_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
              d3d_device_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
            }
          }
          break;

        case HGE_USESOUND:
          if (use_sound_ != value) {
            use_sound_ = value;
            if (use_sound_ && hwnd_) {
              sound_init();
            }
            if (!use_sound_ && hwnd_) {
              sound_done();
            }
          }
          break;

        case HGE_HIDEMOUSE:
          hide_mouse_ = value;
          break;

        case HGE_DONTSUSPEND:
          dont_suspend_ = value;
          break;

#ifdef HGE_SPLASH_ENABLE
        case HGE_SHOWSPLASH:
          splash_screen_enabled_ = value;
          break;
#endif
      }
    }

    void HGE_CALL HGE_Impl::System_SetStateFunc(const hgeFuncState state,
                                                const hgeCallback value) {
      switch (state) {
        case HGE_FRAMEFUNC:
          proc_frame_func_ = value;
          break;
        case HGE_RENDERFUNC:
          proc_render_func_ = value;
          break;
        case HGE_FOCUSLOSTFUNC:
          proc_focus_lost_func_ = value;
          break;
        case HGE_FOCUSGAINFUNC:
          proc_focus_gain_func_ = value;
          break;
        case HGE_GFXRESTOREFUNC:
          proc_gfx_restore_func_ = value;
          break;
        case HGE_EXITFUNC:
          proc_exit_func_ = value;
          break;
      }
    }

    void HGE_CALL HGE_Impl::System_SetStateHwnd(const hgeHwndState state,
                                                HWND value) {
      switch (state) {
        case HGE_HWNDPARENT:
          if (!hwnd_) {
            hwnd_parent_ = value;
          }
          break;
        case HGE_HWND:
          break;
      }
    }

    void HGE_CALL HGE_Impl::System_SetStateInt(const hgeIntState state, const int value) {
      switch (state) {
        case HGE_SCREENWIDTH:
          if (!d3d_device_) {
            screen_width_ = value;
          }
          break;

        case HGE_SCREENHEIGHT:
          if (!d3d_device_) {
            screen_height_ = value;
          }
          break;

        case HGE_SCREENBPP:
          if (!d3d_device_) {
            screen_bpp_ = value;
          }
          break;

        case HGE_SAMPLERATE:
          if (!hBass) {
            sample_rate_ = value;
          }
          break;

        case HGE_FXVOLUME:
          fx_volume_ = value;
          set_fx_volume(fx_volume_);
          break;

        case HGE_MUSVOLUME:
          mus_volume_ = value;
          set_mus_volume(mus_volume_);
          break;

        case HGE_STREAMVOLUME:
          stream_volume_ = value;
          set_stream_volume(stream_volume_);
          break;

        case HGE_FPS:
          if (vert_array_) {
            break;
          }

          if (d3d_device_) {
            if ((hgefps_ >= 0 && value < 0) || (hgefps_ < 0 && value >= 0)) {
              if (value == HGEFPS_VSYNC) {
                d3dpp_windowed_.SwapEffect = D3DSWAPEFFECT_COPY;
                d3dpp_windowed_.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
                d3dpp_fullscreen_.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
              } else {
                d3dpp_windowed_.SwapEffect = D3DSWAPEFFECT_COPY;
                d3dpp_fullscreen_.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
              }
              //if(procFocusLostFunc) procFocusLostFunc();
              gfx_restore();
              //if(procFocusGainFunc) procFocusGainFunc();
            }
          }
          hgefps_ = value;
          if (hgefps_ > 0) {
            fixed_delta_ = int(1000.0f / (float) value);
          } else {
            fixed_delta_ = 0;
          }
          break;
        case HGE_POWERSTATUS:
          break;
      }
    }

#ifndef GCL_HICON
#define GCL_HICON -14
#endif

    void HGE_CALL HGE_Impl::System_SetStateString(const hgeStringState state, const char *value) {
      switch (state) {
        case HGE_ICON:
          icon_ = value;
          if (pHGE->hwnd_) {
            SetClassLong(pHGE->hwnd_, GCL_HICON,
                         reinterpret_cast<LONG>(LoadIcon(pHGE->h_instance_, icon_.c_str())));
          }
          break;
        case HGE_TITLE:
          win_title_ = value;
          if (pHGE->hwnd_) {
            SetWindowText(pHGE->hwnd_, win_title_.c_str());
          }
          break;
        case HGE_INIFILE:
          if (value) {
            ini_file_ = Resource_MakePath(value);
          } else {
            ini_file_.clear();
          }
          break;
        case HGE_LOGFILE:
          if (value) {
            log_file_ = Resource_MakePath(value);
            FILE *hf = fopen(log_file_.c_str(), "w");
            if (!hf) {
              log_file_.clear();
            } else {
              fclose(hf);
            }
          } else {
            log_file_.clear();
          }
          break;
      }
    }

    bool HGE_CALL HGE_Impl::System_GetStateBool(const hgeBoolState state) {
      switch (state) {
        case HGE_WINDOWED:
          return windowed_;
        case HGE_ZBUFFER:
          return z_buffer_;
        case HGE_TEXTUREFILTER:
          return texture_filter_;
        case HGE_USESOUND:
          return use_sound_;
        case HGE_DONTSUSPEND:
          return dont_suspend_;
        case HGE_HIDEMOUSE:
          return hide_mouse_;

#ifdef HGE_SPLASH_ENABLE
        case HGE_SHOWSPLASH:
          return splash_screen_enabled_;
#endif
      }

      return false;
    }

    hgeCallback HGE_CALL HGE_Impl::System_GetStateFunc(const hgeFuncState state) {
      switch (state) {
        case HGE_FRAMEFUNC:
          return proc_frame_func_;
        case HGE_RENDERFUNC:
          return proc_render_func_;
        case HGE_FOCUSLOSTFUNC:
          return proc_focus_lost_func_;
        case HGE_FOCUSGAINFUNC:
          return proc_focus_gain_func_;
        case HGE_EXITFUNC:
          return proc_exit_func_;
        case HGE_GFXRESTOREFUNC:
          break;
      }

      return nullptr;
    }

    HWND HGE_CALL HGE_Impl::System_GetStateHwnd(const hgeHwndState state) {
      switch (state) {
        case HGE_HWND:
          return hwnd_;
        case HGE_HWNDPARENT:
          return hwnd_parent_;
      }

      return nullptr;
    }

    int HGE_CALL HGE_Impl::System_GetStateInt(const hgeIntState state) {
      switch (state) {
        case HGE_SCREENWIDTH:
          return screen_width_;
        case HGE_SCREENHEIGHT:
          return screen_height_;
        case HGE_SCREENBPP:
          return screen_bpp_;
        case HGE_SAMPLERATE:
          return sample_rate_;
        case HGE_FXVOLUME:
          return fx_volume_;
        case HGE_MUSVOLUME:
          return mus_volume_;
        case HGE_STREAMVOLUME:
          return stream_volume_;
        case HGE_FPS:
          return hgefps_;
        case HGE_POWERSTATUS:
          return power_status_;
      }

      return 0;
    }

    const char *HGE_CALL HGE_Impl::System_GetStateString(const hgeStringState state) {
      switch (state) {
        case HGE_ICON:
          return icon_.c_str();
        case HGE_TITLE:
          return win_title_.c_str();
        case HGE_INIFILE: {
          if (!ini_file_.empty()) {
            return ini_file_.c_str();
          }
          return nullptr;
        }
        case HGE_LOGFILE: {
          if (!log_file_.empty()) {
            return log_file_.c_str();
          }
          return nullptr;
        }
      }

      return nullptr;
    }

    const char *HGE_CALL HGE_Impl::System_GetErrorMessage() const {
      return error_.c_str();
    }

// NOLINTNEXTLINE
    void HGE_CALL HGE_Impl::System_Log(const char *szFormat, ...) {
      va_list ap;

      if (log_file_.empty()) {
        return;
      }

      FILE *hf = fopen(log_file_.c_str(), "a");
      if (!hf) {
        return;
      }

              va_start(ap, szFormat);
      vfprintf(hf, szFormat, ap);
              va_end(ap);

      fprintf(hf, "\n");

      fclose(hf);
    }

    bool HGE_CALL HGE_Impl::System_Launch(const char *url) {
      const auto shell_execute = reinterpret_cast<uint32_t>(
              ShellExecute(pHGE->hwnd_, nullptr, url, nullptr, nullptr, SW_SHOWMAXIMIZED)
      );
      if (shell_execute > 32) {
        return true;
      }
      return false;
    }

// NOLINTNEXTLINE
    void HGE_CALL HGE_Impl::System_Snapshot(const char *filename) {
      hgeGAPISurface *pSurf;
      char tempname[_MAX_PATH];

      if (!filename) {
        int i = 0;
        const char *shotname = Resource_EnumFiles("shot???.bmp");
        while (shotname) {
          i++;
          shotname = Resource_EnumFiles();
        }
        sprintf(tempname, "shot%03d.bmp", i);
        filename = Resource_MakePath(tempname);
      }

      if (d3d_device_) {
        d3d_device_->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurf);
        D3DXSaveSurfaceToFile(filename, D3DXIFF_BMP, pSurf, nullptr, nullptr);
        pSurf->Release();
      }
    }

//////// Implementation ////////


// NOLINTNEXTLINE
    HGE_Impl::HGE_Impl()
            : style_windowed_(0), style_fullscreen_(0), n_prim_(0), cur_prim_type_(0),
              cur_blend_mode_(0), cur_texture_(0), cur_shader_(0), t0_(0), t0_fps_(0),
              dt_(0), cfps_(0) {
      h_instance_ = GetModuleHandle(nullptr);
      hwnd_ = nullptr;
      active_ = false;
      error_[0] = 0;

      d3d_ = nullptr;
      d3d_device_ = nullptr;
      d3dpp_ = nullptr;
      targets_ = nullptr;
      cur_target_ = nullptr;
      screen_surf_ = nullptr;
      screen_depth_ = nullptr;
      vertex_buf_ = nullptr;
      index_buf_ = nullptr;
      vert_array_ = nullptr;
      textures_ = nullptr;

      hBass = nullptr;
      is_silent_ = false;
      sound_streams_ = nullptr;

      h_search_ = nullptr;

      input_char_ = v_key_ = zpos_ = 0;
      xpos_ = ypos_ = 0.0f;
      mouse_over_ = false;
      is_captured_ = false;

      hgefps_ = HGEFPS_UNLIMITED;
      time_ = 0.0f;
      delta_time_ = 0.0f;
      fps_ = 0;

      proc_frame_func_ = nullptr;
      proc_render_func_ = nullptr;
      proc_focus_lost_func_ = nullptr;
      proc_focus_gain_func_ = nullptr;
      proc_gfx_restore_func_ = nullptr;
      proc_exit_func_ = nullptr;
      win_title_ = "HGE";
      screen_width_ = 800;
      screen_height_ = 600;
      screen_bpp_ = 32;
      windowed_ = false;
      z_buffer_ = false;
      texture_filter_ = true;
      use_sound_ = true;
      sample_rate_ = 44100;
      fx_volume_ = 100;
      mus_volume_ = 100;
      stream_volume_ = 100;
      fixed_delta_ = 0;
      hide_mouse_ = true;
      dont_suspend_ = false;
      hwnd_parent_ = nullptr;

      power_status_ = HGEPWR_UNSUPPORTED;
      krnl32_ = nullptr;
      get_system_power_status_ = nullptr;

#ifdef HGE_SPLASH_ENABLE
      splash_screen_enabled_ = true;
#endif

      // Get application path and trim trailing slashes to get the directory
      {
        char tmp[MAX_PATH];
        GetModuleFileName(GetModuleHandle(nullptr), tmp, sizeof(tmp));

        size_t i;
        for (i = strlen(tmp) - 1; i > 0; i--)
          if (tmp[i] == '\\') {
            break;
          }
        tmp[i + 1] = 0;

        app_path_ = tmp;
      }
    }

    void HGE_Impl::post_error(char const *error) {
      System_Log(error);
      error_ = error;
    }

    void HGE_Impl::focus_change(const bool b_act) {
      active_ = b_act;

      if (active_) {
        if (proc_focus_gain_func_) {
          proc_focus_gain_func_();
        }
      } else {
        if (proc_focus_lost_func_) {
          proc_focus_lost_func_();
        }
      }
    }

    LRESULT HGE_Impl::window_proc(HWND hwnd, const UINT msg, WPARAM wparam, LPARAM lparam) {
      switch (msg) {
        case WM_CREATE:
          return FALSE;

        case WM_PAINT:
          if (hgeImpl::pHGE->d3d_
              && hgeImpl::pHGE->proc_render_func_
              && hgeImpl::pHGE->windowed_) {
            hgeImpl::pHGE->proc_render_func_();
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
        case WM_ACTIVATE: {
          // tricky: we should catch WA_ACTIVE and WA_CLICKACTIVE,
          // but only if HIWORD(wParam) (fMinimized) == FALSE (0)
          const bool activating = LOWORD(wparam) != WA_INACTIVE && HIWORD(wparam) == 0;
          if (hgeImpl::pHGE->d3d_
              && hgeImpl::pHGE->active_ != activating) {
            hgeImpl::pHGE->focus_change(activating);
          }
          return FALSE;
        }

        case WM_SETCURSOR:
          if (hgeImpl::pHGE->active_
              && LOWORD(lparam) == HTCLIENT
              && hgeImpl::pHGE->hide_mouse_) {
            SetCursor(nullptr);
          } else {
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
          }
          return FALSE;

        case WM_SYSKEYDOWN: {
          if (wparam == VK_F4) {
            if (hgeImpl::pHGE->proc_exit_func_
                && !hgeImpl::pHGE->proc_exit_func_()) {
              return FALSE;
            }
            return DefWindowProc(hwnd, msg, wparam, lparam);
          }
          if (wparam == VK_RETURN) {
            hgeImpl::pHGE->System_SetState(HGE_WINDOWED, !hgeImpl::pHGE->System_GetState(HGE_WINDOWED));
            return FALSE;
          }
          hgeImpl::pHGE->build_event(INPUT_KEYDOWN, wparam, HIWORD(lparam) & 0xFF,
                                     (lparam & 0x40000000) ? HGEINP_REPEAT : 0, -1, -1);
          return FALSE;
        }

        case WM_KEYDOWN:
          hgeImpl::pHGE->build_event(INPUT_KEYDOWN, wparam, HIWORD(lparam) & 0xFF,
                                     (lparam & 0x40000000) ? HGEINP_REPEAT : 0, -1, -1);
          return FALSE;

        case WM_SYSKEYUP:
        case WM_KEYUP:
          hgeImpl::pHGE->build_event(INPUT_KEYUP, wparam, HIWORD(lparam) & 0xFF, 0, -1, -1);
          return FALSE;

        case WM_LBUTTONDOWN:
          SetFocus(hwnd);
          hgeImpl::pHGE->build_event(INPUT_MBUTTONDOWN, HGEK_LBUTTON, 0, 0, LOWORDINT(lparam),
                                     HIWORDINT(lparam));
          return FALSE;
        case WM_MBUTTONDOWN:
          SetFocus(hwnd);
          hgeImpl::pHGE->build_event(INPUT_MBUTTONDOWN, HGEK_MBUTTON, 0, 0, LOWORDINT(lparam),
                                     HIWORDINT(lparam));
          return FALSE;
        case WM_RBUTTONDOWN:
          SetFocus(hwnd);
          hgeImpl::pHGE->build_event(INPUT_MBUTTONDOWN, HGEK_RBUTTON, 0, 0, LOWORDINT(lparam),
                                     HIWORDINT(lparam));
          return FALSE;

        case WM_LBUTTONDBLCLK:
          hgeImpl::pHGE->build_event(INPUT_MBUTTONDOWN, HGEK_LBUTTON, 0, HGEINP_REPEAT,
                                     LOWORDINT(lparam),
                                     HIWORDINT(lparam));
          return FALSE;
        case WM_MBUTTONDBLCLK:
          hgeImpl::pHGE->build_event(INPUT_MBUTTONDOWN, HGEK_MBUTTON, 0, HGEINP_REPEAT,
                                     LOWORDINT(lparam),
                                     HIWORDINT(lparam));
          return FALSE;
        case WM_RBUTTONDBLCLK:
          hgeImpl::pHGE->build_event(INPUT_MBUTTONDOWN, HGEK_RBUTTON, 0, HGEINP_REPEAT,
                                     LOWORDINT(lparam),
                                     HIWORDINT(lparam));
          return FALSE;

        case WM_LBUTTONUP:
          hgeImpl::pHGE->build_event(INPUT_MBUTTONUP, HGEK_LBUTTON, 0, 0, LOWORDINT(lparam),
                                     HIWORDINT(lparam));
          return FALSE;
        case WM_MBUTTONUP:
          hgeImpl::pHGE->build_event(INPUT_MBUTTONUP, HGEK_MBUTTON, 0, 0, LOWORDINT(lparam),
                                     HIWORDINT(lparam));
          return FALSE;
        case WM_RBUTTONUP:
          hgeImpl::pHGE->build_event(INPUT_MBUTTONUP, HGEK_RBUTTON, 0, 0, LOWORDINT(lparam),
                                     HIWORDINT(lparam));
          return FALSE;

        case WM_MOUSEMOVE:
          hgeImpl::pHGE->build_event(INPUT_MOUSEMOVE, 0, 0, 0, LOWORDINT(lparam), HIWORDINT(lparam));
          return FALSE;
        case 0x020A: // WM_MOUSEWHEEL, GET_WHEEL_DELTA_WPARAM(wparam);
          hgeImpl::pHGE->build_event(INPUT_MOUSEWHEEL, short(HIWORD(wparam)) / 120, 0, 0,
                                     LOWORDINT(lparam),
                                     HIWORDINT(lparam));
          return FALSE;

        case WM_SIZE:
          if (hgeImpl::pHGE->d3d_
              && wparam == SIZE_RESTORED) {
            hgeImpl::pHGE->resize(LOWORD(lparam), HIWORD(lparam));
          }
          //return FALSE;
          break;

        case WM_SYSCOMMAND:
          if (wparam == SC_CLOSE) {
            if (hgeImpl::pHGE->proc_exit_func_
                && !hgeImpl::pHGE->proc_exit_func_()) {
              return FALSE;
            }
            hgeImpl::pHGE->active_ = false;
            return DefWindowProc(hwnd, msg, wparam, lparam);
          }
          break;
        default:
          break;
      }

      return DefWindowProc(hwnd, msg, wparam, lparam);
    }

} // namespace

LRESULT CALLBACK WindowProc(HWND hwnd, const UINT msg, WPARAM wparam, LPARAM lparam) {
  return hgeImpl::pHGE->window_proc(hwnd, msg, wparam, lparam);
}

HGE *HGE_CALL hgeCreate(const int ver) {
  if (ver == HGE_VERSION) {
    return static_cast<HGE *>(hgeImpl::HGE_Impl::interface_get());
  }
  return nullptr;
}
