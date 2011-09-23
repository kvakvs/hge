
#ifndef TEXTURE_ASSEMBLER_H
#define TEXTURE_ASSEMBLER_H


#include "..\..\include\hgeresource.h"
#include "gfx_object.h"
#include "optimized_texture.h"


class CTextureAssembler
{
public:
	~CTextureAssembler() { ClearResources(); }

	void AccumulateRMResources(hgeResourceManager *rm, int resgroup = 0, char *mask_set = 0, bool bMaskInclusive = false);
	void AccumulateFileResources(char *wildcard, int resgroup = 0, char *mask_set = 0, bool bMaskInclusive = false);
	void ClearResources();

	void SetMaxTextureSize(int w, int h) { texture.SetMaxSize(w, h); }
	void SetMargin(int margin) { texture.SetMargin(margin); }
	bool GenerateTextures(char *wildcard);

private:
	bool		CheckMask(char *name, char *mask_set, bool bMaskInclusive);
	CGfxObject *FindObj(GfxObjList objlist, char *name);

	GfxObjList			obj_list;
	COptimizedTexture	texture;
};


#endif
