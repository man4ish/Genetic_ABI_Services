/* 
 * File:   VerSvn.cpp
 * Author: wuntyng
 * 
 * Created on August 1, 2012, 11:27 AM
 */

#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <iostream>
#include "VerSvn.h"

VerSvn::VerSvn()
{
    this->svn_revision = 0;
    this->major_number = 0;
    this->minor_number = 0;
    this->patch_number = 0;
    this->is_trunk = false;
    const char *ptr = NULL;
    
    //get revision
    if (strcmp(REV, "$Revision$") != 0)
    {
        ptr = strstr(REV, ":");
        assert(ptr != NULL);
        this->svn_revision = (int)atoi(ptr + 1);
    }
    
    //get version number
    if (strcmp(URL, "$HeadURL$") != 0)
    {
        if((ptr = strstr(URL, "/tags/")) != NULL)
        {
            vector<char *> number;
            char *url = new char[strlen(ptr) + 1];
            strcpy(url, ptr + 6);
            size_t fCnt = this->rowParser(url, "./", number);
            assert(fCnt >= 3);
            this->major_number = atoi(number[0]);
            this->minor_number = atoi(number[1]);
            this->patch_number = atoi(number[2]);

            delete [] url;
        }
        else if(strstr(URL, "/trunk/") != NULL)
            this->is_trunk = true;
        else
        {
            //future branch
        }
    }
}

inline size_t VerSvn::rowParser(char *_source, const char *_delimiter, vector<char*> &_fields_vec) const
{
    _fields_vec.clear();
    char *pch = strtok(_source, _delimiter);
    while (pch != NULL)
    {
        _fields_vec.push_back(pch);
        pch = strtok(NULL, _delimiter);
    }
    return _fields_vec.size();
}

int VerSvn::get_revision() const
{
    return this->svn_revision;
}

int VerSvn::get_major_number() const
{
    return this->major_number;
}

int VerSvn::get_minor_number() const
{
    return this->minor_number;
}

int VerSvn::get_patch_number() const
{
    return this->patch_number;
}

bool VerSvn::get_is_trunk() const
{
    return this->is_trunk;
}
