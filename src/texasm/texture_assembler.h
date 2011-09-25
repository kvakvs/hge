#pragma once
#ifndef TEXTURE_ASSEMBLER_H
#define TEXTURE_ASSEMBLER_H


#include <hgeresource.h>
#include "gfx_object.h"
#include "optimized_texture.h"


class CTextureAssembler
{
public:
	~CTextureAssembler() { ClearResources(); }

	void AccumulateRMResources(hgeResourceManager *rm, int resgroup = 0, hgeConstString mask_set = 0, bool bMaskInclusive = false);
	void AccumulateFileResources(hgeConstString wildcard, int resgroup = 0, hgeConstString mask_set = 0, bool bMaskInclusive = false);
	void ClearResources();

	void SetMaxTextureSize(int w, int h) { texture.SetMaxSize(w, h); }
	void SetMargin(int margin) { texture.SetMargin(margin); }
	bool GenerateTextures(hgeConstString wildcard);

private:
	bool		CheckMask(hgeConstString name, hgeConstString mask_set, bool bMaskInclusive);
	CGfxObject *FindObj(GfxObjList objlist, hgeConstString name);

	GfxObjList			obj_list;
	COptimizedTexture	texture;
};


#endif
