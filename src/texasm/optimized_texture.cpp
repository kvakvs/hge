/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#include "optimized_texture.h"


COptimizedTexture::COptimizedTexture() {
  tex = 0;
  tex_data = nullptr;

  maxw = 1024;
  maxh = 1024;

  texw = 0;
  texh = 0;
  pitch = 0;
}


void COptimizedTexture::Reset() {
  obj_list.clear();
  placer.Init();

  texw = 0;
  texh = 0;
  pitch = 0;

  if (tex_data) {
    hge->Texture_Unlock(tex);
    tex_data = nullptr;
  }

  if (tex) {
    hge->Texture_Free(tex);
    tex = 0;
  }
}


bool COptimizedTexture::PlaceObject(CGfxObject *obj) {
  // find & reserve a place for the object

  CRectPlacement::TRect r(0, 0, obj->GetWidth(), obj->GetHeight());

  if (placer.AddAtEmptySpotAutoGrow(&r, maxw, maxh)) {
    obj->Place(r.x, r.y);
    obj_list.push_back(obj);
    return true;
  }

  return false;
}


void COptimizedTexture::AddNoTextureObject(CGfxObject *obj) {
  obj->Place(0, 0);
  obj_list.push_back(obj);
}


bool COptimizedTexture::Create() {
  // create texture
  texw = placer.GetW();
  texh = placer.GetH();

  tex = hge->Texture_Create(texw, texh);
  if (!tex) {
    SysLog("Can't create target texture.\n");
    return false;
  }

  // get hardware width
  pitch = hge->Texture_GetWidth(tex, false);

  // lock texture
  tex_data = reinterpret_cast<CColor *>(hge->Texture_Lock(tex));
  if (!tex_data) {
    SysLog("Can't lock target texture.\n");
    return false;
  }

  // clear texture
  memset(tex_data, 0, sizeof(CColor) * pitch * hge->Texture_GetHeight(tex, false));

  return true;
}

bool COptimizedTexture::CopyData() {

  int src_x, src_y;
  int tgt_x, tgt_y;

  for (GfxObjIterator it = obj_list.begin(); it != obj_list.end(); ++it) {
    CGfxObject *obj = *it;

    const auto source_tex = obj->GetTexture();
    const auto source_pitch = hge->Texture_GetWidth(source_tex, false);
    const auto source_data = reinterpret_cast<CColor *>(hge->Texture_Lock(source_tex, true));

    if (!source_data) {
      SysLog("Can't lock source texture for \"%s\"\n", obj->GetName());
      return false;
    }

    obj->GetSourcePos(&src_x, &src_y);
    obj->GetTargetPos(&tgt_x, &tgt_y);

    for (int i = 0; i < obj->GetHeight(); i++) {
      memcpy(&tex_data[tgt_y * pitch + tgt_x],
             &source_data[src_y * source_pitch + src_x],
             sizeof(CColor) * int(obj->GetWidth()));

      src_y++;
      tgt_y++;
    }

    hge->Texture_Unlock(source_tex);
  }

  return true;
}

bool COptimizedTexture::OptimizeAlpha() const {

  int g, b;

  if (!tex_data || !pitch) {
    SysLog("No texture generated to optimize alpha.\n");
    return false;
  }

  const auto buf = tex_data;

  for (auto i = 0; i < texh; i++) {
    for (auto j = 0; j < texw; j++) {
      if (!buf[i * pitch + j].a) {
        auto count = 0;
        auto r = g = b = 0;

        for (auto k = -1; k <= 1; k++)
          for (auto l = -1; l <= 1; l++)
            if (i + k >= 0 && i + k < texh &&
                j + l >= 0 && j + l < texw &&
                buf[(i + k) * pitch + (j + l)].a) {
              r += buf[(i + k) * pitch + (j + l)].r;
              g += buf[(i + k) * pitch + (j + l)].g;
              b += buf[(i + k) * pitch + (j + l)].b;
              count++;
            }

        if (count) {
          buf[i * pitch + j].r = unsigned char(r / count);
          buf[i * pitch + j].g = unsigned char(g / count);
          buf[i * pitch + j].b = unsigned char(b / count);
        }
      }
    }
  }

  return true;
}

bool COptimizedTexture::Save(char *filename) const {

  if (!tex_data || !pitch) {
    SysLog("No texture generated to save: %s\n", filename);
    return false;
  }

  const auto fp = fopen(filename, "wb");
  if (!fp) {
    SysLog("Can't create texture file: %s\n", filename);
    return false;
  }

  if (!write32_bit_png_with_pitch(fp, tex_data, true, texw, texh, pitch)) {
    fclose(fp);
    SysLog("Error writing data: %s\n", filename);
    return false;
  }

  fclose(fp);
  return true;
}

bool COptimizedTexture::SaveDescriptions(char *resfile, char *texfile, char *texname) {

  // check for data to be available
  if (!GetNumPlaced()) {
    SysLog("No descriptions to save: %s\n", resfile);
    return false;
  }

  // create resource file
  const auto fp = fopen(resfile, "w");
  if (!fp) {
    SysLog("Can't create description file: %s\n", resfile);
    return false;
  }

  // save texture description
  if (texfile && texname) {
    fprintf(fp, "Texture %s\n", texname);
    fprintf(fp, "{\n");
    fprintf(fp, " filename = \"%s\"\n", texfile);
    fprintf(fp, " resgroup = %d\n", (*obj_list.begin())->GetResGroup());
    fprintf(fp, "}\n\n");
  }

  // save object descriptions
  for (auto &it: obj_list) {
    if (!it->SaveDescription(fp, texname)) {
      fclose(fp);
      SysLog("Error writing description: %s\n", it->GetName());
      return false;
    }

    fprintf(fp, "\n");
  }

  fclose(fp);
  return true;
}
