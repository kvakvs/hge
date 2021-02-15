/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.8.1
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Core functions implementation: ini file
//

#include "hge_impl.h"


void HGE_CALL HGE_Impl::Ini_SetInt(const char *section, const char *name,
                                   const int value) {
  char buf[256];

  if (ini_file_[0]) {
    sprintf(buf, "%d", value);
    WritePrivateProfileString(section, name, buf, ini_file_);
  }
}


int HGE_CALL HGE_Impl::Ini_GetInt(const char *section, const char *name,
                                  const int def_val) {
  char buf[256];

  if (ini_file_[0]) {
    if (GetPrivateProfileString(section, name, "", buf, sizeof(buf), ini_file_)) {
      return atoi(buf);
    }
  }
  return def_val;
}


void HGE_CALL HGE_Impl::Ini_SetFloat(const char *section, const char *name,
                                     const float value) {
  char buf[256];

  if (ini_file_[0]) {
    sprintf(buf, "%f", value);
    WritePrivateProfileString(section, name, buf, ini_file_);
  }
}


float HGE_CALL HGE_Impl::Ini_GetFloat(const char *section, const char *name,
                                      const float def_val) {
  char buf[256];

  if (ini_file_[0]) {
    if (GetPrivateProfileString(section, name, "", buf, sizeof(buf), ini_file_)) {
      return static_cast<float>(atof(buf));
    }
  }
  return def_val;
}


void HGE_CALL HGE_Impl::Ini_SetString(const char *section, const char *name,
                                      const char *value) {
  if (ini_file_[0]) {
    WritePrivateProfileString(section, name, value, ini_file_);
  }
}


char *HGE_CALL HGE_Impl::Ini_GetString(const char *section, const char *name,
                                       const char *def_val) {
  if (ini_file_[0]) {
    GetPrivateProfileString(section, name, def_val, ini_string_, sizeof(ini_string_),
                            ini_file_);
  } else {
    strcpy(ini_string_, def_val);
  }
  return ini_string_;
}
