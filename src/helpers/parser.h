/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Resource script parser header
*/

#pragma once


#include <hge.h>
#include <cstring>
#include <cstdlib>
#include <string>


enum {
    TTNONE,
    TTEND,
    TTNUMBER,
    TTSTRING,

    TTBOOL,
    TTEQUALS,
    TTBASED,
    TTSEPARATOR,
    TTOPENBLOCK,
    TTCLOSEBLOCK,

    TTRES__FIRST,
    TTRES_INCLUDE,
    TTRES_RESOURCE,
    TTRES_TEXTURE,
    TTRES_SOUND,
    TTRES_MUSIC,
    TTRES_STREAM,
    TTRES_TARGET,
    TTRES_SPRITE,
    TTRES_ANIMATION,
    TTRES_FONT,
    TTRES_PARTICLE,
    TTRES_DISTORT,
    TTRES_STRTABLE,
    TTRES__LAST,

    TTPAR__FIRST,
    TTPAR_FILENAME,
    TTPAR_RESGROUP,
    TTPAR_MIPMAP,
    TTPAR_AMPLIFY,
    TTPAR_SIZE,
    TTPAR_ZBUFFER,
    TTPAR_TEXTURE,
    TTPAR_RECT,
    TTPAR_HOTSPOT,
    TTPAR_BLENDMODE,
    TTPAR_COLOR,
    TTPAR_ZORDER,
    TTPAR_FLIP,
    TTPAR_SCALE,
    TTPAR_PROPORTION,
    TTPAR_ROTATION,
    TTPAR_FRAMES,
    TTPAR_FPS,
    TTPAR_MODE,
    TTPAR_TRACKING,
    TTPAR_SPACING,
    TTPAR_SPRITE,
    TTPAR_MESH,
    TTPAR__LAST,

    TTCON__FIRST,
    TTCON_COLORMUL,
    TTCON_COLORADD,
    TTCON_ALPHABLND,
    TTCON_ALPHAADD,
    TTCON_ZWRITE,
    TTCON_NOZWRITE,
    TTCON_FORWARD,
    TTCON_REVERSE,
    TTCON_PINGPONG,
    TTCON_NOPINGPONG,
    TTCON_LOOP,
    TTCON_NOLOOP,
    TTCON_CIRCLE,
    TTCON_RECT,
    TTCON_ALPHA,
    TTCON__LAST
};


class RScriptParser {
public:
    RScriptParser(const std::string &name, const char *scr);

    ~RScriptParser() {
      hge_->Release();
    }

    int get_token();

    void put_back() {
      script_ -= strlen(tokenvalue_);
    }

    int get_line() const {
      return line_;
    }

    const std::string &get_name() const {
      return scriptname_;
    }

    char *tkn_string() {
      return tokenvalue_;
    }

    int tkn_int() const {
      return atoi(tokenvalue_);
    }

    float tkn_float() const {
      return static_cast<float>(atof(tokenvalue_));
    }

    bool tkn_bool() {
      return (tokenvalue_[0] == 't' || tokenvalue_[0] == 'T') ? true : false;
    }

    uint32_t tkn_hex();

    void script_post_error(const char *msg1, const char *msg2);

    int tokentype_;
    char tokenvalue_[128]{};
    const char *script_;
    std::string scriptname_;
    int line_;

private:
    static bool strtkcmp(const char *str, const char *mem);

    static HGE *hge_;
};
