/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Particle systems editor
//

#include "particleed.h"
#include <windows.h>
#include <string>

using namespace std;

bool handle_keys(const int key) {
  switch (key) {
    case 0:
      return false;

    case HGEK_ESCAPE:
      cmdSavePreset(state.nPreset);
      return true;

    case HGEK_TAB:
      state.bIFace = !state.bIFace;
      break;

    case HGEK_LBUTTON:
      if (state.mx >= 168 && state.mx <= 631) {
        state.ps->Fire();
      }
      break;

    case HGEK_SPACE:
      state.ps->Fire();
      break;

    case HGEK_1:
    case HGEK_2:
    case HGEK_3:
    case HGEK_4:
    case HGEK_5:
    case HGEK_6:
    case HGEK_7:
    case HGEK_8:
    case HGEK_9:
      cmdChangePreset(key - HGEK_1);
      break;
  }

  return false;
}

bool do_commands(const int id) {
  switch (id) {
    // Presets & stuff

    case 0:
      return false;
    case CMD_EXIT:
      cmdSavePreset(state.nPreset);
      return true;
    case CMD_HELP:
      state.bHelp = BUTTON_GET_STATE(CMD_HELP);
      break;
    case CMD_BOUNDINGBOX:
      state.bBBox = BUTTON_GET_STATE(CMD_BOUNDINGBOX);
      break;

    case CMD_PRESET1:
    case CMD_PRESET2:
    case CMD_PRESET3:
    case CMD_PRESET4:
    case CMD_PRESET5:
    case CMD_PRESET6:
    case CMD_PRESET7:
    case CMD_PRESET8:
    case CMD_PRESET9:
      cmdChangePreset(id - CMD_PRESET1);
      break;

      // System parameters

    case CMD_SYS_LIFETIME:
      state.ps->info.fLifetime = SLIDER_GET_VALUE(CMD_SYS_LIFETIME);
      GET_TEXT_CTRL(CMD_SYS_TLIFETIME)->printf("%.1f", state.ps->info.fLifetime);
      BUTTON_SET_STATE(CMD_SYS_LIFECONT, false);
      state.ps->Stop();
      break;

    case CMD_SYS_LIFECONT:
      if (BUTTON_GET_STATE(CMD_SYS_LIFECONT)) {
        state.ps->info.fLifetime = -1.0f;
        state.ps->Fire();
      } else {
        state.ps->info.fLifetime = SLIDER_GET_VALUE(CMD_SYS_LIFETIME);
        state.ps->Stop();
      }
      break;

    case CMD_SYS_EMISSION:
      state.ps->info.nEmission = int(SLIDER_GET_VALUE(CMD_SYS_EMISSION));
      GET_TEXT_CTRL(CMD_SYS_TEMISSION)->printf("%d", state.ps->info.nEmission);
      break;

    case CMD_SYS_PARLIFETIMELOCK:
      if (BUTTON_GET_STATE(CMD_SYS_PARLIFETIMELOCK)) {
        SLIDER_SET_VALUE(CMD_SYS_PARLIFETIMEMAX, SLIDER_GET_VALUE(CMD_SYS_PARLIFETIMEMIN));
        state.ps->info.fParticleLifeMax = state.ps->info.fParticleLifeMin;
      }
      break;

    case CMD_SYS_PARLIFETIMEMIN:
      state.ps->info.fParticleLifeMin = SLIDER_GET_VALUE(CMD_SYS_PARLIFETIMEMIN);
      if (BUTTON_GET_STATE(CMD_SYS_PARLIFETIMELOCK)) {
        SLIDER_SET_VALUE(CMD_SYS_PARLIFETIMEMAX, state.ps->info.fParticleLifeMin);
        state.ps->info.fParticleLifeMax = state.ps->info.fParticleLifeMin;
      }
      break;

    case CMD_SYS_PARLIFETIMEMAX:
      state.ps->info.fParticleLifeMax = SLIDER_GET_VALUE(CMD_SYS_PARLIFETIMEMAX);
      if (BUTTON_GET_STATE(CMD_SYS_PARLIFETIMELOCK)) {
        SLIDER_SET_VALUE(CMD_SYS_PARLIFETIMEMIN, state.ps->info.fParticleLifeMax);
        state.ps->info.fParticleLifeMin = state.ps->info.fParticleLifeMax;
      }
      break;

    case CMD_SYS_TEXTURE:
      sprParticles->SetFrame(sprParticles->GetFrame() + 1);
      break;

    case CMD_SYS_BLENDADDITIVE:
      BUTTON_SET_STATE(CMD_SYS_BLENDADDITIVE, true);
      BUTTON_SET_STATE(CMD_SYS_BLENDBLEND, false);
      state.ps->info.sprite->SetBlendMode(
              (hgeBlendMode) (BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_ZWRITE));
      break;

    case CMD_SYS_BLENDBLEND:
      BUTTON_SET_STATE(CMD_SYS_BLENDADDITIVE, false);
      BUTTON_SET_STATE(CMD_SYS_BLENDBLEND, true);
      state.ps->info.sprite->SetBlendMode(
              (hgeBlendMode) (BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_ZWRITE));
      break;

      // Particle movement

    case CMD_PM_DIRECTION:
      state.ps->info.fDirection = SLIDER_GET_VALUE(CMD_PM_DIRECTION);
      GET_TEXT_CTRL(CMD_PM_TDIRECTION)->printf("%d", int(state.ps->info.fDirection * 180 / M_PI));
      break;

    case CMD_PM_RELATIVE:
      state.ps->info.bRelative = BUTTON_GET_STATE(CMD_PM_RELATIVE);
      break;

    case CMD_PM_SPREAD:
      state.ps->info.fSpread = SLIDER_GET_VALUE(CMD_PM_SPREAD);
      GET_TEXT_CTRL(CMD_PM_TSPREAD)->printf("%d", int(state.ps->info.fSpread * 180 / M_PI));
      break;

    case CMD_PM_STARTSPEEDLOCK:
      if (BUTTON_GET_STATE(CMD_PM_STARTSPEEDLOCK)) {
        SLIDER_SET_VALUE(CMD_PM_STARTSPEEDMAX, SLIDER_GET_VALUE(CMD_PM_STARTSPEEDMIN));
        state.ps->info.fSpeedMax = state.ps->info.fSpeedMin;
      }
      break;

    case CMD_PM_STARTSPEEDMIN:
      state.ps->info.fSpeedMin = SLIDER_GET_VALUE(CMD_PM_STARTSPEEDMIN);
      if (BUTTON_GET_STATE(CMD_PM_STARTSPEEDLOCK)) {
        SLIDER_SET_VALUE(CMD_PM_STARTSPEEDMAX, state.ps->info.fSpeedMin);
        state.ps->info.fSpeedMax = state.ps->info.fSpeedMin;
      }
      break;

    case CMD_PM_STARTSPEEDMAX:
      state.ps->info.fSpeedMax = SLIDER_GET_VALUE(CMD_PM_STARTSPEEDMAX);
      if (BUTTON_GET_STATE(CMD_PM_STARTSPEEDLOCK)) {
        SLIDER_SET_VALUE(CMD_PM_STARTSPEEDMIN, state.ps->info.fSpeedMax);
        state.ps->info.fSpeedMin = state.ps->info.fSpeedMax;
      }
      break;

    case CMD_PM_GRAVITYLOCK:
      if (BUTTON_GET_STATE(CMD_PM_GRAVITYLOCK)) {
        SLIDER_SET_VALUE(CMD_PM_GRAVITYMAX, SLIDER_GET_VALUE(CMD_PM_GRAVITYMIN));
        state.ps->info.fGravityMax = state.ps->info.fGravityMin;
      }
      break;

    case CMD_PM_GRAVITYMIN:
      state.ps->info.fGravityMin = SLIDER_GET_VALUE(CMD_PM_GRAVITYMIN);
      if (BUTTON_GET_STATE(CMD_PM_GRAVITYLOCK)) {
        SLIDER_SET_VALUE(CMD_PM_GRAVITYMAX, state.ps->info.fGravityMin);
        state.ps->info.fGravityMax = state.ps->info.fGravityMin;
      }
      break;

    case CMD_PM_GRAVITYMAX:
      state.ps->info.fGravityMax = SLIDER_GET_VALUE(CMD_PM_GRAVITYMAX);
      if (BUTTON_GET_STATE(CMD_PM_GRAVITYLOCK)) {
        SLIDER_SET_VALUE(CMD_PM_GRAVITYMIN, state.ps->info.fGravityMax);
        state.ps->info.fGravityMin = state.ps->info.fGravityMax;
      }
      break;

    case CMD_PM_RADIALLOCK:
      if (BUTTON_GET_STATE(CMD_PM_RADIALLOCK)) {
        SLIDER_SET_VALUE(CMD_PM_RADIALMAX, SLIDER_GET_VALUE(CMD_PM_RADIALMIN));
        state.ps->info.fRadialAccelMax = state.ps->info.fRadialAccelMin;
      }
      break;

    case CMD_PM_RADIALMIN:
      state.ps->info.fRadialAccelMin = SLIDER_GET_VALUE(CMD_PM_RADIALMIN);
      if (BUTTON_GET_STATE(CMD_PM_RADIALLOCK)) {
        SLIDER_SET_VALUE(CMD_PM_RADIALMAX, state.ps->info.fRadialAccelMin);
        state.ps->info.fRadialAccelMax = state.ps->info.fRadialAccelMin;
      }
      break;

    case CMD_PM_RADIALMAX:
      state.ps->info.fRadialAccelMax = SLIDER_GET_VALUE(CMD_PM_RADIALMAX);
      if (BUTTON_GET_STATE(CMD_PM_RADIALLOCK)) {
        SLIDER_SET_VALUE(CMD_PM_RADIALMIN, state.ps->info.fRadialAccelMax);
        state.ps->info.fRadialAccelMin = state.ps->info.fRadialAccelMax;
      }
      break;

    case CMD_PM_TANGENTIALLOCK:
      if (BUTTON_GET_STATE(CMD_PM_TANGENTIALLOCK)) {
        SLIDER_SET_VALUE(CMD_PM_TANGENTIALMAX, SLIDER_GET_VALUE(CMD_PM_TANGENTIALMIN));
        state.ps->info.fTangentialAccelMax = state.ps->info.fTangentialAccelMin;
      }
      break;

    case CMD_PM_TANGENTIALMIN:
      state.ps->info.fTangentialAccelMin = SLIDER_GET_VALUE(CMD_PM_TANGENTIALMIN);
      if (BUTTON_GET_STATE(CMD_PM_TANGENTIALLOCK)) {
        SLIDER_SET_VALUE(CMD_PM_TANGENTIALMAX, state.ps->info.fTangentialAccelMin);
        state.ps->info.fTangentialAccelMax = state.ps->info.fTangentialAccelMin;
      }
      break;

    case CMD_PM_TANGENTIALMAX:
      state.ps->info.fTangentialAccelMax = SLIDER_GET_VALUE(CMD_PM_TANGENTIALMAX);
      if (BUTTON_GET_STATE(CMD_PM_TANGENTIALLOCK)) {
        SLIDER_SET_VALUE(CMD_PM_TANGENTIALMIN, state.ps->info.fTangentialAccelMax);
        state.ps->info.fTangentialAccelMin = state.ps->info.fTangentialAccelMax;
      }
      break;

      // Particle appearance

    case CMD_PA_SIZELOCK:
      if (BUTTON_GET_STATE(CMD_PA_SIZELOCK)) {
        SLIDER_SET_VALUE(CMD_PA_SIZEEND, SLIDER_GET_VALUE(CMD_PA_SIZESTART));
        state.ps->info.fSizeEnd = state.ps->info.fSizeStart;
      }
      break;

    case CMD_PA_SIZESTART:
      state.ps->info.fSizeStart = SLIDER_GET_VALUE(CMD_PA_SIZESTART) / PARTICLE_SIZE;
      if (BUTTON_GET_STATE(CMD_PA_SIZELOCK)) {
        SLIDER_SET_VALUE(CMD_PA_SIZEEND, state.ps->info.fSizeStart * PARTICLE_SIZE);
        state.ps->info.fSizeEnd = state.ps->info.fSizeStart;
      }
      break;

    case CMD_PA_SIZEEND:
      state.ps->info.fSizeEnd = SLIDER_GET_VALUE(CMD_PA_SIZEEND) / PARTICLE_SIZE;
      if (BUTTON_GET_STATE(CMD_PA_SIZELOCK)) {
        SLIDER_SET_VALUE(CMD_PA_SIZESTART, state.ps->info.fSizeEnd * PARTICLE_SIZE);
        state.ps->info.fSizeStart = state.ps->info.fSizeEnd;
      }
      break;

    case CMD_PA_SIZEVAR:
      state.ps->info.fSizeVar = SLIDER_GET_VALUE(CMD_PA_SIZEVAR);
      break;

    case CMD_PA_SPINLOCK:
      if (BUTTON_GET_STATE(CMD_PA_SPINLOCK)) {
        SLIDER_SET_VALUE(CMD_PA_SPINEND, SLIDER_GET_VALUE(CMD_PA_SPINSTART));
        state.ps->info.fSpinEnd = state.ps->info.fSpinStart;
      }
      break;

    case CMD_PA_SPINSTART:
      state.ps->info.fSpinStart = SLIDER_GET_VALUE(CMD_PA_SPINSTART);
      if (BUTTON_GET_STATE(CMD_PA_SPINLOCK)) {
        SLIDER_SET_VALUE(CMD_PA_SPINEND, state.ps->info.fSpinStart);
        state.ps->info.fSpinEnd = state.ps->info.fSpinStart;
      }
      break;

    case CMD_PA_SPINEND:
      state.ps->info.fSpinEnd = SLIDER_GET_VALUE(CMD_PA_SPINEND);
      if (BUTTON_GET_STATE(CMD_PA_SPINLOCK)) {
        SLIDER_SET_VALUE(CMD_PA_SPINSTART, state.ps->info.fSpinEnd);
        state.ps->info.fSpinStart = state.ps->info.fSpinEnd;
      }
      break;

    case CMD_PA_SPINVAR:
      state.ps->info.fSpinVar = SLIDER_GET_VALUE(CMD_PA_SPINVAR);
      break;

    case CMD_PA_ALPHALOCK:
      if (BUTTON_GET_STATE(CMD_PA_ALPHALOCK)) {
        SLIDER_SET_VALUE(CMD_PA_ALPHAEND, SLIDER_GET_VALUE(CMD_PA_ALPHASTART));
        state.ps->info.colColorEnd.a = state.ps->info.colColorStart.a;
      }
      break;

    case CMD_PA_ALPHASTART:
      state.ps->info.colColorStart.a = SLIDER_GET_VALUE(CMD_PA_ALPHASTART);
      if (BUTTON_GET_STATE(CMD_PA_ALPHALOCK)) {
        SLIDER_SET_VALUE(CMD_PA_ALPHAEND, state.ps->info.colColorStart.a);
        state.ps->info.colColorEnd.a = state.ps->info.colColorStart.a;
      }
      break;

    case CMD_PA_ALPHAEND:
      state.ps->info.colColorEnd.a = SLIDER_GET_VALUE(CMD_PA_ALPHAEND);
      if (BUTTON_GET_STATE(CMD_PA_ALPHALOCK)) {
        SLIDER_SET_VALUE(CMD_PA_ALPHASTART, state.ps->info.colColorEnd.a);
        state.ps->info.colColorStart.a = state.ps->info.colColorEnd.a;
      }
      break;

    case CMD_PA_ALPHAVAR:
      state.ps->info.fAlphaVar = SLIDER_GET_VALUE(CMD_PA_ALPHAVAR);
      break;

    case CMD_PA_RSTART:
    case CMD_PA_GSTART:
    case CMD_PA_BSTART:
    case CMD_PA_REND:
    case CMD_PA_GEND:
    case CMD_PA_BEND:
      cmdChangeColor(id);
      break;

    case CMD_PA_RGBVAR:
      state.ps->info.fColorVar = SLIDER_GET_VALUE(CMD_PA_RGBVAR);
      break;

  }

  return false;
}

void cmdChangePreset(int n) {
  state.ps->Stop();
  BUTTON_SET_STATE(CMD_PRESET1 + state.nPreset, false);
  BUTTON_SET_STATE(CMD_PRESET1 + n, true);
  cmdSavePreset(state.nPreset);
  cmdLoadPreset(n);
  state.nPreset = n;
}

void cmdSavePreset(const int n) {
  DWORD size;
  char filename[_MAX_PATH];

  if (!state.ps) {
    return;
  }

  ZeroMemory(filename, _MAX_PATH);
  GetModuleFileName(GetModuleHandle(nullptr), filename, _MAX_PATH);
  string s(filename);
  s = s.substr(0, s.rfind('\\')) + "\\" + "particle" + char('1' + n) + ".psi";
  strcpy(filename, s.c_str());

  state.ps->info.sprite = reinterpret_cast<hgeSprite *>(
          sprParticles->GetFrame() | sprParticles->GetBlendMode() << 16);
  const auto h_f = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
  if (h_f == INVALID_HANDLE_VALUE) {
    return;
  }
  WriteFile(h_f, &state.ps->info, sizeof(hgeParticleSystemInfo), &size, nullptr);
  CloseHandle(h_f);
  state.ps->info.sprite = sprParticles;
}

void cmdLoadPreset(const int n) {
  DWORD size;
  char filename[_MAX_PATH];

  if (!state.ps) {
    return;
  }

  ZeroMemory(filename, _MAX_PATH);
  GetModuleFileName(GetModuleHandle(nullptr), filename, _MAX_PATH);
  string s(filename);
  s = s.substr(0, s.rfind('\\')) + "\\" + "particle" + char('1' + n) + ".psi";
  strcpy(filename, s.c_str());

  const auto h_f = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
  if (h_f == INVALID_HANDLE_VALUE) {
    return;
  }
  ReadFile(h_f, &state.ps->info, sizeof(hgeParticleSystemInfo), &size, nullptr);
  CloseHandle(h_f);
  sprParticles->SetFrame(reinterpret_cast<uint32_t>(state.ps->info.sprite) & 0xFFFF);
  sprParticles->SetBlendMode((hgeBlendMode)(reinterpret_cast<uint32_t>(state.ps->info.sprite) >> 16));
  state.ps->info.sprite = sprParticles;

  // System parameters

  if (state.ps->info.fLifetime == -1.0f) {
    SLIDER_SET_VALUE(CMD_SYS_LIFETIME, 5.0f);
    BUTTON_SET_STATE(CMD_SYS_LIFECONT, true);
    GET_TEXT_CTRL(CMD_SYS_TLIFETIME)->printf("%.1f", 5.0f);
    state.ps->Fire();
  } else {
    SLIDER_SET_VALUE(CMD_SYS_LIFETIME, state.ps->info.fLifetime);
    BUTTON_SET_STATE(CMD_SYS_LIFECONT, false);
    GET_TEXT_CTRL(CMD_SYS_TLIFETIME)->printf("%.1f", state.ps->info.fLifetime);
  }

  SLIDER_SET_VALUE(CMD_SYS_EMISSION, static_cast<float>(state.ps->info.nEmission));
  GET_TEXT_CTRL(CMD_SYS_TEMISSION)->printf("%d", state.ps->info.nEmission);

  SLIDER_SET_VALUE(CMD_SYS_PARLIFETIMEMIN, state.ps->info.fParticleLifeMin);
  SLIDER_SET_VALUE(CMD_SYS_PARLIFETIMEMAX, state.ps->info.fParticleLifeMax);
  BUTTON_SET_STATE(CMD_SYS_PARLIFETIMELOCK, false);

  if (sprParticles->GetBlendMode() & BLEND_ALPHABLEND) {
    BUTTON_SET_STATE(CMD_SYS_BLENDADDITIVE, false);
    BUTTON_SET_STATE(CMD_SYS_BLENDBLEND, true);
  } else {
    BUTTON_SET_STATE(CMD_SYS_BLENDADDITIVE, true);
    BUTTON_SET_STATE(CMD_SYS_BLENDBLEND, false);
  }

  // Particle movement

  SLIDER_SET_VALUE(CMD_PM_DIRECTION, state.ps->info.fDirection);
  GET_TEXT_CTRL(CMD_PM_TDIRECTION)->printf("%d", int(state.ps->info.fDirection * 180 / M_PI));
  BUTTON_SET_STATE(CMD_PM_RELATIVE, state.ps->info.bRelative);

  SLIDER_SET_VALUE(CMD_PM_SPREAD, state.ps->info.fSpread);
  GET_TEXT_CTRL(CMD_PM_TSPREAD)->printf("%d", int(state.ps->info.fSpread * 180 / M_PI));

  SLIDER_SET_VALUE(CMD_PM_STARTSPEEDMIN, state.ps->info.fSpeedMin);
  SLIDER_SET_VALUE(CMD_PM_STARTSPEEDMAX, state.ps->info.fSpeedMax);
  BUTTON_SET_STATE(CMD_PM_STARTSPEEDLOCK, false);

  SLIDER_SET_VALUE(CMD_PM_GRAVITYMIN, state.ps->info.fGravityMin);
  SLIDER_SET_VALUE(CMD_PM_GRAVITYMAX, state.ps->info.fGravityMax);
  BUTTON_SET_STATE(CMD_PM_GRAVITYLOCK, false);

  SLIDER_SET_VALUE(CMD_PM_RADIALMIN, state.ps->info.fRadialAccelMin);
  SLIDER_SET_VALUE(CMD_PM_RADIALMAX, state.ps->info.fRadialAccelMax);
  BUTTON_SET_STATE(CMD_PM_RADIALLOCK, false);

  SLIDER_SET_VALUE(CMD_PM_TANGENTIALMIN, state.ps->info.fTangentialAccelMin);
  SLIDER_SET_VALUE(CMD_PM_TANGENTIALMAX, state.ps->info.fTangentialAccelMax);
  BUTTON_SET_STATE(CMD_PM_TANGENTIALLOCK, false);

  // Particle appearance

  SLIDER_SET_VALUE(CMD_PA_SIZESTART, state.ps->info.fSizeStart * PARTICLE_SIZE);
  SLIDER_SET_VALUE(CMD_PA_SIZEEND, state.ps->info.fSizeEnd * PARTICLE_SIZE);
  SLIDER_SET_VALUE(CMD_PA_SIZEVAR, state.ps->info.fSizeVar);
  BUTTON_SET_STATE(CMD_PA_SIZELOCK, false);

  SLIDER_SET_VALUE(CMD_PA_SPINSTART, state.ps->info.fSpinStart);
  SLIDER_SET_VALUE(CMD_PA_SPINEND, state.ps->info.fSpinEnd);
  SLIDER_SET_VALUE(CMD_PA_SPINVAR, state.ps->info.fSpinVar);
  BUTTON_SET_STATE(CMD_PA_SPINLOCK, false);

  SLIDER_SET_VALUE(CMD_PA_ALPHASTART, state.ps->info.colColorStart.a);
  SLIDER_SET_VALUE(CMD_PA_ALPHAEND, state.ps->info.colColorEnd.a);
  SLIDER_SET_VALUE(CMD_PA_ALPHAVAR, state.ps->info.fAlphaVar);
  BUTTON_SET_STATE(CMD_PA_ALPHALOCK, false);

  SLIDER_SET_VALUE(CMD_PA_RSTART, state.ps->info.colColorStart.r);
  SLIDER_SET_VALUE(CMD_PA_GSTART, state.ps->info.colColorStart.g);
  SLIDER_SET_VALUE(CMD_PA_BSTART, state.ps->info.colColorStart.b);
  SLIDER_SET_VALUE(CMD_PA_REND, state.ps->info.colColorEnd.r);
  SLIDER_SET_VALUE(CMD_PA_GEND, state.ps->info.colColorEnd.g);
  SLIDER_SET_VALUE(CMD_PA_BEND, state.ps->info.colColorEnd.b);
  SLIDER_SET_VALUE(CMD_PA_RGBVAR, state.ps->info.fColorVar);

  hgeColor32 col1 = state.ps->info.colColorStart.GetHWColor().set_a(0xFF);
  hgeColor32 col2 = state.ps->info.colColorEnd.GetHWColor().set_a(0xFF);
  sprColor->SetColor(col1, 0);
  sprColor->SetColor(col1, 1);
  sprColor->SetColor(col2, 2);
  sprColor->SetColor(col2, 3);
}

void cmdChangeColor(int id) {
  const auto val = SLIDER_GET_VALUE(id);

  switch (id) {
    case CMD_PA_RSTART:
      state.ps->info.colColorStart.r = val;
      break;
    case CMD_PA_GSTART:
      state.ps->info.colColorStart.g = val;
      break;
    case CMD_PA_BSTART:
      state.ps->info.colColorStart.b = val;
      break;
    case CMD_PA_REND:
      state.ps->info.colColorEnd.r = val;
      break;
    case CMD_PA_GEND:
      state.ps->info.colColorEnd.g = val;
      break;
    case CMD_PA_BEND:
      state.ps->info.colColorEnd.b = val;
      break;
  }

  hgeColor32 col1 = state.ps->info.colColorStart.GetHWColor().set_a(0xFF);
  hgeColor32 col2 = state.ps->info.colColorEnd.GetHWColor().set_a(0xFF);
  sprColor->SetColor(col1, 0);
  sprColor->SetColor(col1, 1);
  sprColor->SetColor(col2, 2);
  sprColor->SetColor(col2, 3);
}
