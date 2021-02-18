/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once

#include <vector>
#include <hge.h>

enum {
    HGEDISP_NODE = 0,
    HGEDISP_TOPLEFT = 1,
    HGEDISP_CENTER = 2,
};

/*
** HGE Distortion mesh class
*/
class hgeDistortionMesh {
public:
    hgeDistortionMesh(int cols, int rows);

    hgeDistortionMesh(const hgeDistortionMesh &dm);

    ~hgeDistortionMesh();

    hgeDistortionMesh &operator=(const hgeDistortionMesh &dm);

    void Render(float x, float y);

    void Clear(hgeColor32 col, float z = 0.5f);

    void SetTexture(HTEXTURE tex);

    void SetTextureRect(float x, float y, float w, float h);

    void SetBlendMode(hgeBlendMode blend);

    void SetZ(int col, int row, float z);

    void SetColor(int col, int row, hgeColor32 color);

    void SetDisplacement(int col, int row, float dx, float dy, int ref);

    HTEXTURE GetTexture() const {
      return quad_.tex;
    }

    void GetTextureRect(float *x, float *y, float *w, float *h) const {
      *x = tx_;
      *y = ty_;
      *w = width_;
      *h = height_;
    }

    hgeBlendMode GetBlendMode() const {
      return quad_.blend;
    }

    float GetZ(int col, int row) const;

    uint32_t GetColor(int col, int row) const;

    void GetDisplacement(int col, int row, float *dx, float *dy, int ref) const;

    int GetRows() {
      return rows_;
    }

    int GetCols() {
      return cols_;
    }

private:
    hgeDistortionMesh() {}

    static HGE *hge_;

    std::vector<hgeVertex> disp_array_;
    int rows_;
    int cols_;
    float cellw_;
    float cellh_;
    float tx_;
    float ty_;
    float width_;
    float height_;
    hgeQuad quad_{};
};
