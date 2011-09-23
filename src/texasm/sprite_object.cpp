
#include "sprite_object.h"

CSpriteObject::CSpriteObject(hgeSprite *_spr, char *_name, int _resgroup, bool _owned)
	: CGfxObject(_name, _resgroup)
{
	owned	 = _owned;
	spr		 = _spr;
}

CSpriteObject::~CSpriteObject()
{
	if(owned)
	{
		hge->Texture_Free(GetTexture());
		delete[] name;
		delete spr;
	}
}


void CSpriteObject::GetSourcePos(int *_x, int *_y)
{
	float x, y, w, h;

	spr->GetTextureRect(&x, &y, &w, &h);

	*_x = int(x);
	*_y = int(y);
}


bool CSpriteObject::SaveDescription(FILE *fp, char *texname)
{
	fprintf(fp, "Sprite %s\n", name);
	fprintf(fp, "{\n");

	if(GetTexture() && texname)
		fprintf(fp, " texture = %s\n", texname);

	fprintf(fp, " rect = %d,%d,%d,%d\n", x, y, GetWidth(), GetHeight());

	float HotX, HotY;
	spr->GetHotSpot(&HotX, &HotY);
	if(HotX || HotY)
		fprintf(fp, " hotspot = %d,%d\n", int(HotX), int(HotY));

	int BlendMode = spr->GetBlendMode();
	if(BlendMode != BLEND_DEFAULT)
	{
		fprintf(fp, " blendmode = ");

		if(BlendMode & BLEND_COLORADD) fprintf(fp,"COLORADD,");
		else fprintf(fp,"COLORMUL,");

		if(BlendMode & BLEND_ALPHABLEND) fprintf(fp,"ALPHABLEND,");
		else fprintf(fp,"ALPHAADD,");

		if(BlendMode & BLEND_ZWRITE) fprintf(fp,"ZWRITE\n");
		else fprintf(fp,"NOZWRITE\n");
	}

	DWORD Color = spr->GetColor();
	if(Color != 0xFFFFFFFF)
		fprintf(fp, " color = %08X\n", Color);

	float ZOrder = spr->GetZ();
	if(ZOrder != 0.5f)
		fprintf(fp, " zorder = %03f\n", ZOrder);

	bool FlipX, FlipY;
	spr->GetFlip(&FlipX, &FlipY);
	if(FlipX || FlipY)
		fprintf(fp, " flip = %s,%s\n", FlipX ? "true" : "false", FlipY ? "true" : "false");

	fprintf(fp, " resgroup = %d\n", resgroup);
	
	fprintf(fp, "}\n");

	return true;
}