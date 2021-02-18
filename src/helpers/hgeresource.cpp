/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// hgeResourceManager helper class implementation
//


#include <hgeresource.h>
#include "parser.h"
#include "resources.h"


HGE *hgeResourceManager::hge_ = nullptr;


hgeResourceManager::hgeResourceManager(const char *scriptname) {
  hge_ = hgeCreate(HGE_VERSION);

  for (auto &re : res) {
    re = nullptr;
  }
  _parse_script(scriptname);
}

hgeResourceManager::~hgeResourceManager() {
  _remove_all();
  hge_->Release();
}

void hgeResourceManager::_parse_script(const char *scriptname) {
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
  for (auto &re : res) {
    auto rc = re;
    while (rc) {
      rc->Free();
      const auto rcnext = rc->next;
      delete rc;
      rc = rcnext;
    }
    re = nullptr;
  }
}

void hgeResourceManager::ChangeScript(const char *scriptname) {
  _remove_all();
  _parse_script(scriptname);
}

bool hgeResourceManager::Precache(const int groupid) {
  auto b_result = true;

  for (auto rc : res) {
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
  for (auto rc : res) {
    while (rc) {
      if (!groupid || groupid == rc->resgroup) {
        rc->Free();
      }
      rc = rc->next;
    }
  }
}

void *hgeResourceManager::GetResource(const char *name, const int resgroup) {
  auto res1 = _hgeResources::FindRes(this, RES_RESOURCE, name);

  if (res1) {
    return reinterpret_cast<void *>(res1->Get(this));
  }
  void *reshandle = hge_->Resource_Load(name);
  if (reshandle) {
    const auto resource = new RResource();
    resource->handle = reinterpret_cast<uint32_t>(reshandle);
    resource->resgroup = resgroup;
    resource->name = name;
    resource->filename = name;
    _hgeResources::AddRes(this, RES_RESOURCE, resource);

    return reshandle;
  }

  return nullptr;
}

HTEXTURE hgeResourceManager::GetTexture(const char *name, const int resgroup) {
  auto res1 = _hgeResources::FindRes(this, RES_TEXTURE, name);
  if (res1) {
    return static_cast<HTEXTURE>(res1->Get(this));
  }
  const auto reshandle = hge_->Texture_Load(name);
  if (reshandle) {
    const auto resource = new RTexture();
    resource->handle = reshandle;
    resource->resgroup = resgroup;
    resource->mipmap = false;
    resource->name = name;
    resource->filename = name;
    _hgeResources::AddRes(this, RES_TEXTURE, resource);

    return reshandle;
  }

  return 0;
}

HEFFECT hgeResourceManager::GetEffect(const char *name, const int resgroup) {
  auto res1 = _hgeResources::FindRes(this, RES_EFFECT, name);
  if (res1) {
    return static_cast<HEFFECT>(res1->Get(this));
  }
  const auto reshandle = hge_->Effect_Load(name);
  if (reshandle) {
    auto *resource = new REffect();
    resource->handle = reshandle;
    resource->resgroup = resgroup;
    resource->name = name;
    resource->filename = name;
    _hgeResources::AddRes(this, RES_EFFECT, resource);

    return reshandle;
  }

  return 0;
}

HMUSIC hgeResourceManager::GetMusic(const char *name, const int resgroup) {
  auto res1 = _hgeResources::FindRes(this, RES_MUSIC, name);
  if (res1) {
    return static_cast<HMUSIC>(res1->Get(this));
  }
  const auto reshandle = hge_->Music_Load(name);
  if (reshandle) {
    auto *resource = new RMusic();
    resource->handle = reshandle;
    resource->resgroup = resgroup;
    resource->name = name;
    resource->filename = name;
    _hgeResources::AddRes(this, RES_MUSIC, resource);

    return reshandle;
  }

  return 0;
}

HSTREAM hgeResourceManager::GetStream(const char *name, const int resgroup) {
  auto res1 = _hgeResources::FindRes(this, RES_STREAM, name);
  if (res1) {
    return static_cast<HSTREAM>(res1->Get(this));
  }
  const auto reshandle = hge_->Stream_Load(name);
  if (reshandle) {
    const auto resource = new RStream();
    resource->handle = reshandle;
    resource->resgroup = resgroup;
    resource->name = name;
    resource->filename = name;
    _hgeResources::AddRes(this, RES_STREAM, resource);

    return reshandle;
  }

  return 0;
}

HTARGET hgeResourceManager::GetTarget(const char *name) {
  auto res1 = _hgeResources::FindRes(this, RES_TARGET, name);
  if (res1) {
    return static_cast<HTARGET>(res1->Get(this));
  }
  return 0;
}

hgeSprite *hgeResourceManager::GetSprite(const char *name) {
  auto res1 = _hgeResources::FindRes(this, RES_SPRITE, name);
  if (res1) {
    return reinterpret_cast<hgeSprite *>(res1->Get(this));
  }
  return nullptr;
}

hgeAnimation *hgeResourceManager::GetAnimation(const char *name) {
  auto res1 = _hgeResources::FindRes(this, RES_ANIMATION, name);
  if (res1) {
    return reinterpret_cast<hgeAnimation *>(res1->Get(this));
  }
  return nullptr;
}

hgeFont *hgeResourceManager::GetFont(const char *name) {
  auto res1 = _hgeResources::FindRes(this, RES_FONT, name);
  if (res1) {
    return reinterpret_cast<hgeFont *>(res1->Get(this));
  }
  return nullptr;
}

hgeParticleSystem *hgeResourceManager::GetParticleSystem(const char *name) {
  auto res1 = _hgeResources::FindRes(this, RES_PARTICLE, name);
  if (res1) {
    return reinterpret_cast<hgeParticleSystem *>(res1->Get(this));
  }
  return nullptr;
}

hgeDistortionMesh *hgeResourceManager::GetDistortionMesh(const char *name) {
  auto res1 = _hgeResources::FindRes(this, RES_DISTORT, name);
  if (res1) {
    return reinterpret_cast<hgeDistortionMesh *>(res1->Get(this));
  }
  return nullptr;
}

hgeStringTable *hgeResourceManager::GetStringTable(const char *name,
                                                   const int resgroup) {
  auto res1 = _hgeResources::FindRes(this, RES_STRTABLE, name);
  if (res1) {
    return reinterpret_cast<hgeStringTable *>(res1->Get(this));
  }
  auto strtable = new hgeStringTable(name);
  const auto resource = new RStringTable();
  resource->handle = reinterpret_cast<uint32_t>(strtable);
  resource->resgroup = resgroup;
  resource->name = name;
  resource->filename = name;
  _hgeResources::AddRes(this, RES_STRTABLE, resource);

  return strtable;
}
