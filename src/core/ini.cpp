/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
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

namespace hgeImpl {

    void HGE_CALL HGE_Impl::Ini_SetInt(const char *section, const char *name,
                                       const int value) {
      if (!ini_file_.empty()) {
        char buf[256];
        sprintf(buf, "%d", value);
        WritePrivateProfileString(section, name, buf, ini_file_.c_str());
      }
    }


    int HGE_CALL HGE_Impl::Ini_GetInt(const char *section, const char *name,
                                      const int def_val) {
      if (!ini_file_.empty()) {
        char buf[256];
        if (GetPrivateProfileString(section, name, "", buf, sizeof(buf), ini_file_.c_str())) {
          return atoi(buf);
        }
      }
      return def_val;
    }


    void HGE_CALL HGE_Impl::Ini_SetFloat(const char *section, const char *name,
                                         const float value) {
      char buf[256];

      if (!ini_file_.empty()) {
        sprintf(buf, "%f", value);
        WritePrivateProfileString(section, name, buf, ini_file_.c_str());
      }
    }


    float HGE_CALL HGE_Impl::Ini_GetFloat(const char *section, const char *name,
                                          const float def_val) {
      if (!ini_file_.empty()) {
        char buf[256];
        if (GetPrivateProfileString(section, name, "", buf, sizeof(buf), ini_file_.c_str())) {
          return static_cast<float>(atof(buf));
        }
      }
      return def_val;
    }


    void HGE_CALL HGE_Impl::Ini_SetString(const char *section, const char *name,
                                          const char *value) {
      if (!ini_file_.empty()) {
        WritePrivateProfileString(section, name, value, ini_file_.c_str());
      }
    }


    const char *HGE_CALL HGE_Impl::Ini_GetString(const char *section, const char *name,
                                                 const char *def_val) {
      static char tmp[256];

      if (!ini_file_.empty()) {
        GetPrivateProfileString(section, name, def_val, tmp, sizeof(tmp), ini_file_.c_str());
      } else {
        strcpy(tmp, def_val);
      }
      return tmp;
    }

} // namespace
