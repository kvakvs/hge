/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// Core functions implementation: resources management
//


#include "hge_impl.h"

#define NOCRYPT
//#define NOUNCRYPT
#include <unzip.h>

namespace hgeImpl {

// NOLINTNEXTLINE
    bool HGE_CALL HGE_Impl::Resource_AttachPack(const char *filename, const char *password) {
//      auto res_item = res_list_;

      char sz_name[MAX_PATH];
      strcpy(sz_name, Resource_MakePath(filename));
      strupr(sz_name);

      // Search if already attached
      if (std::any_of(res_list_.begin(),
                      res_list_.end(),
                      [sz_name](const ResourcePackInfo &i) { return i.filename == sz_name; })) {
        return false;
      }

      const auto zip = unzOpen(sz_name);
      if (!zip) {
        return false;
      }
      unzClose(zip);

      res_list_.emplace_back(sz_name, password);
      return true;
    }

    void HGE_CALL HGE_Impl::Resource_RemovePack(const char *filename) {
      char sz_name[MAX_PATH];
      strcpy(sz_name, Resource_MakePath(filename));
      strupr(sz_name);

      auto _ = std::remove_if(
              res_list_.begin(),
              res_list_.end(),
              [sz_name](ResourcePackInfo &p) { return p.filename == sz_name; });
    }

    void HGE_CALL HGE_Impl::Resource_RemoveAllPacks() {
      res_list_.clear();
    }

// NOLINTNEXTLINE
    void *HGE_CALL HGE_Impl::Resource_Load(const char *filename, uint32_t *size) {
      static const char *res_err = "Can't load resource: %s";

      char sz_name[_MAX_PATH];
      char sz_zip_name[_MAX_PATH];
      unz_file_info file_info;
      int i;
      void *ptr;

      if (filename[0] == '\\' || filename[0] == '/' || filename[1] == ':') {
        goto _fromfile; // skip absolute paths
      }

      // Load from pack

      strcpy(sz_name, filename);
      strupr(sz_name);
      for (i = 0; sz_name[i]; i++) {
        if (sz_name[i] == '/') {
          sz_name[i] = '\\';
        }
      }

      for (const auto &res_item: res_list_) {
        const auto zip = unzOpen(res_item.filename.c_str());
        auto done = unzGoToFirstFile(zip);
        while (done == UNZ_OK) {
          unzGetCurrentFileInfo(zip, &file_info, sz_zip_name, sizeof(sz_zip_name), nullptr, 0,
                                nullptr, 0);
          strupr(sz_zip_name);
          for (i = 0; sz_zip_name[i]; i++) {
            if (sz_zip_name[i] == '/') {
              sz_zip_name[i] = '\\';
            }
          }
          if (!strcmp(sz_name, sz_zip_name)) {
            if (unzOpenCurrentFilePassword(
                    zip,
                    res_item.password.empty() ? nullptr
                                              : res_item.password.c_str()
            ) != UNZ_OK) {
              unzClose(zip);
              sprintf(sz_name, res_err, filename);
              post_error(sz_name);
              return nullptr;
            }

            ptr = malloc(file_info.uncompressed_size);
            if (!ptr) {
              unzCloseCurrentFile(zip);
              unzClose(zip);
              sprintf(sz_name, res_err, filename);
              post_error(sz_name);
              return nullptr;
            }

            if (unzReadCurrentFile(zip, ptr, file_info.uncompressed_size) < 0) {
              unzCloseCurrentFile(zip);
              unzClose(zip);
              free(ptr);
              sprintf(sz_name, res_err, filename);
              post_error(sz_name);
              return nullptr;
            }
            unzCloseCurrentFile(zip);
            unzClose(zip);
            if (size) {
              *size = file_info.uncompressed_size;
            }
            return ptr;
          }

          done = unzGoToNextFile(zip);
        }

        unzClose(zip);
      }

      // Load from file
      _fromfile:
      const auto h_f = CreateFile(Resource_MakePath(filename), GENERIC_READ,
                                  FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
                                  nullptr);
      if (h_f == INVALID_HANDLE_VALUE) {
        sprintf(sz_name, res_err, filename);
        post_error(sz_name);
        return nullptr;
      }
      file_info.uncompressed_size = GetFileSize(h_f, nullptr);
      ptr = malloc(file_info.uncompressed_size);
      if (!ptr) {
        CloseHandle(h_f);
        sprintf(sz_name, res_err, filename);
        post_error(sz_name);
        return nullptr;
      }
      if (ReadFile(h_f, ptr, file_info.uncompressed_size, &file_info.uncompressed_size,
                   nullptr) == 0) {
        CloseHandle(h_f);
        free(ptr);
        sprintf(sz_name, res_err, filename);
        post_error(sz_name);
        return nullptr;
      }

      CloseHandle(h_f);
      if (size) {
        *size = file_info.uncompressed_size;
      }
      return ptr;
    }


    void HGE_CALL HGE_Impl::Resource_Free(void *res) {
      if (res) {
        free(res);
      }
    }


// NOLINTNEXTLINE
    const char *HGE_CALL HGE_Impl::Resource_MakePath(const char *filename) {
      static char tmp_filename_[_MAX_PATH];

      if (!filename) {
        strcpy(tmp_filename_, app_path_.c_str());
      } else if (filename[0] == '\\' || filename[0] == '/' || filename[1] == ':') {
        strcpy(tmp_filename_, filename);
      } else {
        strcpy(tmp_filename_, app_path_.c_str());
        strcat(tmp_filename_, filename);
      }

      for (auto i = 0; tmp_filename_[i]; i++) {
        if (tmp_filename_[i] == '/') {
          tmp_filename_[i] = '\\';
        }
      }
      return tmp_filename_;
    }

// NOLINTNEXTLINE
    const char *HGE_CALL HGE_Impl::Resource_EnumFiles(const char *wildcard) {
      if (wildcard) {
        if (h_search_) {
          FindClose(h_search_);
          h_search_ = nullptr;
        }
        h_search_ = FindFirstFile(Resource_MakePath(wildcard), &search_data_);
        if (h_search_ == INVALID_HANDLE_VALUE) {
          h_search_ = nullptr;
          return nullptr;
        }

        if (!(search_data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
          return search_data_.cFileName;
        }
        return Resource_EnumFiles();
      }
      if (!h_search_) {
        return nullptr;
      }
      for (;;) {
        if (!FindNextFile(h_search_, &search_data_)) {
          FindClose(h_search_);
          h_search_ = nullptr;
          return nullptr;
        }
        if (!(search_data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
          return search_data_.cFileName;
        }
      }
    }

// NOLINTNEXTLINE
    const char *HGE_CALL HGE_Impl::Resource_EnumFolders(const char *wildcard) {
      if (wildcard) {
        if (h_search_) {
          FindClose(h_search_);
          h_search_ = nullptr;
        }
        h_search_ = FindFirstFile(Resource_MakePath(wildcard), &search_data_);
        if (h_search_ == INVALID_HANDLE_VALUE) {
          h_search_ = nullptr;
          return nullptr;
        }

        if ((search_data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            && strcmp(search_data_.cFileName, ".") != 0
            && strcmp(search_data_.cFileName, "..") != 0) {
          return search_data_.cFileName;
        }
        return Resource_EnumFolders();
      }
      if (!h_search_) {
        return nullptr;
      }
      for (;;) {
        if (!FindNextFile(h_search_, &search_data_)) {
          FindClose(h_search_);
          h_search_ = nullptr;
          return nullptr;
        }
        if ((search_data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            && strcmp(search_data_.cFileName, ".") != 0
            && strcmp(search_data_.cFileName, "..") != 0) {
          return search_data_.cFileName;
        }
      }
    }

} // namespace
