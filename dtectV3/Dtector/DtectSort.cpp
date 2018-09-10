/* 
 * File:   DtectSort.cpp
 * Author: Manish
 * 
 * Created on February 8, 2013, 5:59 PM
 */

#include "DtectSort.h"

DtectSort::DtectSort()
{
    this->name_map.clear();
    this->name_it = this->name_map.begin();
}

DtectSort::~DtectSort()
{
    if (this->name_map.empty() == false)
    {
        for (this->name_it = this->name_map.begin(); this->name_it != this->name_map.end(); ++this->name_it)
            delete this->name_it->first;
        this->name_map.clear();
    }
}

void DtectSort::insert_name(
        const string& _type,
        const string& _name,
        const string& _id)
{
    Name obj(_type, _name);
    if (this->name_map.find(&obj) == this->name_map.end())
        this->name_map.insert(pair<Name*, string>(new Name(&obj), _id));
    this->name_it = this->name_map.begin();
}

bool DtectSort::get_name(
        string& _type,
        string& _name,
        string& _id)
{
    if (this->name_it == this->name_map.end())
        return false;
    _type = this->name_it->first->type;
    _name = this->name_it->first->name;
    _id = this->name_it->second;
    ++this->name_it;
    return true;
}