/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeResourceManager helper class implementation
*/


#include "../../include/hgeresource.h"
#include "parser.h"
#include "resources.h"


HGE* hgeResourceManager::hge = nullptr;


hgeResourceManager::hgeResourceManager(const char* scriptname) {
    hge = hgeCreate(HGE_VERSION);

    for (auto i = 0; i < RESTYPES; i++) {
        res[i] = nullptr;
    }
    _parse_script(scriptname);
}

hgeResourceManager::~hgeResourceManager() {
    _remove_all();
    hge->Release();
}

void hgeResourceManager::_parse_script(const char* scriptname) {
    if (scriptname) {
        RScript::Parse(this, nullptr, scriptname, nullptr);

        auto rc = res[RES_SCRIPT];
        while (rc) {
            rc->Free();
            const auto rcnext = rc->next;
            delete rc;
            rc = rcnext;
        }
        res[RES_SCRIPT] = nullptr;
    }
}

void hgeResourceManager::_remove_all() {
    for (auto i = 0; i < RESTYPES; i++) {
        auto rc = res[i];
        while (rc) {
            rc->Free();
            const auto rcnext = rc->next;
            delete rc;
            rc = rcnext;
        }
        res[i] = nullptr;
    }
}

void hgeResourceManager::ChangeScript(const char* scriptname) {
    _remove_all();
    _parse_script(scriptname);
}

bool hgeResourceManager::Precache(const int groupid) {
    auto b_result = true;

    for (int i = 0; i < RESTYPES; i++) {
        auto rc = res[i];
        while (rc) {
            if (!groupid || groupid == rc->resgroup) {
                b_result = b_result && (rc->Get(this) != 0);
            }
            rc = rc->next;
        }
    }

    return b_result;
}

void hgeResourceManager::Purge(const int groupid) {
    for (int i = 0; i < RESTYPES; i++) {
        auto rc = res[i];
        while (rc) {
            if (!groupid || groupid == rc->resgroup) {
                rc->Free();
            }
            rc = rc->next;
        }
    }
}

void* hgeResourceManager::GetResource(const char* name, const int resgroup) {
    auto res = FindRes(this, RES_RESOURCE, name);

    if (res) {
        return reinterpret_cast<void *>(res->Get(this));
    }
    void* reshandle = hge->Resource_Load(name);
    if (reshandle) {
        const auto resource = new RResource();
        resource->handle = reinterpret_cast<hgeU32>(reshandle);
        resource->resgroup = resgroup;
        strcpy(resource->name, name);
        strcpy(resource->filename, name);
        AddRes(this, RES_RESOURCE, resource);

        return reshandle;
    }

    return nullptr;
}

HTEXTURE hgeResourceManager::GetTexture(const char* name, const int resgroup) {
    auto res = FindRes(this, RES_TEXTURE, name);
    if (res) {
        return static_cast<HTEXTURE>(res->Get(this));
    }
    const auto reshandle = hge->Texture_Load(name);
    if (reshandle) {
        const auto resource = new RTexture();
        resource->handle = reshandle;
        resource->resgroup = resgroup;
        resource->mipmap = false;
        strcpy(resource->name, name);
        strcpy(resource->filename, name);
        AddRes(this, RES_TEXTURE, resource);

        return reshandle;
    }

    return 0;
}

HEFFECT hgeResourceManager::GetEffect(const char* name, const int resgroup) {
    auto res = FindRes(this, RES_EFFECT, name);
    if (res) {
        return static_cast<HEFFECT>(res->Get(this));
    }
    const auto reshandle = hge->Effect_Load(name);
    if (reshandle) {
        REffect* resource = new REffect();
        resource->handle = reshandle;
        resource->resgroup = resgroup;
        strcpy(resource->name, name);
        strcpy(resource->filename, name);
        AddRes(this, RES_EFFECT, resource);

        return reshandle;
    }

    return 0;
}

HMUSIC hgeResourceManager::GetMusic(const char* name, const int resgroup) {
    auto res = FindRes(this, RES_MUSIC, name);
    if (res) {
        return static_cast<HMUSIC>(res->Get(this));
    }
    const auto reshandle = hge->Music_Load(name);
    if (reshandle) {
        RMusic* resource = new RMusic();
        resource->handle = reshandle;
        resource->resgroup = resgroup;
        strcpy(resource->name, name);
        strcpy(resource->filename, name);
        AddRes(this, RES_MUSIC, resource);

        return reshandle;
    }

    return 0;
}

HSTREAM hgeResourceManager::GetStream(const char* name, const int resgroup) {
    auto res = FindRes(this, RES_STREAM, name);
    if (res) {
        return static_cast<HSTREAM>(res->Get(this));
    }
    const auto reshandle = hge->Stream_Load(name);
    if (reshandle) {
        const auto resource = new RStream();
        resource->handle = reshandle;
        resource->resgroup = resgroup;
        strcpy(resource->name, name);
        strcpy(resource->filename, name);
        AddRes(this, RES_STREAM, resource);

        return reshandle;
    }

    return 0;
}

HTARGET hgeResourceManager::GetTarget(const char* name) {
    auto res = FindRes(this, RES_TARGET, name);
    if (res) {
        return static_cast<HTARGET>(res->Get(this));
    }
    return 0;
}

hgeSprite* hgeResourceManager::GetSprite(const char* name) {
    auto res = FindRes(this, RES_SPRITE, name);
    if (res) {
        return reinterpret_cast<hgeSprite *>(res->Get(this));
    }
    return nullptr;
}

hgeAnimation* hgeResourceManager::GetAnimation(const char* name) {
    auto res = FindRes(this, RES_ANIMATION, name);
    if (res) {
        return reinterpret_cast<hgeAnimation *>(res->Get(this));
    }
    return nullptr;
}

hgeFont* hgeResourceManager::GetFont(const char* name) {
    auto res = FindRes(this, RES_FONT, name);
    if (res) {
        return reinterpret_cast<hgeFont *>(res->Get(this));
    }
    return nullptr;
}

hgeParticleSystem* hgeResourceManager::GetParticleSystem(const char* name) {
    auto res = FindRes(this, RES_PARTICLE, name);
    if (res) {
        return reinterpret_cast<hgeParticleSystem *>(res->Get(this));
    }
    return nullptr;
}

hgeDistortionMesh* hgeResourceManager::GetDistortionMesh(const char* name) {
    auto res = FindRes(this, RES_DISTORT, name);
    if (res) {
        return reinterpret_cast<hgeDistortionMesh *>(res->Get(this));
    }
    return nullptr;
}

hgeStringTable* hgeResourceManager::GetStringTable(const char* name, 
                                                   const int resgroup) {
    auto res = FindRes(this, RES_STRTABLE, name);
    if (res) {
        return reinterpret_cast<hgeStringTable*>(res->Get(this));
    }
    auto strtable = new hgeStringTable(name);
    const auto resource = new RStringTable();
    resource->handle = reinterpret_cast<hgeU32>(strtable);
    resource->resgroup = resgroup;
    strcpy(resource->name, name);
    strcpy(resource->filename, name);
    AddRes(this, RES_STRTABLE, resource);

    return strtable;
}
