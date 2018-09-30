/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeResourceManager resources header
*/

#pragma once


#include "../../include/hgeresource.h"
#include "parser.h"


#define RES_SCRIPT		0

#define RES_RESOURCE	1
#define RES_TEXTURE		2
#define RES_EFFECT		3
#define RES_MUSIC		4
#define RES_STREAM		5
#define RES_TARGET		6
#define RES_SPRITE		7
#define RES_ANIMATION	8
#define RES_FONT		9
#define RES_PARTICLE	10
#define RES_DISTORT		11
#define RES_STRTABLE	12


void AddRes(hgeResourceManager* rm, int type, ResDesc* resource);
ResDesc* FindRes(hgeResourceManager* rm, int type, const char* name);


struct RScript : public ResDesc {
    static void Parse(hgeResourceManager* rm, RScriptParser* sp, const char* name,
                      const char* basename);

    hgeU32 Get(hgeResourceManager* rm) override {
        return 0;
    }

    void Free() override {
    }
};

struct RResource : public ResDesc {
    char filename[MAXRESCHARS];

    static void Parse(hgeResourceManager* rm, RScriptParser* sp, const char* name,
                      const char* basename);
    hgeU32 Get(hgeResourceManager* rm) override;
    void Free() override;
};

struct RTexture : public ResDesc {
    char filename[MAXRESCHARS];
    bool mipmap;

    static void Parse(hgeResourceManager* rm, RScriptParser* sp, const char* name,
                      const char* basename);
    hgeU32 Get(hgeResourceManager* rm) override;
    void Free() override;
};

struct REffect : public ResDesc {
    char filename[MAXRESCHARS];

    static void Parse(hgeResourceManager* rm, RScriptParser* sp, const char* name,
                      const char* basename);
    hgeU32 Get(hgeResourceManager* rm) override;
    void Free() override;
};

struct RMusic : public ResDesc {
    char filename[MAXRESCHARS];
    int amplify;

    static void Parse(hgeResourceManager* rm, RScriptParser* sp, const char* name,
                      const char* basename);
    hgeU32 Get(hgeResourceManager* rm) override;
    void Free() override;
};

struct RStream : public ResDesc {
    char filename[MAXRESCHARS];

    static void Parse(hgeResourceManager* rm, RScriptParser* sp, const char* name,
                      const char* basename);
    hgeU32 Get(hgeResourceManager* rm) override;
    void Free() override;
};

struct RTarget : public ResDesc {
    int width;
    int height;
    bool zbuffer;

    static void Parse(hgeResourceManager* rm, RScriptParser* sp, const char* name,
                      const char* basename);
    hgeU32 Get(hgeResourceManager* rm) override;
    void Free() override;
};

struct RSprite : public ResDesc {
    char texname[MAXRESCHARS];
    float tx, ty, w, h;
    float hotx, hoty;
    int blend;
    hgeU32 color;
    float z;
    bool bXFlip, bYFlip;
    //	float		x,y;
    //	float		scale;
    //	float		rotation;
    //	int			collision;

    static void Parse(hgeResourceManager* rm, RScriptParser* sp, const char* name,
                      const char* basename);
    hgeU32 Get(hgeResourceManager* rm) override;
    void Free() override;
};

struct RAnimation : public RSprite {
    int frames;
    float fps;
    int mode;

    static void Parse(hgeResourceManager* rm, RScriptParser* sp, const char* name,
                      const char* basename);
    hgeU32 Get(hgeResourceManager* rm) override;
    void Free() override;
};

struct RFont : public ResDesc {
    char filename[MAXRESCHARS];
    bool mipmap;
    int blend;
    hgeU32 color;
    float z;
    float scale;
    float proportion;
    float tracking;
    float spacing;
    float rotation;

    static void Parse(hgeResourceManager* rm, RScriptParser* sp, const char* name,
                      const char* basename);
    hgeU32 Get(hgeResourceManager* rm) override;
    void Free() override;
};

struct RParticle : public ResDesc {
    char filename[MAXRESCHARS];
    char spritename[MAXRESCHARS];

    static void Parse(hgeResourceManager* rm, RScriptParser* sp, const char* name,
                      const char* basename);
    hgeU32 Get(hgeResourceManager* rm) override;
    void Free() override;
};

struct RDistort : public ResDesc {
    char texname[MAXRESCHARS];
    float tx, ty, w, h;
    int cols, rows;
    int blend;
    hgeU32 color;
    float z;

    static void Parse(hgeResourceManager* rm, RScriptParser* sp, const char* name,
                      const char* basename);
    hgeU32 Get(hgeResourceManager* rm) override;
    void Free() override;
};


struct RStringTable : public ResDesc {
    char filename[MAXRESCHARS];

    static void Parse(hgeResourceManager* rm, RScriptParser* sp, const char* name,
                      const char* basename);
    hgeU32 Get(hgeResourceManager* rm) override;
    void Free() override;
};
