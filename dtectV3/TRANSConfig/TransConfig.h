/* 
 * File:   TransConfig.h
 * Author: Manish
 *
 * Created on January 9, 2013, 12:01 PM
 */

#ifndef TRANSCONFIG_H
#define	TRANSCONFIG_H

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <string>
#include <map>

#include "XmlParse.h"

using namespace std;

struct DocInfo
{
    map<string, string> doc_id_map;
    int doc_format;
    int version;
    string latex;

    inline DocInfo()
    {
        this->doc_id_map.clear();
        this->doc_format = 1;
        this->version = 1;
    }
    
    void insert_doc_id(const string& _id_type, const string& _id)
    {
        this->doc_id_map[_id_type] = _id;
    }
    
    const char* get_doc_id(const string& _type) const
    {
        map<string, string>::const_iterator cit;
        cit = this->doc_id_map.find(_type);
        assert(cit != this->doc_id_map.end());
        return cit->second.c_str();
    }
};

typedef map<string, DocInfo*> STR_DOCINFO_MAP;
typedef pair<string, DocInfo*> STR_DOCINFO_PAIR;

class TransConfig : public XmlParse
{
private:
    char* env_var;
    string latex; //syhwah 2013-03-20
    STR_DOCINFO_MAP docinfo_map;
    STR_DOCINFO_MAP::iterator docinfo_it;
    
    bool parse(
        const xercesc::DOMElement* _pa_elem,
        const xercesc::DOMElement* _ch_elem);
    
    bool init();
    void cleanup();
    bool insert_doc(const char* _key);
    
public:
    TransConfig();
    ~TransConfig();
    
    const char* get_doc_id(
        const string& _type) const;
    
    const char* get_doc_id(
        const string& _testcode,
        const string& _type) const;
    
    int get_version(
        const string& _testcode) const;

    const char* get_latex() const;
};

#endif	/* TRANSCONFIG_H */

