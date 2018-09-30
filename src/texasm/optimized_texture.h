#pragma once


#include "texasm.h"
#include "gfx_object.h"
#include "RectPlacement.h"


bool Write32BitPNGWithPitch(FILE* fp, void* pBits, bool bNeedAlpha, int nWidth, int nHeight,
                            int nPitch);


struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};


class COptimizedTexture {
public:
    COptimizedTexture();

    ~COptimizedTexture() {
        Reset();
    }

    void Reset();

    void SetMaxSize(const int w, const int h) {
        maxw = w;
        maxh = h;
    }

    void SetMargin(const int margin) {
        placer.SetMargin(margin);
    }

    bool PlaceObject(CGfxObject* obj);
    void AddNoTextureObject(CGfxObject* obj);

    int GetNumPlaced() {
        return obj_list.size();
    }

    bool Create();
    bool CopyData();
    bool OptimizeAlpha() const;
    bool Save(char* filename) const;
    bool SaveDescriptions(char* resfile, char* texfile, char* texname);

private:
    int maxw;
    int maxh;

    GfxObjList obj_list;
    CRectPlacement placer;
    int texw;
    int texh;

    HTEXTURE tex;
    CColor* tex_data;
    int pitch;
};
