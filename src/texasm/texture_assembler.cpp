#include "texture_assembler.h"
#include "sprite_object.h"

#include "hgesprite.h"


void CTextureAssembler::ClearResources() {

    for (GfxObjIterator it = obj_list.begin(); it != obj_list.end(); ++it) {
        delete *it;
    }

    obj_list.clear();
}


bool CTextureAssembler::CheckMask(char* name, char* mask_set, bool bMaskInclusive) {

    if (!mask_set || !*mask_set) {
        return true;
    }

    char* mask = mask_set;
    int name_len = strlen(name);

    while (*mask) {
        int mask_len = strlen(mask);

        bool match = (name_len >= mask_len) && !memcmp(name, mask, mask_len);

        if (match && bMaskInclusive) {
            return true;
        }
        if (!match && !bMaskInclusive) {
            return true;
        }

        mask += strlen(mask) + 1;
    }

    return false;
}


CGfxObject* CTextureAssembler::FindObj(GfxObjList objlist, char* name) {

    for (GfxObjIterator it = obj_list.begin(); it != obj_list.end(); it++)
        if (!strcmp((*it)->GetName(), name)) {
            return *it;
        }

    return nullptr;
}


void CTextureAssembler::AccumulateRMResources(hgeResourceManager* rm, int resgroup, char* mask_set,
                                              bool bMaskInclusive) {

    // RES_SPRITE
    ResDesc* resdesc = rm->res[7];

    while (resdesc) {
        if (!resgroup || resdesc->resgroup == resgroup)
            if (CheckMask(resdesc->name, mask_set, bMaskInclusive))
                if (!FindObj(obj_list, resdesc->name)) {
                    obj_list.push_back(new CSpriteObject((hgeSprite *)resdesc->Get(rm),
                                                         resdesc->name,
                                                         resdesc->resgroup, false));
                }

        resdesc = resdesc->next;
    }
}


void CTextureAssembler::AccumulateFileResources(char* wildcard, int resgroup, char* mask_set,
                                                bool bMaskInclusive) {
    WIN32_FIND_DATA SearchData;
    char filename[MAX_PATH];
    char temp[MAX_PATH];
    char* buf;


    HANDLE hSearch = FindFirstFile(wildcard, &SearchData);

    if (hSearch == INVALID_HANDLE_VALUE) {
        SysLog("Can't find the path: %s\n", wildcard);
        return;
    }

    do {
        // recurse subfolders

        if (SearchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (SearchData.cFileName[0] == '.') {
                continue;
            }

            strcpy(filename, wildcard);
            buf = strrchr(filename, '\\');
            if (!buf) {
                buf = filename;
            }
            else {
                buf++;
            }

            strcpy(temp, buf);
            strcpy(buf, SearchData.cFileName);
            strcat(filename, "\\");
            strcat(filename, temp);

            AccumulateFileResources(filename, resgroup, mask_set, bMaskInclusive);
        }

            // process a file

        else {
            if (CheckMask(SearchData.cFileName, mask_set, bMaskInclusive))
                if (!FindObj(obj_list, SearchData.cFileName)) {
                    strcpy(filename, wildcard);
                    buf = strrchr(filename, '\\');
                    if (!buf) {
                        buf = filename;
                    }
                    else {
                        buf++;
                    }
                    strcpy(buf, SearchData.cFileName);

                    HTEXTURE tex = hge->Texture_Load(filename);
                    if (!tex) {
                        SysLog("Can't load texture: %s\n", filename);
                        continue;
                    }

                    hgeSprite* spr = new hgeSprite(tex, 0, 0,
                                                   (float)hge->Texture_GetWidth(tex, true),
                                                   (float)hge->Texture_GetHeight(tex, true));

                    buf = new char[strlen(SearchData.cFileName) + 1];
                    strcpy(buf, SearchData.cFileName);

                    obj_list.push_back(new CSpriteObject(spr, buf, resgroup, true));
                }
        }

    }
    while (FindNextFile(hSearch, &SearchData));

    FindClose(hSearch);
}


bool CTextureAssembler::GenerateTextures(char* wildcard) {
    GfxObjIterator it;

    char texfile[MAX_PATH];
    char resfile[MAX_PATH];
    char texname[128];

    // extract texture name from wildcard

    char* name = strrchr(wildcard, '\\');

    if (name != nullptr) {
        name++;
    }
    else {
        name = wildcard;
    }

    int pathlen = name - wildcard;

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

    for (it = obj_list.begin(); it != obj_list.end(); it++)
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

        for (it = obj_list.begin(); it != obj_list.end(); it++)
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
