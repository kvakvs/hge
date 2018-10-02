/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeStringTable helper class header
*/


#pragma once


#include "hge.h"
#include <string>
#include <map>


#define MAXSTRNAMELENGTH 64


/*
** HGE String table class
*/
class hgeStringTable {
public:
    hgeStringTable(const char* filename);
    ~hgeStringTable();

    const char* GetString(const char* name);

private:
    hgeStringTable(const hgeStringTable&);
    hgeStringTable& operator=(const hgeStringTable&);

    std::map<std::string, std::string> strings_;

    static HGE* hge_;
};
