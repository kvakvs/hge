/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once


//
// HGE Blending constants
//
enum hgeBlendMode : uint32_t {
    BLEND_COLORADD = 1 << 0, BLEND_COLORMUL = 0,
    BLEND_ALPHABLEND = 1 << 1, BLEND_ALPHAADD = 0,
    BLEND_ZWRITE = 1 << 2, BLEND_NOZWRITE = 0,

    // Darken does real color multiplication, white source pixels don't change destination, while
    // black source pixels make destination completely black
    // Use example: http://relishgames.com/forum/index.php?p=/discussion/5799/darken-screen-plus-uneffected-hole/p1
    BLEND_DARKEN = 1 << 3,
    BLEND_BLACKEN = BLEND_DARKEN, /* synonym for darken */
    BLEND_DEFAULT = (BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE),
    BLEND_DEFAULT_Z = (BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_ZWRITE),
};

//
// Hardware color inlines
//
// Expecting that compiler will inline this. Declared static to avoid duplicate
// function errors
//

struct hgeColor32 {
    // Stores color as 0xAA RR GG BB
    uint32_t argb{};

    constexpr hgeColor32() : argb(0) {}

    constexpr hgeColor32(uint32_t argb_) : argb(argb_) {} // NOLINT(google-explicit-constructor)

    constexpr hgeColor32(uint8_t a, uint8_t r, uint8_t g, uint8_t b) : argb(
            (uint32_t(a) << 24) + (uint32_t(r) << 16) + (uint32_t(g) << 8) + uint32_t(b)
    ) {}

    bool operator==(const hgeColor32 &other) const {
      return argb == other.argb;
    }

    bool operator!=(const hgeColor32 &other) const {
      return argb != other.argb;
    }

    static constexpr hgeColor32 WHITE() { return {0xFFFFFFFF}; }

    static constexpr hgeColor32 BLACK() { return {0xFF000000}; }

    static constexpr hgeColor32 TRANSPARENT_BLACK() { return {0x00000000}; }

    uint8_t get_a() const { return argb >> 24; }

    uint8_t get_r() const { return argb >> 16 & 0xFF; }

    uint8_t get_g() const { return argb >> 8 & 0xFF; }

    uint8_t get_b() const { return argb & 0xFF; }

    hgeColor32 set_a(uint8_t a) {
      argb = (argb & 0x00FFFFFF) + (uint32_t(a) << 24);
      return *this;
    }

    hgeColor32 set_r(uint8_t r) {
      argb = (argb & 0xFF00FFFF) + (uint32_t(r) << 16);
      return *this;
    }

    hgeColor32 set_g(uint8_t g) {
      argb = (argb & 0xFFFF00FF) + (uint32_t(g) << 8);
      return *this;
    }

    hgeColor32 set_b(uint8_t b) {
      argb = (argb & 0xFFFFFF00) + uint32_t(b);
      return *this;
    }

    hgeColor32 set_a(uint8_t a) const {
      return {(argb & 0x00FFFFFF) + (uint32_t(a) << 24)};
    }

    hgeColor32 set_r(uint8_t r) const {
      return {(argb & 0xFF00FFFF) + (uint32_t(r) << 16)};
    }

    hgeColor32 set_g(uint8_t g) const {
      return {(argb & 0xFFFF00FF) + (uint32_t(g) << 8)};
    }

    hgeColor32 set_b(uint8_t b) const {
      return {(argb & 0xFFFFFF00) + uint32_t(b)};
    }
};

// Legacy macros became functions and now delegate work to the hgeColor32 class
static hgeColor32 ARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b) { return {a, r, g, b}; }

static uint8_t GETA(const hgeColor32 col) { return col.get_a(); }

static uint8_t GETR(const hgeColor32 col) { return col.get_r(); }

static uint8_t GETG(const hgeColor32 col) { return col.get_g(); }

static uint8_t GETB(const hgeColor32 col) { return col.get_b(); }

static hgeColor32 SETA(hgeColor32 col, const uint8_t a) { return col.set_a(a); }

static hgeColor32 SETR(hgeColor32 col, const uint8_t r) { return col.set_r(r); }

static hgeColor32 SETG(hgeColor32 col, const uint8_t g) { return col.set_g(g); }

static hgeColor32 SETB(hgeColor32 col, const uint8_t b) { return col.set_b(b); }
