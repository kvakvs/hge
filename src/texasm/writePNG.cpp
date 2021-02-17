/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#include "texasm.h"
#include <png.h>
#include <exception>

class PNGError: public std::exception {
};

void WarningCallback(png_structp png_ptr,
                     const png_const_charp msg) {
  SysLog("LIBPNG Warning: %s\n", msg);
}

void ErrorCallback(png_structp png_ptr,
                   const png_const_charp msg) {
  SysLog("LIBPNG Error: %s\n", msg);
  throw PNGError();
}

bool write32_bit_png_with_pitch(FILE *fp, void *p_bits, const bool need_alpha,
                                const int width, const int height,
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
    if (need_alpha) {
      png_set_IHDR(png_ptr, info_ptr, width, height,
                   8,
                   PNG_COLOR_TYPE_RGB_ALPHA,
                   PNG_INTERLACE_NONE,
                   PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    } else {
      png_set_IHDR(png_ptr, info_ptr, width, height,
                   8,
                   PNG_COLOR_TYPE_RGB,
                   PNG_INTERLACE_NONE,
                   PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    }

    png_write_info(png_ptr, info_ptr);

    if (!need_alpha) {
      png_set_filler(png_ptr, 0, PNG_FILLER_AFTER); //	strip alpha
    }

    png_set_bgr(png_ptr); //	switch to little-endian notation

    //	writing
    auto p_src = static_cast<unsigned char *>(p_bits);
    for (auto i = 0; i < height; i++, p_src += pitch * 4) {
      png_write_row(png_ptr, p_src);
    }

    png_write_end(png_ptr, info_ptr);
  }
  catch (PNGError) {
    png_destroy_write_struct(&png_ptr, (info_ptr == nullptr) ? NULL : &info_ptr);
    return false;
  }

  //	cleanup
  png_destroy_write_struct(&png_ptr, (info_ptr == nullptr) ? NULL : &info_ptr);
  return true;
}
