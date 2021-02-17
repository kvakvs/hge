/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// PNG Images Optimizer
//


#include "pngopt.h"

HGE *hge = nullptr;


struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};


struct filelist {
    char filename[256];
    filelist *next;
};

filelist *files = nullptr;


extern bool
write32_bit_png_with_pitch(FILE *fp, void *pBits, bool bNeedAlpha, int nWidth, int nHeight,
                           int nPitch);

bool convert(char *filename);


int main(int argc, char *argv[]) {
  WIN32_FIND_DATA SearchData;
  int nfiles = 0;
  bool done = false;
  char filename[256];
  filelist *newFile;

  printf("\nPNG Optimizer v0.2\nCopyright (C) 2003-2008, Relish Games\n\n");

  if (argc != 2) {
    printf("Usage: PNGOPT.EXE <wildcard>\n\n");
    printf("The image data of all files found by the specified wildcard\n");
    printf("will be expanded beneath the alpha channel boundary.\n");
    printf("So images will render without artifacts when scaled, stretched,\n");
    printf("rotated or rendered into not integer coordinates.\n\n");
    return 0;
  }
  const auto h_search = FindFirstFile(argv[1], &SearchData);
  filelist *nextFile = nullptr;

  for (;;) {
    if (h_search == INVALID_HANDLE_VALUE || done) {
      FindClose(h_search);
      break;
    }

    if (!(SearchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      strcpy(filename, argv[1]);
      char *buf = strrchr(filename, '\\');
      if (!buf) {
        buf = filename;
      } else {
        buf++;
      }
      strcpy(buf, SearchData.cFileName);
      newFile = new filelist;
      strcpy(newFile->filename, filename);
      newFile->next = nullptr;
      if (nextFile) {
        nextFile->next = newFile;
      } else {
        files = newFile;
      }
      nextFile = newFile;
    }

    done = !FindNextFile(h_search, &SearchData);
  }

  hge = hgeCreate(HGE_VERSION);
  hge->System_SetState(HGE_USESOUND, false);
  hge->System_SetState(HGE_WINDOWED, true);
  hge->System_SetState(HGE_SCREENWIDTH, 320);
  hge->System_SetState(HGE_SCREENHEIGHT, 200);
  hge->System_SetState(HGE_SHOWSPLASH, false);

  if (!hge->System_Initiate()) {
    hge->Release();
    printf("\nCan't initiate HGE [nfiles=%d].\n\n", nfiles);
    return 0;
  }

  newFile = files;
  while (newFile) {
    if (convert(newFile->filename)) {
      nfiles++;
    }
    nextFile = newFile->next;
    delete newFile;
    newFile = nextFile;
  }

  hge->System_Shutdown();
  hge->Release();
  printf("\n%d image(s) successfully optimized.\n\n", nfiles);
  return 0;
}

bool convert(char *filename) {

  int g, b;

  printf("%s - ", filename);

  const auto tex = hge->Texture_Load(filename);
  if (!tex) {
    printf("Can't load texture.\n");
    return false;
  }

  const auto width = hge->Texture_GetWidth(tex, true);
  const auto height = hge->Texture_GetHeight(tex, true);
  const auto pitch = hge->Texture_GetWidth(tex, false);

  color *buf = reinterpret_cast<color *>(hge->Texture_Lock(tex, false));
  if (!buf) {
    printf("Can't lock texture.\n");
    return false;
  }

  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      if (!buf[i * pitch + j].a) {
        int count = 0;
        int r = g = b = 0;

        for (int k = -1; k <= 1; k++)
          for (int l = -1; l <= 1; l++)
            if (i + k >= 0 && i + k < height &&
                j + l >= 0 && j + l < width &&
                buf[(i + k) * pitch + (j + l)].a) {
              r += buf[(i + k) * pitch + (j + l)].r;
              g += buf[(i + k) * pitch + (j + l)].g;
              b += buf[(i + k) * pitch + (j + l)].b;
              count++;
            }

        if (count) {
          buf[i * pitch + j].r = static_cast<unsigned char>(r / count);
          buf[i * pitch + j].g = static_cast<unsigned char>(g / count);
          buf[i * pitch + j].b = static_cast<unsigned char>(b / count);
        }
      }


  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    hge->Texture_Unlock(tex);
    printf("Can't write to file.\n");
    return false;
  }

  if (!write32_bit_png_with_pitch(fp, buf, true, width, height, pitch)) {
    hge->Texture_Unlock(tex);
    fclose(fp);
    printf("Error writing data.\n");
    return false;
  }

  fclose(fp);
  hge->Texture_Unlock(tex);

  printf("Ok\n");
  return true;
}
