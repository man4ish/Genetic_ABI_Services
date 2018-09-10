/* 
 * File:   DtectSort.h
 * Author: jameswong
 *
 * Created on February 8, 2013, 5:59 PM
 */

#ifndef DTECTSORT_H
#define	DTECTSORT_H

#include <map>
#include <string>

using namespace std;

struct Name
{
    string type;
    string name;
    
    Name(const string& _type, const string& _name)
    {
        this->type = _type;
        this->name = _name;
    }
    
    Name(const Name* _obj)
    {
        this->type = _obj->type;
        this->name = _obj->name;
    }
};

struct Name_ltr
{
    inline bool operator()(const Name* _lhs, const Name* _rhs) const
    {
        if (_lhs->type != _rhs->type)
            return (_lhs->type < _rhs->type);
        return (_lhs->name < _rhs->name);
    }
};

class DtectSort
{
private:
    map<Name*, string, Name_ltr> name_map;
    map<Name*, string, Name_ltr>::iterator name_it;
    
public:
    DtectSort();
    ~DtectSort();
    
    void insert_name(
        const string& _type,
        const string& _name,
        const string& _id);
    
    bool get_name(
        string& _type,
        string& _name,
        string& _id);
};

#endif	/* DTECTSORT_H */

