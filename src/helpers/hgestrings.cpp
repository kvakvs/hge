/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeStringTable helper class implementation
*/


#include "..\..\include\hgestrings.h"
#include <ctype.h>

const char STRHEADERTAG[]="[HGESTRINGTABLE]";
const char STRFORMATERROR[]="String table %s has incorrect format.";


HGE *hgeStringTable::hge_=nullptr;


hgeStringTable::hgeStringTable(const char *filename)
{
    uint32_t size;
    char str_name[MAXSTRNAMELENGTH];

    hge_=hgeCreate(HGE_VERSION);
    strings_=nullptr;

    // load string table file
    void *data = hge_->Resource_Load(filename, &size);
    if(!data) {
        return;
    }

    char *desc = new char[size+1];
    memcpy(desc,data,size);
    desc[size]=0;
    hge_->Resource_Free(data);

    // check header
    if(memcmp(desc, STRHEADERTAG, sizeof(STRHEADERTAG)-1)) {
        hge_->System_Log(STRFORMATERROR, filename);
        delete[] desc;
        return;
    }

    char *pdesc = desc+sizeof(STRHEADERTAG);
    char *str_value = new char[8192];

    for(;;) {
        // skip whitespaces
        while(isspace(*pdesc)) {
            pdesc++;
        }
        if(!*pdesc) {
            break;
        }

        // skip comments
        if(*pdesc==';') {
            while(*pdesc && *pdesc != '\n') {
                pdesc++;
            }
            pdesc++;
            continue;
        }

        // get string name -> str_name
        int i = 0;
        while(pdesc[i] && pdesc[i]!='=' && !isspace(pdesc[i]) && i<MAXSTRNAMELENGTH) {
            str_name[i]=pdesc[i];
            i++;
        }
        str_name[i]=0;
        pdesc+=i;

        // skip string name overflow characters
        while(*pdesc && *pdesc!='=' && !isspace(*pdesc)) {
            pdesc++;
        }
        if(!*pdesc) {
            break;
        }

        // skip whitespaces to '='
        while(isspace(*pdesc)) {
            pdesc++;
        }
        if(*pdesc!='=')	{
            hge_->System_Log(STRFORMATERROR, filename);
            break;
        }
        pdesc++;

        // skip whitespaces to '"'
        while(isspace(*pdesc)) {
            pdesc++;
        }
        if(*pdesc!='"')	{
            hge_->System_Log(STRFORMATERROR, filename);
            break;
        }
        pdesc++;

        // parse string value till the closing '"' -> str_value
        // consider: \", \n, \\, LF, CR, whitespaces at line begin/end
        char *pvalue = str_value;

        while(*pdesc && *pdesc!='"') {
            if(*pdesc=='\n' || *pdesc=='\r') {
                while(isspace(*pdesc)) {
                    pdesc++;
                }

                pvalue--;
                while(pvalue>=str_value && isspace(*pvalue)) {
                    pvalue--;
                }
                pvalue++;
                *pvalue=' ';
                pvalue++;

                continue;
            }

            if(*pdesc=='\\') {
                pdesc++;
                if(!*pdesc) {
                    continue;
                }
                if(*pdesc=='n') {
                    *pvalue='\n';
                } else {
                    *pvalue=*pdesc;
                }
                pvalue++;
                pdesc++;
                continue;
            }

            *pvalue=*pdesc;
            pvalue++;
            pdesc++;
        }

        *pvalue=0;

        // add the parsed string to the list
        NamedString *str = new NamedString;
        strcpy(str->name, str_name);
        str->string=new char[strlen(str_value)+1];
        strcpy(str->string, str_value);
        str->next=strings_;
        strings_=str;

        if(!*pdesc) {
            break;
        }
        pdesc++;
    }

    delete[] str_value;
    delete[] desc;
}

hgeStringTable::~hgeStringTable()
{

    NamedString *str = strings_;
    while(str) {
        NamedString *strnext = str->next;
        delete[] str->string;
        delete str;
        str=strnext;
    }

    hge_->Release();
}

char *hgeStringTable::GetString(const char *name)
{
    NamedString *str=strings_;

    while(str) {
        if(!strcmp(name, str->name)) {
            return str->string;
        }
        str=str->next;
    }

    return nullptr;
}
