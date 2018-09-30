#pragma once



#include "gfx_object.h"
#include "../../include/hgesprite.h"


class CSpriteObject : public CGfxObject {
public:
    CSpriteObject(hgeSprite* _spr, char* _name, int _resgroup, bool _owned);
    ~CSpriteObject();

    int GetWidth() const override {
        return spr ? int(spr->GetWidth()) : 0;
    }

    int GetHeight() const override {
        return spr ? int(spr->GetHeight()) : 0;
    }

    HTEXTURE GetTexture() override {
        return spr ? spr->GetTexture() : 0;
    }

    void GetSourcePos(int* _x, int* _y) override;
    bool SaveDescription(FILE* fp, char* texname) override;

private:
    hgeSprite* spr;
    bool owned;
};
