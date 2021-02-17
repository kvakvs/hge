/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Resource script parser implementation
//

#include "parser.h"


HGE *RScriptParser::hge_ = nullptr;


struct Keyword {
    const char *word_;
    int code_;
};

Keyword keytable[] = {
        {"=",           TTEQUALS},
        {":",           TTBASED},
        {",",           TTSEPARATOR},
        {"{",           TTOPENBLOCK},
        {"}",           TTCLOSEBLOCK},
        {"true",        TTBOOL},
        {"false",       TTBOOL},

        {"Include",     TTRES_INCLUDE},
        {"Resource",    TTRES_RESOURCE},
        {"Texture",     TTRES_TEXTURE},
        {"Sound",       TTRES_SOUND},
        {"Music",       TTRES_MUSIC},
        {"Stream",      TTRES_STREAM},
        {"Target",      TTRES_TARGET},
        {"Sprite",      TTRES_SPRITE},
        {"Animation",   TTRES_ANIMATION},
        {"Font",        TTRES_FONT},
        {"Particle",    TTRES_PARTICLE},
        {"Distortion",  TTRES_DISTORT},
        {"StringTable", TTRES_STRTABLE},

        {"filename",    TTPAR_FILENAME},
        {"resgroup",    TTPAR_RESGROUP},
        {"mipmap",      TTPAR_MIPMAP},
        {"amplify",     TTPAR_AMPLIFY},
        {"size",        TTPAR_SIZE},
        {"zbuffer",     TTPAR_ZBUFFER},
        {"texture",     TTPAR_TEXTURE},
        {"rect",        TTPAR_RECT},
        {"hotspot",     TTPAR_HOTSPOT},
        {"blendmode",   TTPAR_BLENDMODE},
        {"color",       TTPAR_COLOR},
        {"zorder",      TTPAR_ZORDER},
        {"flip",        TTPAR_FLIP},
        {"scale",       TTPAR_SCALE},
        {"proportion",  TTPAR_PROPORTION},
        {"rotation",    TTPAR_ROTATION},
        {"frames",      TTPAR_FRAMES},
        {"fps",         TTPAR_FPS},
        {"mode",        TTPAR_MODE},
        {"tracking",    TTPAR_TRACKING},
        {"spacing",     TTPAR_SPACING},
        {"sprite",      TTPAR_SPRITE},
        {"mesh",        TTPAR_MESH},

        {"COLORMUL",    TTCON_COLORMUL},
        {"COLORADD",    TTCON_COLORADD},
        {"ALPHABLEND",  TTCON_ALPHABLND},
        {"ALPHAADD",    TTCON_ALPHAADD},
        {"ZWRITE",      TTCON_ZWRITE},
        {"NOZWRITE",    TTCON_NOZWRITE},
        {"FORWARD",     TTCON_FORWARD},
        {"REVERSE",     TTCON_REVERSE},
        {"PINGPONG",    TTCON_PINGPONG},
        {"NOPINGPONG",  TTCON_NOPINGPONG},
        {"LOOP",        TTCON_LOOP},
        {"NOLOOP",      TTCON_NOLOOP},
        {"CIRCLE",      TTCON_CIRCLE},
        {"RECT",        TTCON_RECT},
        {"ALPHA",       TTCON_ALPHA},

        {nullptr,       TTNONE}
};

RScriptParser::RScriptParser(const std::string &name, const char *scr) {
  hge_ = hgeCreate(HGE_VERSION);

  scriptname_ = name;
  script_ = scr;
  tokenvalue_[0] = 0;
  tokentype_ = TTNONE;
  line_ = 1;
}

int RScriptParser::get_token() {
  int i;

  // Skip whitespaces and comments

  for (;;) {
    while (*script_ == ' ' || *script_ == '\t' || *script_ == '\n' || *script_ == '\r') {
      if (*script_ == '\n') {
        line_++;
      }
      script_++;
    }
    if (*script_ == ';')
      while (*script_ && *script_ != '\n' && *script_ != '\r') {
        script_++;
      }
    else {
      break;
    }
  }

  // End of script

  if (!*script_) {
    tokentype_ = TTEND;
    tokenvalue_[0] = 0;
    return tokentype_;
  }

  // Number

  if ((*script_ >= '0' && *script_ <= '9') || *script_ == '.' || *script_ == '-') {
    tokentype_ = TTNUMBER;
    for (i = 0; (*script_ >= '0' && *script_ <= '9') || *script_ == '.' || *script_ == '-'; i++) {
      tokenvalue_[i] = *script_++;
    }

    // Hexadecimal number starting with decimal digit

    if ((*script_ >= 'A' && *script_ <= 'F') || (*script_ >= 'a' && *script_ <= 'f')) {
      tokentype_ = TTSTRING;
      for (; (*script_ >= 'A' && *script_ <= 'F') || (*script_ >= 'a' && *script_ <= 'f'); i++) {
        tokenvalue_[i] = *script_++;
      }
    }

    tokenvalue_[i] = 0;
    return tokentype_;
  }

  // Quoted string

  if (*script_ == '"') {
    tokentype_ = TTSTRING;
    script_++;
    for (i = 0; *script_ && *script_ != '"' && *script_ != '\n' && *script_ != '\r'; i++) {
      tokenvalue_[i] = *script_++;
    }
    tokenvalue_[i] = 0;
    if (*script_) {
      script_++;
    }
    return tokentype_;
  }

  // Keyword

  for (i = 0; keytable[i].word_; i++)
    if (!strtkcmp(keytable[i].word_, script_)) {
      tokentype_ = keytable[i].code_;
      strcpy(tokenvalue_, keytable[i].word_);
      script_ += strlen(keytable[i].word_);
      return tokentype_;
    }

  // Unquoted string or hexadecimal number

  tokentype_ = TTSTRING;
  for (i = 0;
       *script_ && *script_ != ' ' && *script_ != '\t' && *script_ != '\n' && *script_ != '\r'
       && *script_ != ',' && *script_ != '=' && *script_ != '{' && *script_ != '}' &&
       *script_ != ':';
       i++) {
    tokenvalue_[i] = *script_++;
  }
  tokenvalue_[i] = 0;
  return tokentype_;
}

bool RScriptParser::strtkcmp(const char *str, const char *mem) {
  const int len = strlen(str);
  for (auto i = 0; i < len; i++) {
    if (!mem[i]) {
      return true;
    }
    if (mem[i] != str[i]) {
      return true;
    }
  }
  return false;
}

uint32_t RScriptParser::tkn_hex() {
  uint32_t dw = 0;
  for (auto i = 0; tokenvalue_[i]; i++) {
    auto chr = tokenvalue_[i];
    if (chr >= 'a') {
      chr -= 'a' - ':';
    }
    if (chr >= 'A') {
      chr -= 'A' - ':';
    }
    chr -= '0';
    if (chr > 0xF) {
      chr = 0xF;
    }
    dw = (dw << 4) | chr;
  }
  return dw;
}

void RScriptParser::script_post_error(const char *msg1, const char *msg2) {
  hge_->System_Log("%s, line %d: %s'%s'%s",
                   get_name().c_str(),
                   get_line(),
                   msg1,
                   tokenvalue_[0] ? tkn_string() : "<EOF>",
                   msg2);
}
