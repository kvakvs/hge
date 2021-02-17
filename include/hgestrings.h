/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once

#include <hge.h>
#include <string>
#include <map>

#define MAXSTRNAMELENGTH 64


/*
** HGE String table class
*/
class hgeStringTable {
public:
    hgeStringTable(const char *filename);

    ~hgeStringTable();

    const char *GetString(const char *name);

private:
    hgeStringTable(const hgeStringTable &);

    hgeStringTable &operator=(const hgeStringTable &);

    std::map<std::string, std::string> strings_;

    static HGE *hge_;
};
