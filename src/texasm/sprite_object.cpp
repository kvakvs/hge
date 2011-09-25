
#include "sprite_object.h"

CSpriteObject::CSpriteObject(hgeSprite *_spr, hgeString _name, int _resgroup, bool _owned)
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


bool CSpriteObject::SaveDescription(FILE *fp, hgeConstString texname)
{
	hge_fprintf(fp, TXT("Sprite %s\n"), name);
	hge_fprintf(fp, TXT("{\n"));

	if(GetTexture() && texname)
		hge_fprintf(fp, TXT(" texture = %s\n"), texname);

	hge_fprintf(fp, TXT(" rect = %d,%d,%d,%d\n"), x, y, GetWidth(), GetHeight());

	float HotX, HotY;
	spr->GetHotSpot(&HotX, &HotY);
	if(HotX || HotY)
		hge_fprintf(fp, TXT(" hotspot = %d,%d\n"), int(HotX), int(HotY));

	int BlendMode = spr->GetBlendMode();
	if(BlendMode != BLEND_DEFAULT)
	{
		hge_fprintf(fp, TXT(" blendmode = "));

		if(BlendMode & BLEND_COLORADD) hge_fprintf(fp,TXT("COLORADD,"));
		else hge_fprintf(fp,TXT("COLORMUL,"));

		if(BlendMode & BLEND_ALPHABLEND) hge_fprintf(fp,TXT("ALPHABLEND,"));
		else hge_fprintf(fp,TXT("ALPHAADD,"));

		if(BlendMode & BLEND_ZWRITE) hge_fprintf(fp,TXT("ZWRITE\n"));
		else hge_fprintf(fp,TXT("NOZWRITE\n"));
	}

	uint32_t Color = spr->GetColor();
	if(Color != 0xFFFFFFFF)
		hge_fprintf(fp, TXT(" color = %08X\n"), Color);

	float ZOrder = spr->GetZ();
	if(ZOrder != 0.5f)
		hge_fprintf(fp, TXT(" zorder = %03f\n"), ZOrder);

	bool FlipX, FlipY;
	spr->GetFlip(&FlipX, &FlipY);
	if(FlipX || FlipY)
		hge_fprintf(fp, TXT(" flip = %s,%s\n"),
			FlipX ? TXT("true") : TXT("false"),
			FlipY ? TXT("true") : TXT("false")
			);

	hge_fprintf(fp, TXT(" resgroup = %d\n"), resgroup);
	
	hge_fprintf(fp, TXT("}\n"));

	return true;
}