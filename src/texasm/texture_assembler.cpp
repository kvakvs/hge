/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#include "texture_assembler.h"
#include "sprite_object.h"

#include <hgesprite.h>


void CTextureAssembler::ClearResources() {

  for (auto &it : obj_list) {
    delete it;
  }

  obj_list.clear();
}


bool CTextureAssembler::CheckMask(const std::string &name,
                                  char *mask_set,
                                  const bool mask_inclusive) {

  if (!mask_set || !*mask_set) {
    return true;
  }

  auto mask = mask_set;
  const int name_len = name.length();

  while (*mask) {
    const int mask_len = strlen(mask);

    const auto match = (name_len >= mask_len) && !memcmp(name.data(), mask, mask_len);

    if (match && mask_inclusive) {
      return true;
    }
    if (!match && !mask_inclusive) {
      return true;
    }

    mask += strlen(mask) + 1;
  }

  return false;
}


CGfxObject *CTextureAssembler::FindObj(const GfxObjList &objlist, const char *name) const {
  for (auto &it : obj_list)
    if (!strcmp(it->GetName(), name)) {
      return it;
    }
  return nullptr;
}


void CTextureAssembler::AccumulateRMResources(hgeResourceManager *rm,
                                              const int resgroup, char *mask_set,
                                              const bool mask_inclusive) {
  // RES_SPRITE
  auto resdesc = rm->res[7];

  while (resdesc) {
    if (!resgroup || resdesc->resgroup == resgroup)
      if (CheckMask(resdesc->name, mask_set, mask_inclusive))
        if (!FindObj(obj_list, resdesc->name.c_str())) {
          const auto spr = new CSpriteObject(
                  reinterpret_cast<hgeSprite *>(resdesc->Get(rm)),
                  resdesc->name.c_str(),
                  resdesc->resgroup,
                  false
          );
          obj_list.push_back(spr);
        }

    resdesc = resdesc->next;
  }
}


void CTextureAssembler::AccumulateFileResources(char *wildcard, const int resgroup,
                                                char *mask_set,
                                                const bool mask_inclusive) {
  WIN32_FIND_DATA search_data;
  char filename[MAX_PATH];
  char temp[MAX_PATH];
  char *buf;


  const auto h_search = FindFirstFile(wildcard, &search_data);

  if (h_search == INVALID_HANDLE_VALUE) {
    SysLog("Can't find the path: %s\n", wildcard);
    return;
  }

  do {
    // recurse subfolders

    if (search_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      if (search_data.cFileName[0] == '.') {
        continue;
      }

      strcpy(filename, wildcard);
      buf = strrchr(filename, '\\');
      if (!buf) {
        buf = filename;
      } else {
        buf++;
      }

      strcpy(temp, buf);
      strcpy(buf, search_data.cFileName);
      strcat(filename, "\\");
      strcat(filename, temp);

      AccumulateFileResources(filename, resgroup, mask_set, mask_inclusive);
    }

      // process a file

    else {
      if (CheckMask(search_data.cFileName, mask_set, mask_inclusive))
        if (!FindObj(obj_list, search_data.cFileName)) {
          strcpy(filename, wildcard);
          buf = strrchr(filename, '\\');
          if (!buf) {
            buf = filename;
          } else {
            buf++;
          }
          strcpy(buf, search_data.cFileName);

          const auto tex = hge->Texture_Load(filename);
          if (!tex) {
            SysLog("Can't load texture: %s\n", filename);
            continue;
          }

          const auto spr = new hgeSprite(
                  tex, 0, 0,
                  static_cast<float>(hge->Texture_GetWidth(tex, true)),
                  static_cast<float>(hge->Texture_GetHeight(tex, true))
          );

          buf = new char[strlen(search_data.cFileName) + 1];
          strcpy(buf, search_data.cFileName);

          obj_list.push_back(new CSpriteObject(spr, buf, resgroup, true));
        }
    }

  } while (FindNextFile(h_search, &search_data));

  FindClose(h_search);
}


bool CTextureAssembler::GenerateTextures(char *wildcard) {
  GfxObjIterator it;

  char texfile[MAX_PATH];
  char resfile[MAX_PATH];
  char texname[128];

  // extract texture name from wildcard

  char *name = strrchr(wildcard, '\\');

  if (name != nullptr) {
    name++;
  } else {
    name = wildcard;
  }

  const auto pathlen = name - wildcard;

  if (!strlen(name)) {
    SysLog("Invalid wildcard: %s\n", wildcard);
    return false;
  }

  // check if we have something to process

  if (!obj_list.size()) {
    SysLog("No resources in task.\n");
    return false;
  }

  // sort objects

  obj_list.sort(ByLargestDimensionDescending());

  // process objects without texture data

  texture.Reset();

  for (it = obj_list.begin(); it != obj_list.end(); ++it)
    if (!(*it)->GetTexture()) {
      texture.AddNoTextureObject(*it);
    }

  if (texture.GetNumPlaced()) {
    if (pathlen) {
      memcpy(resfile, wildcard, pathlen);
    }
    resfile[pathlen] = 0;
    strcat(resfile, "NoTexture.res");

    texture.SaveDescriptions(resfile, nullptr, nullptr);

    SysLog("%d object(s) without texture stored.\n", texture.GetNumPlaced());
  }

  // process objects with texture data

  int nTexture = 1;

  for (;;) {
    texture.Reset();

    for (it = obj_list.begin(); it != obj_list.end(); ++it)
      if (!(*it)->IsPlaced()) {
        texture.PlaceObject(*it);
      }

    if (!texture.GetNumPlaced()) {
      break;
    }

    SysLog("Texture %d ... ", nTexture);

    sprintf(texfile, "%s%d.png", wildcard, nTexture);
    sprintf(resfile, "%s%d.res", wildcard, nTexture);
    sprintf(texname, "%s%d", name, nTexture);

    if (!texture.Create() ||
        !texture.CopyData() ||
        !texture.OptimizeAlpha() ||
        !texture.Save(texfile) ||
        !texture.SaveDescriptions(resfile, texfile, texname)) {
      texture.Reset();
      return false;
    }

    SysLog("%d object(s) placed.\n", texture.GetNumPlaced());

    nTexture++;
  }

  return true;
}
