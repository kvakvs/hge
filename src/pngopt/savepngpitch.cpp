/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#include "pngopt.h"

#include <png.h>

class PNGError {
};

void WarningCallback(png_structp png_ptr,
                     const png_const_charp msg) {
  hge->System_Log("LIBPNG Warning: %s", msg);
}

void ErrorCallback(png_structp png_ptr,
                   const png_const_charp msg) {
  hge->System_Log("LIBPNG Error: %s", msg);
  throw PNGError();
}

bool write32_bit_png_with_pitch(FILE *fp, void *p_bits, const bool b_need_alpha,
                                const int n_width, const int n_height,
                                const int pitch) {
  png_structp png_ptr = nullptr;
  png_infop info_ptr = nullptr;
  try {
    //	create png structs
    png_ptr = png_create_write_struct
            (PNG_LIBPNG_VER_STRING, nullptr,
             ErrorCallback, WarningCallback);
    if (png_ptr == nullptr) {
      return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
      return false;
    }

    //	init i/o
    png_init_io(png_ptr, fp);

    //	setup params
    if (b_need_alpha) {
      png_set_IHDR(png_ptr, info_ptr, n_width, n_height,
                   8,
                   PNG_COLOR_TYPE_RGB_ALPHA,
                   PNG_INTERLACE_NONE,
                   PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    } else {
      png_set_IHDR(png_ptr, info_ptr, n_width, n_height,
                   8,
                   PNG_COLOR_TYPE_RGB,
                   PNG_INTERLACE_NONE,
                   PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    }

    png_write_info(png_ptr, info_ptr);

    if (!b_need_alpha) {
      png_set_filler(png_ptr, 0, PNG_FILLER_AFTER); //	strip alpha
    }

    png_set_bgr(png_ptr); //	switch to little-endian notation

    //	writing
    auto pSrc = static_cast<unsigned char *>(p_bits);
    for (auto i = 0; i < n_height; i++, pSrc += pitch * 4) {
      png_write_row(png_ptr, pSrc);
    }

    png_write_end(png_ptr, info_ptr);
  }
  catch (PNGError) {
    png_destroy_write_struct(&png_ptr, (info_ptr == nullptr) ? nullptr : &info_ptr);
    return false;
  }

  //	cleanup
  png_destroy_write_struct(&png_ptr, (info_ptr == nullptr) ? nullptr : &info_ptr);
  return true;
}
