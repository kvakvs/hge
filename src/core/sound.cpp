/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: audio routines
*/


#include "hge_impl.h"


#define BASSDEF(f) (WINAPI *f)	// define the functions as pointers
#include "BASS\bass.h"

#define LOADBASSFUNCTION(f) *((void**)&f)=(void*)GetProcAddress(m_bass_dll,#f)


HEFFECT HGE_CALL HGE_Impl::Effect_Load(hgeConstString filename, uint32_t size)
{
	uint32_t _size, length, samples;
	HSAMPLE hs;
	HSTREAM hstrm;
	BASS_CHANNELINFO info;
	void *buffer, *data;

	if (m_bass_dll)
	{
		if (m_nosound_flag)
			return 1;

		if (size)
		{
			data = (void *) filename;
			_size = size;
		}
		else
		{
			data = Resource_Load(filename, &_size);
			if (!data)
				return NULL;
		}

		hs = BASS_SampleLoad(TRUE, data, 0, _size, 4, BASS_SAMPLE_OVER_VOL);
		if (!hs)
		{
			hstrm = BASS_StreamCreateFile(TRUE, data, 0, _size,
					BASS_STREAM_DECODE);
			if (hstrm)
			{
				length = (uint32_t) BASS_ChannelGetLength(hstrm);
				BASS_ChannelGetInfo(hstrm, &info);
				samples = length;
				if (info.chans < 2)
					samples >>= 1;
				if ((info.flags & BASS_SAMPLE_8BITS) == 0)
					samples >>= 1;
				buffer = BASS_SampleCreate(samples, info.freq, 2, 4, info.flags
						| BASS_SAMPLE_OVER_VOL);
				if (!buffer)
				{
					BASS_StreamFree(hstrm);
					_PostError(
							TXT("Can't create sound effect: Not enough memory"));
				}
				else
				{
					BASS_ChannelGetData(hstrm, buffer, length);
					hs = BASS_SampleCreateDone();
					BASS_StreamFree(hstrm);
					if (!hs)
						_PostError(TXT("Can't create sound effect"));
				}
			}
		}

		if (!size)
			Resource_Free(data);
		return hs;
	}
	else
		return 0;
}



HCHANNEL HGE_CALL HGE_Impl::Effect_Play(HEFFECT eff)
{
	if (m_bass_dll)
	{
		HCHANNEL chn;
		chn = BASS_SampleGetChannel(eff, FALSE);
		BASS_ChannelPlay(chn, TRUE);
		return chn;
	}
	else
		return 0;
}



HCHANNEL HGE_CALL HGE_Impl::Effect_PlayEx(HEFFECT eff, int volume, int pan,
		float pitch, bool loop)
{
	if (m_bass_dll)
	{
		BASS_SAMPLE info;
		HCHANNEL chn;
		BASS_SampleGetInfo(eff, &info);

		chn = BASS_SampleGetChannel(eff, FALSE);
		BASS_ChannelSetAttributes(chn, (int) (pitch * info.freq), volume, pan);

		info.flags &= ~BASS_SAMPLE_LOOP;
		if (loop)
			info.flags |= BASS_SAMPLE_LOOP;
		BASS_ChannelSetFlags(chn, info.flags);
		BASS_ChannelPlay(chn, TRUE);
		return chn;
	}
	else
		return 0;
}



void HGE_CALL HGE_Impl::Effect_Free(HEFFECT eff)
{
	if (m_bass_dll)
		BASS_SampleFree(eff);
}



HMUSIC HGE_CALL HGE_Impl::Music_Load(hgeConstString filename, uint32_t size)
{
	void *data;
	uint32_t _size;
	HMUSIC hm;

	if (m_bass_dll)
	{
		if (size)
		{
			data = (void *) filename;
			_size = size;
		}
		else
		{
			data = Resource_Load(filename, &_size);
			if (!data)
				return 0;
		}

		hm = BASS_MusicLoad(TRUE, data, 0, 0, BASS_MUSIC_PRESCAN
				| BASS_MUSIC_POSRESETEX | BASS_MUSIC_RAMP, 0);
		if (!hm)
			_PostError(TXT("Can't load music"));
		if (!size)
			Resource_Free(data);
		return hm;
	}
	else
		return 0;
}



HCHANNEL HGE_CALL HGE_Impl::Music_Play(HMUSIC mus, bool loop, int volume,
		int order, int row)
{
	if (m_bass_dll)
	{
		uint32_t pos = BASS_MusicGetOrderPosition(mus);
		if (order == -1)
			order = LOWORD(pos);
		if (row == -1)
			row = HIWORD(pos);
		BASS_ChannelSetPosition(mus, MAKEMUSICPOS(order, row));

		BASS_CHANNELINFO info;
		BASS_ChannelGetInfo(mus, &info);
		BASS_ChannelSetAttributes(mus, info.freq, volume, 0);

		info.flags &= ~BASS_SAMPLE_LOOP;
		if (loop)
			info.flags |= BASS_SAMPLE_LOOP;
		BASS_ChannelSetFlags(mus, info.flags);

		BASS_ChannelPlay(mus, FALSE);

		return mus;
	}
	else
		return 0;
}



void HGE_CALL HGE_Impl::Music_Free(HMUSIC mus)
{
	if (m_bass_dll)
		BASS_MusicFree(mus);
}



void HGE_CALL HGE_Impl::Music_SetAmplification(HMUSIC music, int ampl)
{
	if (m_bass_dll)
		BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_AMPLIFY, ampl);
}



int HGE_CALL HGE_Impl::Music_GetAmplification(HMUSIC music)
{
	if (m_bass_dll)
		return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_AMPLIFY);
	else
		return -1;
}



int HGE_CALL HGE_Impl::Music_GetLength(HMUSIC music)
{
	if (m_bass_dll)
	{
		return BASS_MusicGetOrders(music);
	}
	else
		return -1;
}



void HGE_CALL HGE_Impl::Music_SetPos(HMUSIC music, int order, int row)
{
	if (m_bass_dll)
	{
		BASS_ChannelSetPosition(music, MAKEMUSICPOS(order, row));
	}
}



bool HGE_CALL HGE_Impl::Music_GetPos(HMUSIC music, int *order, int *row)
{
	if (m_bass_dll)
	{
		uint32_t pos;
		pos = BASS_MusicGetOrderPosition(music);
		if (pos == -1)
			return false;
		*order = LOWORD(pos);
		*row = HIWORD(pos);
		return true;
	}
	else
		return false;
}



void HGE_CALL HGE_Impl::Music_SetInstrVolume(HMUSIC music, int instr,
		int volume)
{
	if (m_bass_dll)
	{
		BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_VOL_INST + instr,
				volume);
	}
}



int HGE_CALL HGE_Impl::Music_GetInstrVolume(HMUSIC music, int instr)
{
	if (m_bass_dll)
	{
		return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_VOL_INST + instr);
	}
	else
		return -1;
}



void HGE_CALL HGE_Impl::Music_SetChannelVolume(HMUSIC music, int channel,
		int volume)
{
	if (m_bass_dll)
	{
		BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_VOL_CHAN + channel,
				volume);
	}
}



int HGE_CALL HGE_Impl::Music_GetChannelVolume(HMUSIC music, int channel)
{
	if (m_bass_dll)
	{
		return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_VOL_CHAN
				+ channel);
	}
	else
		return -1;
}



HSTREAM HGE_CALL HGE_Impl::Stream_Load(hgeConstString filename, uint32_t size)
{
	void *data;
	uint32_t _size;
	HSTREAM hs;
	CStreamList *stmItem;

	if (m_bass_dll)
	{
		if (m_nosound_flag)
			return 1;

		if (size)
		{
			data = (void *) filename;
			_size = size;
		}
		else
		{
			data = Resource_Load(filename, &_size);
			if (!data)
				return 0;
		}
		hs = BASS_StreamCreateFile(TRUE, data, 0, _size, 0);
		if (!hs)
		{
			_PostError(TXT("Can't load stream"));
			if (!size)
				Resource_Free(data);
			return 0;
		}
		if (!size)
		{
			stmItem = new CStreamList;
			stmItem->hstream = hs;
			stmItem->data = data;
			stmItem->next = m_streams;
			m_streams = stmItem;
		}
		return hs;
	}
	else
		return 0;
}



void HGE_CALL HGE_Impl::Stream_Free(HSTREAM stream)
{
	CStreamList *stmItem = m_streams, *stmPrev = 0;

	if (m_bass_dll)
	{
		while (stmItem)
		{
			if (stmItem->hstream == stream)
			{
				if (stmPrev)
					stmPrev->next = stmItem->next;
				else
					m_streams = stmItem->next;
				Resource_Free(stmItem->data);
				delete stmItem;
				break;
			}
			stmPrev = stmItem;
			stmItem = stmItem->next;
		}
		BASS_StreamFree(stream);
	}
}



HCHANNEL HGE_CALL HGE_Impl::Stream_Play(HSTREAM stream, bool loop, int volume)
{
	if (m_bass_dll)
	{
		BASS_CHANNELINFO info;
		BASS_ChannelGetInfo(stream, &info);
		BASS_ChannelSetAttributes(stream, info.freq, volume, 0);

		info.flags &= ~BASS_SAMPLE_LOOP;
		if (loop)
			info.flags |= BASS_SAMPLE_LOOP;
		BASS_ChannelSetFlags(stream, info.flags);
		BASS_ChannelPlay(stream, TRUE);
		return stream;
	}
	else
		return 0;
}



void HGE_CALL HGE_Impl::Channel_SetPanning(HCHANNEL chn, int pan)
{
	if (m_bass_dll)
		BASS_ChannelSetAttributes(chn, -1, -1, pan);
}



void HGE_CALL HGE_Impl::Channel_SetVolume(HCHANNEL chn, int volume)
{
	if (m_bass_dll)
		BASS_ChannelSetAttributes(chn, -1, volume, -101);
}



void HGE_CALL HGE_Impl::Channel_SetPitch(HCHANNEL chn, float pitch)
{
	if (m_bass_dll)
	{
		BASS_CHANNELINFO info;
		BASS_ChannelGetInfo(chn, &info);
		BASS_ChannelSetAttributes(chn, (int) (pitch * info.freq), -1, -101);
	}
}



void HGE_CALL HGE_Impl::Channel_Pause(HCHANNEL chn)
{
	if (m_bass_dll)
		BASS_ChannelPause(chn);
}



void HGE_CALL HGE_Impl::Channel_Resume(HCHANNEL chn)
{
	if (m_bass_dll)
		BASS_ChannelPlay(chn, FALSE);
}



void HGE_CALL HGE_Impl::Channel_Stop(HCHANNEL chn)
{
	if (m_bass_dll)
		BASS_ChannelStop(chn);
}



void HGE_CALL HGE_Impl::Channel_PauseAll()
{
	if (m_bass_dll)
		BASS_Pause();
}



void HGE_CALL HGE_Impl::Channel_ResumeAll()
{
	if (m_bass_dll)
		BASS_Start();
}



void HGE_CALL HGE_Impl::Channel_StopAll()
{
	if (m_bass_dll)
	{
		BASS_Stop();
		BASS_Start();
	}
}



bool HGE_CALL HGE_Impl::Channel_IsPlaying(HCHANNEL chn)
{
	if (m_bass_dll)
	{
		if (BASS_ChannelIsActive(chn) == BASS_ACTIVE_PLAYING)
			return true;
		else
			return false;
	}
	else
		return false;
}



float HGE_CALL HGE_Impl::Channel_GetLength(HCHANNEL chn)
{
	if (m_bass_dll)
	{
		return BASS_ChannelBytes2Seconds(chn, BASS_ChannelGetLength(chn));
	}
	else
		return -1;
}



float HGE_CALL HGE_Impl::Channel_GetPos(HCHANNEL chn)
{
	if (m_bass_dll)
	{
		return BASS_ChannelBytes2Seconds(chn, BASS_ChannelGetPosition(chn));
	}
	else
		return -1;
}



void HGE_CALL HGE_Impl::Channel_SetPos(HCHANNEL chn, float fSeconds)
{
	if (m_bass_dll)
	{
		BASS_ChannelSetPosition(chn, BASS_ChannelSeconds2Bytes(chn, fSeconds));
	}
}



void HGE_CALL HGE_Impl::Channel_SlideTo(HCHANNEL channel, float time,
		int volume, int pan, float pitch)
{
	if (m_bass_dll)
	{
		BASS_CHANNELINFO info;
		BASS_ChannelGetInfo(channel, &info);

		int freq;
		if (pitch == -1)
			freq = -1;
		else
			freq = (int) (pitch * info.freq);

		BASS_ChannelSlideAttributes(channel, freq, volume, pan, uint32_t(time * 1000));
	}
}



bool HGE_CALL HGE_Impl::Channel_IsSliding(HCHANNEL channel)
{
	if (m_bass_dll)
	{
		if (BASS_ChannelIsSliding(channel))
			return true;
		else
			return false;
	}
	else
		return false;
}


//////// Implementation ////////


bool HGE_Impl::_SoundInit()
{
	if (!m_use_sound_flag || m_bass_dll)
		return true;

	m_bass_dll = LoadLibrary("bass.dll");
	if (!m_bass_dll)
	{
		_PostError(TXT("Can't load BASS.DLL"));
		return false;
	}

	LOADBASSFUNCTION(BASS_GetVersion);

	if (HIWORD(BASS_GetVersion()) != BASSVERSION)
	{
		_PostError(TXT("Incorrect BASS.DLL version"));
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

	m_nosound_flag = false;
	if (!BASS_Init(-1, m_sample_rate, 0, m_hwnd, NULL))
	{
		System_Log(TXT("BASS Init failed, using no sound"));
		BASS_Init(0, m_sample_rate, 0, m_hwnd, NULL);
		m_nosound_flag = true;
	}
	else
	{
		System_Log(TXT("Sound Device: %s"), BASS_GetDeviceDescription(1));
		System_Log(TXT("Sample rate: %ld\n"), m_sample_rate);
	}

	//BASS_SetConfig(BASS_CONFIG_BUFFER, 5000);
	//BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 50);

	_SetFXVolume(m_fx_volume);
	_SetMusVolume(m_mus_volume);
	_SetStreamVolume(m_stream_volume);

	return true;
}



void HGE_Impl::_SoundDone()
{
	CStreamList *stmItem = m_streams, *stmNext;

	if (m_bass_dll)
	{
		BASS_Stop();
		BASS_Free();

		//int err = BASS_ErrorGetCode(); 

		FreeLibrary(m_bass_dll);
		m_bass_dll = 0;

		while (stmItem)
		{
			stmNext = stmItem->next;
			Resource_Free(stmItem->data);
			delete stmItem;
			stmItem = stmNext;
		}
		m_streams = 0;
	}
}



void HGE_Impl::_SetMusVolume(int vol)
{
	if (m_bass_dll)
		BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC, vol);
}



void HGE_Impl::_SetStreamVolume(int vol)
{
	if (m_bass_dll)
		BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, vol);
}



void HGE_Impl::_SetFXVolume(int vol)
{
	if (m_bass_dll)
		BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, vol);
}






