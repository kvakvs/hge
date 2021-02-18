/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// hgeResourceManager resources header
//

#pragma once


#include <hgeresource.h>
#include "parser.h"


enum {
    RES_SCRIPT = 0,
    RES_RESOURCE = 1,
    RES_TEXTURE = 2,
    RES_EFFECT = 3,
    RES_MUSIC = 4,
    RES_STREAM = 5,
    RES_TARGET = 6,
    RES_SPRITE = 7,
    RES_ANIMATION = 8,
    RES_FONT = 9,
    RES_PARTICLE = 10,
    RES_DISTORT = 11,
    RES_STRTABLE = 12,
};

struct RScript : public ResDesc {
    static void Parse(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                      const char *basename);

    uint32_t Get(hgeResourceManager *rm) override {
      return 0;
    }

    void Free() override {
    }
};

struct RResource : public ResDesc {
protected:
    friend class hgeResourceManager;

    friend class _hgeResources;

    std::string filename;

public:
    static void Parse(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                      const char *basename);

    uint32_t Get(hgeResourceManager *rm) override;

    void Free() override;
};

struct RTexture : public ResDesc {
protected:
    friend class hgeResourceManager;

    std::string filename;
    bool mipmap;

public:
    static void Parse(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                      const char *basename);

    uint32_t Get(hgeResourceManager *rm) override;

    void Free() override;
};

struct REffect : public ResDesc {
protected:
    friend class hgeResourceManager;

    std::string filename;

public:
    static void Parse(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                      const char *basename);

    uint32_t Get(hgeResourceManager *rm) override;

    void Free() override;
};

struct RMusic : public ResDesc {
protected:
    friend class hgeResourceManager;

    std::string filename;
    int amplify;

public:
    static void Parse(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                      const char *basename);

    uint32_t Get(hgeResourceManager *rm) override;

    void Free() override;
};

struct RStream : public ResDesc {
protected:
    friend class hgeResourceManager;

    std::string filename;

public:
    static void Parse(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                      const char *basename);

    uint32_t Get(hgeResourceManager *rm) override;

    void Free() override;
};

struct RTarget : public ResDesc {
    int width;
    int height;
    bool zbuffer;

    static void Parse(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                      const char *basename);

    uint32_t Get(hgeResourceManager *rm) override;

    void Free() override;
};

struct RSprite : public ResDesc {
    char texname[MAXRESCHARS];
    float tx, ty, w, h;
    float hotx, hoty;
    hgeBlendMode blend;
    hgeColor32 color;
    float z;
    bool bXFlip, bYFlip;

    static void Parse(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                      const char *basename);

    uint32_t Get(hgeResourceManager *rm) override;

    void Free() override;
};

struct RAnimation : public RSprite {
    int frames{};
    float fps{};
    int mode{};

    static void Parse(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                      const char *basename);

    uint32_t Get(hgeResourceManager *rm) override;

    void Free() override;
};

struct RFont : public ResDesc {
    char filename[MAXRESCHARS];
    bool mipmap;
    hgeBlendMode blend;
    hgeColor32 color;
    float z;
    float scale;
    float proportion;
    float tracking;
    float spacing;
    float rotation;

    static void Parse(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                      const char *basename);

    uint32_t Get(hgeResourceManager *rm) override;

    void Free() override;
};

struct RParticle : public ResDesc {
    char filename[MAXRESCHARS];
    char spritename[MAXRESCHARS];

    static void Parse(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                      const char *basename);

    uint32_t Get(hgeResourceManager *rm) override;

    void Free() override;
};

struct RDistort : public ResDesc {
    char texname[MAXRESCHARS];
    float tx, ty, w, h;
    int cols, rows;
    hgeBlendMode blend;
    hgeColor32 color;
    float z;

    static void Parse(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                      const char *basename);

    uint32_t Get(hgeResourceManager *rm) override;

    void Free() override;
};


struct RStringTable : public ResDesc {
protected:
    friend class hgeResourceManager;

    std::string filename;

public:
    static void Parse(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                      const char *basename);

    uint32_t Get(hgeResourceManager *rm) override;

    void Free() override;
};


class _hgeResources {
public:
    static void AddRes(hgeResourceManager *rm, int type, ResDesc *resource);

    static ResDesc *FindRes(hgeResourceManager *rm, int type, const char *name);


    static void ScriptParseFileResource(hgeResourceManager *rm, RScriptParser *sp, const char *name,
                                        const char *basename, ResDesc *rr, int restype);

    static bool ScriptSkipToNextParameter(RScriptParser *sp, bool ignore);

    static void ScriptParseSpriteAnim(RScriptParser *sp, RSprite *rc, bool anim);
};
