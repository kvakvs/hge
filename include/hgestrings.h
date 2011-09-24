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
	hgeStringTable(const hgeString filename);
	~hgeStringTable();

	hgeString GetString(const hgeString name);

private:
	hgeStringTable(const hgeStringTable &);
	hgeStringTable & operator= (const hgeStringTable &);

	NamedString		* strings;

	static HGE		* hge;
};


#endif
