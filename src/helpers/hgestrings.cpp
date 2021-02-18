/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// hgeStringTable helper class implementation
//


#include <hgestrings.h>
#include <cctype>
#include <memory>

const char str_header_tag[] = "[HGESTRINGTABLE]";
const char str_format_error[] = "String table %s has incorrect format.";


HGE *hgeStringTable::hge_ = nullptr;


hgeStringTable::hgeStringTable(const char *filename) {
  uint32_t size;
  char str_name[MAXSTRNAMELENGTH];

  hge_ = hgeCreate(HGE_VERSION);

  // load string table file
  const auto data = hge_->Resource_Load(filename, &size);
  if (!data) {
    return;
  }

  std::unique_ptr<char[]> desc_holder(new char[size + 1]);
  const auto desc = desc_holder.get();

  memcpy(desc, data, size);
  desc[size] = 0;
  hge_->Resource_Free(data);

  // check header
  if (memcmp(desc, str_header_tag, sizeof(str_header_tag) - 1)) {
    hge_->System_Log(str_format_error, filename);
    return;
  }

  auto pdesc = desc + sizeof(str_header_tag);
  std::unique_ptr<char[]> str_value_holder(new char[8192]);
  const auto str_value = str_value_holder.get();

  for (;;) {
    // skip whitespaces
    while (isspace(*pdesc)) {
      pdesc++;
    }
    if (!*pdesc) {
      break;
    }

    // skip comments
    if (*pdesc == ';') {
      while (*pdesc && *pdesc != '\n') {
        pdesc++;
      }
      pdesc++;
      continue;
    }

    // get string name -> str_name
    int i = 0;
    while (pdesc[i] && pdesc[i] != '=' && !isspace(pdesc[i]) && i < MAXSTRNAMELENGTH) {
      str_name[i] = pdesc[i];
      i++;
    }
    str_name[i] = 0;
    pdesc += i;

    // skip string name overflow characters
    while (*pdesc && *pdesc != '=' && !isspace(*pdesc)) {
      pdesc++;
    }
    if (!*pdesc) {
      break;
    }

    // skip whitespaces to '='
    while (isspace(*pdesc)) {
      pdesc++;
    }
    if (*pdesc != '=') {
      hge_->System_Log(str_format_error, filename);
      break;
    }
    pdesc++;

    // skip whitespaces to '"'
    while (isspace(*pdesc)) {
      pdesc++;
    }
    if (*pdesc != '"') {
      hge_->System_Log(str_format_error, filename);
      break;
    }
    pdesc++;

    // parse string value till the closing '"' -> str_value
    // consider: \", \n, \\, LF, CR, whitespaces at line begin/end
    auto pvalue = str_value;

    while (*pdesc && *pdesc != '"') {
      if (*pdesc == '\n' || *pdesc == '\r') {
        while (isspace(*pdesc)) {
          pdesc++;
        }

        pvalue--;
        while (pvalue >= str_value && isspace(*pvalue)) {
          pvalue--;
        }
        pvalue++;
        *pvalue = ' ';
        pvalue++;

        continue;
      }

      if (*pdesc == '\\') {
        pdesc++;
        if (!*pdesc) {
          continue;
        }
        if (*pdesc == 'n') {
          *pvalue = '\n';
        } else {
          *pvalue = *pdesc;
        }
        pvalue++;
        pdesc++;
        continue;
      }

      *pvalue = *pdesc;
      pvalue++;
      pdesc++;
    }

    *pvalue = 0;

    // add the parsed string to the list
    strings_[str_name] = str_value;

    if (!*pdesc) {
      break;
    }
    pdesc++;
  }
}

hgeStringTable::~hgeStringTable() {
  strings_.clear();
  hge_->Release();
}

const char *hgeStringTable::GetString(const char *name) {
  const auto iter = strings_.find(name);
  if (iter == strings_.end()) {
    return nullptr;
  }
  return iter->second.c_str();
}
