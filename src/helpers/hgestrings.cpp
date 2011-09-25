/*
 ** Haaf's Game Engine 1.7
 ** Copyright (C) 2003-2007, Relish Games
 ** hge.relishgames.com
 **
 ** hgeStringTable helper class implementation
 */
#include <hgestrings.h>
#include <ctype.h>

hgeConstString STRHEADERTAG = TXT("[HGESTRINGTABLE]");
hgeConstString STRFORMATERROR = TXT("String table %s has incorrect format.");

HGE *hgeStringTable::hge = 0;



hgeStringTable::hgeStringTable(hgeConstString filename)
{
	int i;
	void *data;
	uint32_t size;
	hgeChar *desc, *pdesc;
	NamedString *str;
	hgeChar str_name[HGE_MAX_STRINGNAME_LENGTH];
	hgeChar *str_value, *pvalue;

	hge = hgeCreate(HGE_VERSION);
	strings = 0;

	// load string table file
	data = hge->Resource_Load(filename, &size);
	if (!data)
		return;

	desc = new hgeChar[size + 1];
	memcpy(desc, data, size * sizeof(hgeChar));
	desc[size] = 0;
	hge->Resource_Free(data);

	// check header
	if (memcmp(desc, STRHEADERTAG, sizeof(STRHEADERTAG) - 1))
	{
		hge->System_Log(STRFORMATERROR, filename);
		delete[] desc;
		return;
	}

	pdesc = desc + sizeof(STRHEADERTAG);
	str_value = new hgeChar[8192];

	for (;;)
	{
		// skip whitespaces
		while (isspace(*pdesc))
			pdesc++;
		if (!*pdesc)
			break;

		// skip comments
		if (*pdesc == ';')
		{
			while (*pdesc && *pdesc != '\n')
				pdesc++;
			pdesc++;
			continue;
		}

		// get string name -> str_name
		i = 0;
		while (pdesc[i] && pdesc[i] != '=' && !isspace(pdesc[i]) && i
				< HGE_MAX_STRINGNAME_LENGTH)
		{
			str_name[i] = pdesc[i];
			i++;
		}
		str_name[i] = 0;
		pdesc += i;

		// skip string name overflow characters
		while (*pdesc && *pdesc != '=' && !isspace(*pdesc))
			pdesc++;
		if (!*pdesc)
			break;

		// skip whitespaces to '='
		while (isspace(*pdesc))
			pdesc++;
		if (*pdesc != '=')
		{
			hge->System_Log(STRFORMATERROR, filename);
			break;
		}
		pdesc++;

		// skip whitespaces to '"'
		while (isspace(*pdesc))
			pdesc++;
		if (*pdesc != '"')
		{
			hge->System_Log(STRFORMATERROR, filename);
			break;
		}
		pdesc++;

		// parse string value till the closing '"' -> str_value
		// consider: \", \n, \\, LF, CR, whitespaces at line begin/end
		pvalue = str_value;

		while (*pdesc && *pdesc != '"')
		{
			if (*pdesc == '\n' || *pdesc == '\r')
			{
				while (isspace(*pdesc))
					pdesc++;

				pvalue--;
				while (pvalue >= str_value && isspace(*pvalue))
					pvalue--;
				pvalue++;
				*pvalue = ' ';
				pvalue++;

				continue;
			}

			if (*pdesc == '\\')
			{
				pdesc++;
				if (!*pdesc)
					continue;
				if (*pdesc == 'n')
					*pvalue = '\n';
				else
					*pvalue = *pdesc;
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
		str = new NamedString;
		hge_strcpy(str->name, str_name);
		str->string = new hgeChar[hge_strlen(str_value) + 1];
		hge_strcpy(str->string, str_value);
		str->next = strings;
		strings = str;

		if (!*pdesc)
			break;
		pdesc++;
	}

	delete[] str_value;
	delete[] desc;
}



hgeStringTable::~hgeStringTable()
{
	NamedString *str, *strnext;

	str = strings;
	while (str)
	{
		strnext = str->next;
		delete[] str->string;
		delete str;
		str = strnext;
	}

	hge->Release();
}



hgeString hgeStringTable::GetString(hgeConstString name)
{
	NamedString *str = strings;

	while (str)
	{
		if (!hge_strcmp(name, str->name))
			return str->string;
		str = str->next;
	}

	return 0;
}
