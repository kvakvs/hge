/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeStringTable helper class header
*/
#pragma once
#ifndef HGESTRINGS_H
#define HGESTRINGS_H


#include <hge.h>


#define HGE_MAX_STRINGNAME_LENGTH 64


// TODO: wrap this in std::map instead of handcrafted list, for faster search
struct NamedString
{
	hgeChar		name[HGE_MAX_STRINGNAME_LENGTH];
	hgeString	string;
	NamedString	* next;
};

/*
** HGE String table class
*/
class hgeStringTable
{
public:
	hgeStringTable(hgeConstString filename);
	~hgeStringTable();

	hgeString GetString(hgeConstString name);

private:
	hgeStringTable(const hgeStringTable &);
	hgeStringTable & operator= (const hgeStringTable &);

	NamedString		* strings;

	static HGE		* hge;
};


#endif
