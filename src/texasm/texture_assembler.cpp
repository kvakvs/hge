#include "texture_assembler.h"
#include "sprite_object.h"

#include <hgesprite.h>



void CTextureAssembler::ClearResources()
{
	GfxObjIterator it;

	for(it = obj_list.begin( ); it != obj_list.end( ); ++it)
		delete *it;

	obj_list.clear();
}


bool CTextureAssembler::CheckMask(hgeConstString name, hgeConstString mask_set, bool bMaskInclusive)
{
	int mask_len;
	int name_len;
	bool match;

	if(!mask_set || !*mask_set) return true;

	hgeConstString mask = mask_set;
	name_len = hge_strlen(name);

	while(*mask)
	{
		mask_len = hge_strlen(mask);

		match = (name_len >= mask_len) && !memcmp(name, mask, mask_len * sizeof(hgeChar));

		if(match && bMaskInclusive) return true;
		if(!match && !bMaskInclusive) return true;

		mask += hge_strlen(mask) + 1;
	}

	return false;
}


CGfxObject *CTextureAssembler::FindObj(GfxObjList objlist, hgeConstString name)
{
	GfxObjIterator it;

	for(it = obj_list.begin( ); it != obj_list.end( ); ++it)
		if( ! hge_strcmp((*it)->GetName(), name)) return *it;

	return 0;
}


void CTextureAssembler::AccumulateRMResources(hgeResourceManager *rm, int resgroup, hgeConstString mask_set, bool bMaskInclusive)
{
	ResDesc *resdesc;
	
	// RES_SPRITE
	resdesc = rm->res[7];

	while(resdesc)
	{
		if(!resgroup || resdesc->resgroup == resgroup)
			if(CheckMask(resdesc->name, mask_set, bMaskInclusive))
				if(!FindObj(obj_list, resdesc->name))
		{
			obj_list.push_back(new CSpriteObject((hgeSprite *)resdesc->Get(rm), resdesc->name, resdesc->resgroup, false));
		}

		resdesc = resdesc->next;
	}
}


void CTextureAssembler::AccumulateFileResources( hgeConstString wildcard,
	int resgroup, hgeConstString mask_set, bool bMaskInclusive)
{
	HANDLE				hSearch;
	HGE_WINAPI_UNICODE_SUFFIX(WIN32_FIND_DATA)	SearchData;
	hgeChar				filename[MAX_PATH];
	hgeChar				temp[MAX_PATH];
	hgeChar				*buf;

	HTEXTURE			tex;
	hgeSprite			*spr;


	hSearch = HGE_WINAPI_UNICODE_SUFFIX(FindFirstFile)(wildcard, &SearchData);

	if(hSearch == INVALID_HANDLE_VALUE)
	{
		SysLog( TXT("Can't find the path: %s\n"), wildcard);
		return;
	}

	do
	{
		// recurse subfolders

		if(SearchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(SearchData.cFileName[0] == '.') continue;

			hge_strcpy(filename, wildcard);
			buf = hge_strrchr(filename, '\\');
			if(!buf) buf=filename; else buf++;

			hge_strcpy(temp, buf);
			hge_strcpy(buf, SearchData.cFileName);
			hge_strcat(filename, TXT("\\") );
			hge_strcat(filename, temp);

			AccumulateFileResources(filename, resgroup, mask_set, bMaskInclusive);
		}

		// process a file

		else
		{
			if(CheckMask(SearchData.cFileName, mask_set, bMaskInclusive))
				if(!FindObj(obj_list, SearchData.cFileName))
				{
					hge_strcpy(filename, wildcard);
					buf=hge_strrchr(filename, '\\');
					if(!buf) buf=filename; else buf++;
					hge_strcpy(buf, SearchData.cFileName);

					tex = hge->Texture_Load(filename);
					if(!tex)
					{
						SysLog( TXT("Can't load texture: %s\n"), filename);
						continue;
					}

					spr = new hgeSprite(tex, 0, 0,
										(float)hge->Texture_GetWidth(tex, true),
										(float)hge->Texture_GetHeight(tex, true));

					buf = new hgeChar[hge_strlen(SearchData.cFileName)+1];
					hge_strcpy(buf, SearchData.cFileName);

					obj_list.push_back(new CSpriteObject(spr, buf, resgroup, true));
				}
		}

	} while(
		HGE_WINAPI_UNICODE_SUFFIX(FindNextFile)(hSearch, &SearchData)
		);

	FindClose(hSearch);
}


bool CTextureAssembler::GenerateTextures(hgeConstString wildcard)
{
	GfxObjIterator it;

	hgeChar texfile[MAX_PATH];
	hgeChar resfile[MAX_PATH];
	hgeChar texname[128];

	int pathlen;

	int nTexture;

	// extract texture name from wildcard

	hgeConstString name = hge_strrchr(wildcard, TXT('\\'));

	if(name != NULL)	name++;
				else	name = wildcard;

	pathlen = name - wildcard;

	if(! hge_strlen(name) )
	{
		SysLog( TXT("Invalid wildcard: %s\n"), wildcard );
		return false;
	}

	// check if we have something to process

	if(!obj_list.size())
	{
		SysLog( TXT("No resources in task.\n") );
		return false;
	}

	// sort objects

	obj_list.sort(ByLargestDimensionDescending());

	// process objects without texture data

	texture.Reset();

	for(it = obj_list.begin(); it != obj_list.end(); ++it)
		if(!(*it)->GetTexture())
			texture.AddNoTextureObject(*it);

	if(texture.GetNumPlaced())
	{
		if(pathlen)	memcpy(resfile, wildcard, pathlen);
		resfile[pathlen] = 0;
		hge_strcat(resfile, TXT("NoTexture.res") );

		texture.SaveDescriptions(resfile, 0, 0);

		SysLog( TXT("%d object(s) without texture stored.\n"), texture.GetNumPlaced());
	}

	// process objects with texture data

	nTexture = 1;

	for(;;) 
	{
		texture.Reset();

		for(it = obj_list.begin(); it != obj_list.end(); ++it)
			if(!(*it)->IsPlaced())
				texture.PlaceObject(*it);

		if(!texture.GetNumPlaced()) break;

		SysLog( TXT("Texture %d ... "), nTexture);

		hge_sprintf(texfile, sizeof texfile, TXT("%s%d.png"), wildcard, nTexture);
		hge_sprintf(resfile, sizeof resfile, TXT("%s%d.res"), wildcard, nTexture);
		hge_sprintf(texname, sizeof texname, TXT("%s%d"), name, nTexture);

		if(!texture.Create() ||
		   !texture.CopyData() ||
		   !texture.OptimizeAlpha() ||
		   !texture.Save(texfile) ||
		   !texture.SaveDescriptions(resfile, texfile, texname))
		{
			texture.Reset();
			return false;
		}

		SysLog( TXT("%d object(s) placed.\n"), texture.GetNumPlaced());

		nTexture++;
	}

	return true;
}

