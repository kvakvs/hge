/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Core functions implementation: audio routines
//
#include "hge_impl.h"

#define BASSDEF(f) (WINAPI *f)        // define the functions as pointers

#include "BASS/bass.h"

#define LOADBASSFUNCTION(f) *((void**)&f)=(void*)GetProcAddress(hBass,#f)

namespace hgeImpl {

// NOLINTNEXTLINE
    HEFFECT HGE_CALL HGE_Impl::Effect_Load(const char *filename,
                                           const uint32_t size) {
      uint32_t size_1;
      BASS_CHANNELINFO info;
      void *data;

      if (!hBass) return 0;
      if (is_silent_) {
        return 1;
      }

      if (size) {
        data = const_cast<void *>(static_cast<const void *>(filename));
        size_1 = size;
      } else {
        data = Resource_Load(filename, &size_1);
        if (!data) {
          return NULL;
        }
      }

      auto hs = BASS_SampleLoad(TRUE, data, 0, size_1, 4, BASS_SAMPLE_OVER_VOL);
      if (!hs) {
        const auto hstrm = BASS_StreamCreateFile(TRUE, data, 0, size_1,
                                                 BASS_STREAM_DECODE);
        if (hstrm) {
          const auto length = static_cast<uint32_t>(BASS_ChannelGetLength(hstrm));
          BASS_ChannelGetInfo(hstrm, &info);
          auto samples = length;
          if (info.chans < 2) {
            samples >>= 1;
          }
          if ((info.flags & BASS_SAMPLE_8BITS) == 0) {
            samples >>= 1;
          }
          const auto buffer = BASS_SampleCreate(samples, info.freq, 2, 4,
                                                info.flags | BASS_SAMPLE_OVER_VOL);
          if (!buffer) {
            BASS_StreamFree(hstrm);
            post_error("Can't create sound effect: Not enough memory");
          } else {
            BASS_ChannelGetData(hstrm, buffer, length);
            hs = BASS_SampleCreateDone();
            BASS_StreamFree(hstrm);
            if (!hs) {
              post_error("Can't create sound effect");
            }
          }
        }
      }

      if (!size) {
        Resource_Free(data);
      }
      return hs;
    }

    HCHANNEL HGE_CALL HGE_Impl::Effect_Play(const HEFFECT eff) {
      if (!hBass) return 0;
      const auto chn = BASS_SampleGetChannel(eff, FALSE);
      BASS_ChannelPlay(chn, TRUE);
      return chn;
    }

// NOLINTNEXTLINE
    HCHANNEL HGE_CALL HGE_Impl::Effect_PlayEx(
            const HEFFECT eff, const int volume, const int pan,
            const float pitch, const bool loop) {
      if (!hBass) return 0;
      BASS_SAMPLE info;
      BASS_SampleGetInfo(eff, &info);

      const auto chn = BASS_SampleGetChannel(eff, FALSE);
      BASS_ChannelSetAttributes(chn, static_cast<int>(pitch * info.freq), volume, pan);

      info.flags &= ~BASS_SAMPLE_LOOP;
      if (loop) {
        info.flags |= BASS_SAMPLE_LOOP;
      }
      BASS_ChannelSetFlags(chn, info.flags);
      BASS_ChannelPlay(chn, TRUE);
      return chn;
    }


    void HGE_CALL HGE_Impl::Effect_Free(const HEFFECT eff) {
      if (!hBass) return;
      BASS_SampleFree(eff);
    }


// NOLINTNEXTLINE
    HMUSIC HGE_CALL HGE_Impl::Music_Load(
            const char *filename, const uint32_t size) {
      void *data;
      uint32_t size_1;

      if (!hBass) return 0;
      if (size) {
        data = const_cast<void *>(static_cast<const void *>(filename));
        //size_1 = size;
      } else {
        data = Resource_Load(filename, &size_1);
        if (!data) {
          return 0;
        }
      }

      const auto hm = BASS_MusicLoad(
              TRUE, data, 0, 0, BASS_MUSIC_PRESCAN | BASS_MUSIC_POSRESETEX | BASS_MUSIC_RAMP,
              0);
      if (!hm) {
        post_error("Can't load music");
      }
      if (!size) {
        Resource_Free(data);
      }
      return hm;
    }

// NOLINTNEXTLINE
    HCHANNEL HGE_CALL HGE_Impl::Music_Play(
            const HMUSIC mus, const bool loop,
            const int volume, int order, int row) {
      if (!hBass) return 0;
      auto pos = BASS_MusicGetOrderPosition(mus);
      if (order == -1) {
        order = LOWORD(pos);
      }
      if (row == -1) {
        row = HIWORD(pos);
      }
      BASS_ChannelSetPosition(mus, MAKEMUSICPOS(order, row));

      BASS_CHANNELINFO info;
      BASS_ChannelGetInfo(mus, &info);
      BASS_ChannelSetAttributes(mus, info.freq, volume, 0);

      info.flags &= ~BASS_SAMPLE_LOOP;
      if (loop) {
        info.flags |= BASS_SAMPLE_LOOP;
      }
      BASS_ChannelSetFlags(mus, info.flags);

      BASS_ChannelPlay(mus, FALSE);

      return mus;
    }

    void HGE_CALL HGE_Impl::Music_Free(const HMUSIC mus) {
      if (!hBass) return;
      BASS_MusicFree(mus);
    }

    void HGE_CALL HGE_Impl::Music_SetAmplification(const HMUSIC music, const int ampl) {
      if (!hBass) return;
      BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_AMPLIFY, ampl);
    }

    int HGE_CALL HGE_Impl::Music_GetAmplification(const HMUSIC music) {
      if (!hBass) return -1;
      return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_AMPLIFY);
    }

    int HGE_CALL HGE_Impl::Music_GetLength(const HMUSIC music) {
      if (!hBass) return -1;
      return BASS_MusicGetOrders(music);
    }

    void HGE_CALL HGE_Impl::Music_SetPos(const HMUSIC music, int order, int row) {
      if (!hBass) return;
      BASS_ChannelSetPosition(music, MAKEMUSICPOS(order, row));
    }

    bool HGE_CALL HGE_Impl::Music_GetPos(const HMUSIC music, int *order, int *row) {
      if (!hBass) return false;
      uint32_t pos;
      pos = BASS_MusicGetOrderPosition(music);
      if (pos == -1) {
        return false;
      }
      *order = LOWORD(pos);
      *row = HIWORD(pos);
      return true;
    }

    void HGE_CALL HGE_Impl::Music_SetInstrVolume(const HMUSIC music,
                                                 const int instr, const int volume) {
      if (!hBass) return;
      BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_VOL_INST + instr, volume);
    }

    int HGE_CALL HGE_Impl::Music_GetInstrVolume(const HMUSIC music, const int instr) {
      if (!hBass) return -1;
      return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_VOL_INST + instr);
    }

    void HGE_CALL HGE_Impl::Music_SetChannelVolume(const HMUSIC music,
                                                   const int channel, const int volume) {
      if (!hBass) return;
      BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_VOL_CHAN + channel, volume);
    }

    int HGE_CALL HGE_Impl::Music_GetChannelVolume(const HMUSIC music, const int channel) {
      if (!hBass) return -1;
      return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_VOL_CHAN + channel);
    }

// NOLINTNEXTLINE
    HSTREAM HGE_CALL HGE_Impl::Stream_Load(
            const char *filename, const uint32_t size) {
      void *data;
      uint32_t _size;

      if (!hBass) return 0;
      if (is_silent_) {
        return 1;
      }

      if (size) {
        data = const_cast<void *>(static_cast<const void *>(filename));
        _size = size;
      } else {
        data = Resource_Load(filename, &_size);
        if (!data) {
          return 0;
        }
      }
      const auto hs = BASS_StreamCreateFile(TRUE, data, 0, _size, 0);
      if (!hs) {
        post_error("Can't load stream");
        if (!size) {
          Resource_Free(data);
        }
        return 0;
      }
      if (!size) {
        const auto stmItem = new CStreamList;
        stmItem->hstream = hs;
        stmItem->data = data;
        stmItem->next = sound_streams_;
        sound_streams_ = stmItem;
      }
      return hs;
    }

    void HGE_CALL HGE_Impl::Stream_Free(const HSTREAM stream) {
      auto stm_item = sound_streams_;
      CStreamList *stm_prev = nullptr;

      if (!hBass) return;
      while (stm_item) {
        if (stm_item->hstream == stream) {
          if (stm_prev) {
            stm_prev->next = stm_item->next;
          } else {
            sound_streams_ = stm_item->next;
          }
          Resource_Free(stm_item->data);
          delete stm_item;
          break;
        }
        stm_prev = stm_item;
        stm_item = stm_item->next;
      }
      BASS_StreamFree(stream);
    }

// NOLINTNEXTLINE
    HCHANNEL HGE_CALL HGE_Impl::Stream_Play(
            const HSTREAM stream, const bool loop,
            const int volume) {
      if (!hBass) return 0;
      BASS_CHANNELINFO info;
      BASS_ChannelGetInfo(stream, &info);
      BASS_ChannelSetAttributes(stream, info.freq, volume, 0);

      info.flags &= ~BASS_SAMPLE_LOOP;
      if (loop) {
        info.flags |= BASS_SAMPLE_LOOP;
      }
      BASS_ChannelSetFlags(stream, info.flags);
      BASS_ChannelPlay(stream, TRUE);
      return stream;
    }

    void HGE_CALL HGE_Impl::Channel_SetPanning(const HCHANNEL chn, const int pan) {
      if (!hBass) return;
      BASS_ChannelSetAttributes(chn, -1, -1, pan);
    }

    void HGE_CALL HGE_Impl::Channel_SetVolume(const HCHANNEL chn, const int volume) {
      if (!hBass) return;
      BASS_ChannelSetAttributes(chn, -1, volume, -101);
    }

    void HGE_CALL HGE_Impl::Channel_SetPitch(const HCHANNEL chn, const float pitch) {
      if (!hBass) return;
      BASS_CHANNELINFO info;
      BASS_ChannelGetInfo(chn, &info);
      BASS_ChannelSetAttributes(chn, static_cast<int>(pitch * info.freq), -1, -101);
    }

    void HGE_CALL HGE_Impl::Channel_Pause(const HCHANNEL chn) {
      if (!hBass) return;
      BASS_ChannelPause(chn);
    }

    void HGE_CALL HGE_Impl::Channel_Resume(const HCHANNEL chn) {
      if (!hBass) return;
      BASS_ChannelPlay(chn, FALSE);
    }

    void HGE_CALL HGE_Impl::Channel_Stop(const HCHANNEL chn) {
      if (!hBass) return;
      BASS_ChannelStop(chn);
    }

    void HGE_CALL HGE_Impl::Channel_PauseAll() {
      if (!hBass) return;
      BASS_Pause();
    }

    void HGE_CALL HGE_Impl::Channel_ResumeAll() {
      if (!hBass) return;
      BASS_Start();
    }

    void HGE_CALL HGE_Impl::Channel_StopAll() {
      if (!hBass) return;
      BASS_Stop();
      BASS_Start();
    }

    bool HGE_CALL HGE_Impl::Channel_IsPlaying(const HCHANNEL chn) {
      if (!hBass) return false;
      if (BASS_ChannelIsActive(chn) == BASS_ACTIVE_PLAYING) {
        return true;
      }
      return false;
    }

    float HGE_CALL HGE_Impl::Channel_GetLength(const HCHANNEL chn) {
      if (!hBass) return -1;
      return BASS_ChannelBytes2Seconds(chn, BASS_ChannelGetLength(chn));
    }

    float HGE_CALL HGE_Impl::Channel_GetPos(const HCHANNEL chn) {
      if (!hBass) return -1;
      return BASS_ChannelBytes2Seconds(chn, BASS_ChannelGetPosition(chn));
    }

    void HGE_CALL HGE_Impl::Channel_SetPos(const HCHANNEL chn, const float seconds) {
      if (!hBass) return;
      BASS_ChannelSetPosition(chn, BASS_ChannelSeconds2Bytes(chn, seconds));
    }

// NOLINTNEXTLINE
    void HGE_CALL HGE_Impl::Channel_SlideTo(
            const HCHANNEL channel, const float time,
            const int volume, const int pan,
            const float pitch) {
      if (!hBass) return;
      BASS_CHANNELINFO info;
      BASS_ChannelGetInfo(channel, &info);

      int freq;
      if (pitch == -1) {
        freq = -1;
      } else {
        freq = static_cast<int>(pitch * info.freq);
      }

      BASS_ChannelSlideAttributes(channel, freq, volume, pan, uint32_t(time * 1000));
    }

    bool HGE_CALL HGE_Impl::Channel_IsSliding(const HCHANNEL channel) {
      if (hBass) {
        if (BASS_ChannelIsSliding(channel)) {
          return true;
        }
        return false;
      }
      return false;
    }


//////// Implementation ////////


    bool HGE_Impl::sound_init() {
      if (!use_sound_ || hBass) {
        return true;
      }

      hBass = LoadLibrary("bass.dll");
      if (!hBass) {
        post_error("Can't load BASS.DLL");
        return false;
      }

      LOADBASSFUNCTION(BASS_GetVersion);

      if (HIWORD(BASS_GetVersion()) != BASSVERSION) {
        post_error("Incorrect BASS.DLL version");
        return false;
      }

      LOADBASSFUNCTION(BASS_GetDeviceDescription);
      LOADBASSFUNCTION(BASS_Init);
      LOADBASSFUNCTION(BASS_Free);
      LOADBASSFUNCTION(BASS_Start);
      LOADBASSFUNCTION(BASS_Pause);
      LOADBASSFUNCTION(BASS_Stop);
      LOADBASSFUNCTION(BASS_SetConfig);
      //LOADBASSFUNCTION(BASS_ErrorGetCode);

      LOADBASSFUNCTION(BASS_SampleLoad);
      LOADBASSFUNCTION(BASS_SampleCreate);
      LOADBASSFUNCTION(BASS_SampleCreateDone);
      LOADBASSFUNCTION(BASS_SampleGetInfo);
      LOADBASSFUNCTION(BASS_SampleGetChannel);
      LOADBASSFUNCTION(BASS_SampleFree);

      LOADBASSFUNCTION(BASS_MusicLoad);
      LOADBASSFUNCTION(BASS_MusicFree);
      LOADBASSFUNCTION(BASS_MusicGetOrders);
      LOADBASSFUNCTION(BASS_MusicGetOrderPosition);
      LOADBASSFUNCTION(BASS_MusicSetAttribute);
      LOADBASSFUNCTION(BASS_MusicGetAttribute);

      LOADBASSFUNCTION(BASS_StreamCreateFile);
      LOADBASSFUNCTION(BASS_StreamFree);

      LOADBASSFUNCTION(BASS_ChannelGetInfo);
      LOADBASSFUNCTION(BASS_ChannelGetAttributes);
      LOADBASSFUNCTION(BASS_ChannelSetAttributes);
      LOADBASSFUNCTION(BASS_ChannelSlideAttributes);
      LOADBASSFUNCTION(BASS_ChannelIsSliding);
      LOADBASSFUNCTION(BASS_ChannelSetFlags);
      LOADBASSFUNCTION(BASS_ChannelGetData);
      LOADBASSFUNCTION(BASS_ChannelPlay);
      LOADBASSFUNCTION(BASS_ChannelPause);
      LOADBASSFUNCTION(BASS_ChannelStop);
      LOADBASSFUNCTION(BASS_ChannelIsActive);
      LOADBASSFUNCTION(BASS_ChannelGetLength);
      LOADBASSFUNCTION(BASS_ChannelGetPosition);
      LOADBASSFUNCTION(BASS_ChannelSetPosition);
      LOADBASSFUNCTION(BASS_ChannelSeconds2Bytes);
      LOADBASSFUNCTION(BASS_ChannelBytes2Seconds);

      is_silent_ = false;
      if (!BASS_Init(-1, sample_rate_, 0, hwnd_, nullptr)) {
        System_Log("BASS Init failed, using no sound");
        BASS_Init(0, sample_rate_, 0, hwnd_, nullptr);
        is_silent_ = true;
      } else {
        System_Log("Sound Device: %s", BASS_GetDeviceDescription(1));
        System_Log("Sample rate: %ld\n", sample_rate_);
      }

      //BASS_SetConfig(BASS_CONFIG_BUFFER, 5000);
      //BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 50);

      set_fx_volume(fx_volume_);
      set_mus_volume(mus_volume_);
      set_stream_volume(stream_volume_);

      return true;
    }

    void HGE_Impl::sound_done() {
      auto stm_item = sound_streams_;

      if (hBass) {
        BASS_Stop();
        BASS_Free();

        //int err = BASS_ErrorGetCode();

        FreeLibrary(hBass);
        hBass = nullptr;

        while (stm_item) {
          const auto stm_next = stm_item->next;
          Resource_Free(stm_item->data);
          delete stm_item;
          stm_item = stm_next;
        }
        sound_streams_ = nullptr;
      }
    }

    void HGE_Impl::set_mus_volume(const int vol) const {
      if (!hBass) return;
      BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC, vol);
    }

    void HGE_Impl::set_stream_volume(const int vol) const {
      if (!hBass) return;
      BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, vol);
    }

    void HGE_Impl::set_fx_volume(const int vol) const {
      if (!hBass) return;
      BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, vol);
    }

} // namespace
