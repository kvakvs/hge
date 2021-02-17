/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// HGE Font Description files 1.XX -> 1.6 converter
//

#include <cstdio>
#include <cstdlib>
#include <windows.h>

#include <hge.h>

HGE *hge = nullptr;


struct filelist {
    char filename[256];
    filelist *next;
};

filelist *files = nullptr;


bool convert(char *filename);

char *_skip_token(char *szStr);


int main(const int argc, char *argv[]) {
  WIN32_FIND_DATA search_data;
  auto nfiles = 0;
  auto done = false;
  char filename[256];
  filelist *new_file;

  printf("\nHGE Font 1.XX -> 1.6 converter\nCopyright (C) 2003-2006, Relish Games\n\n");

  if (argc != 2) {
    printf("Usage: FONTCONV.EXE <wildcard>\n\n");
    printf("All files found by the specified wildcard will\n");
    printf("be automatically converted to newer format. Bitmap files\n");
    printf("should be available along with font description files.\n\n");
    return 0;
  }
  const auto h_search = FindFirstFile(argv[1], &search_data);
  filelist *nextFile = nullptr;

  for (;;) {
    if (h_search == INVALID_HANDLE_VALUE || done) {
      FindClose(h_search);
      break;
    }

    if (!(search_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      strcpy(filename, argv[1]);
      char *buf = strrchr(filename, '\\');
      if (!buf) {
        buf = filename;
      } else {
        buf++;
      }
      strcpy(buf, search_data.cFileName);
      new_file = new filelist;
      strcpy(new_file->filename, filename);
      new_file->next = nullptr;
      if (nextFile) {
        nextFile->next = new_file;
      } else {
        files = new_file;
      }
      nextFile = new_file;
    }

    done = !FindNextFile(h_search, &search_data);
  }

  hge = hgeCreate(HGE_VERSION);
  hge->System_SetState(HGE_USESOUND, false);
  hge->System_SetState(HGE_WINDOWED, true);
  hge->System_SetState(HGE_SCREENWIDTH, 640);
  hge->System_SetState(HGE_SCREENHEIGHT, 480);

  if (!hge->System_Initiate()) {
    hge->Release();
    printf("\nCan't initiate HGE [nfiles=%d].\n\n", nfiles);
    return 0;
  }

  new_file = files;
  while (new_file) {
    if (convert(new_file->filename)) {
      nfiles++;
    }
    nextFile = new_file->next;
    delete new_file;
    new_file = nextFile;
  }

  hge->System_Shutdown();
  hge->Release();
  printf("\n%d file(s) successfully converted.\n\n", nfiles);
  return 0;
}

bool convert(char *filename) {
  static char signature1[] = "[HGEFONT]";
  static char signature2[] = "[hgefont]";
  static char tempfile[] = "tmpfont.tmp";

  char strbuf[256], texname[256];
  int height, texy;

  printf("%s - ", filename);
  FILE *hf = fopen(filename, "r");
  if (hf == nullptr) {
    printf("Can't open file\n");
    return false;
  }
  fseek(hf, 0, SEEK_END);
  const auto size = ftell(hf);
  rewind(hf);
  char *desc = static_cast<char *>(malloc(size));
  if (desc == nullptr) {
    printf("Can't allocate buffer\n");
    fclose(hf);
    return false;
  }
  fread(desc, 1, size, hf);
  fclose(hf);
  if (!strncmp(desc, signature1, strlen(signature1)) ||
      !strncmp(desc, signature2, strlen(signature2))) {
    printf("Already converted\n");
    return false;
  }

  char *pdesc = desc;
  hf = fopen(tempfile, "w");
  if (hf == nullptr) {
    printf("Can't create temporary file\n");
    free(desc);
    return false;
  }

  fprintf(hf, "[HGEFONT]\n\n");

  sscanf(pdesc, " %s", strbuf);
  fprintf(hf, "Bitmap=%s\n\n", strbuf);
  pdesc = _skip_token(pdesc);

  strcpy(texname, filename);
  char *pbuf = strrchr(texname, '\\');
  if (!pbuf) {
    pbuf = texname;
  } else {
    pbuf++;
  }
  strcpy(pbuf, strbuf);

  const auto htex = hge->Texture_Load(texname);
  if (!htex) {
    printf("Can't load bitmap\n");
    fclose(hf);
    free(desc);
    return false;
  }
  const auto tex_width = hge->Texture_GetWidth(htex);
  printf("%s %dx%d - ", texname, tex_width, hge->Texture_GetWidth(htex));
  hge->Texture_Free(htex);
  int texx = texy = 0;

  sscanf(pdesc, " %d", &height);
  //fprintf(hf,"Height=%d\n",height);
  pdesc = _skip_token(pdesc);

  for (int i = 0; i < 256; i++) {
    int intbuf = 0;
    sscanf(pdesc, " %d", &intbuf);
    if (!intbuf) {
      break;
    }
    pdesc = _skip_token(pdesc);
    if (intbuf < 0) {
      i += abs(intbuf) - 1;
    } else {
      if (texx + intbuf > tex_width) {
        texy += height;
        texx = 0;
      }
      if (i >= 32 && i <= 126) {
        fprintf(hf, "Char=\"%c\",%d,%d,%d,%d,0,0\n", static_cast<char>(i),
                texx, texy, intbuf, height);
      } else {
        fprintf(hf, "Char=%2X,%d,%d,%d,%d\n", i, texx, texy, intbuf, height);
      }
      texx += intbuf;
    }
  }

  fclose(hf);
  free(desc);

  if (!DeleteFile(filename)) {
    printf("Can't replace file\n");
    return false;
  }
  if (!MoveFile(tempfile, filename)) {
    printf("Sorry! Due to system failure the file seems lost\n");
    return false;
  }
  printf("Ok\n");

  return true;
}


char *_skip_token(char *szStr) {
  while (*szStr && (*szStr == ' ' || *szStr == '\t' || *szStr == '\n' || *szStr == '\r')) {
    szStr++;
  }
  while (*szStr && (*szStr != ' ' && *szStr != '\t' && *szStr != '\n' && *szStr != '\r')) {
    szStr++;
  }
  return szStr;
}
