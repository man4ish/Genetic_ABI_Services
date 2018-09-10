/* 
 * File:   GBLPOP.h
 * Author: wuntyng
 *
 * Created on June 18, 2012, 2:37 PM
 */

#ifndef GBLPOP_H
#define	GBLPOP_H

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <map>
#include "CkSpider.h"
#include "XmlParse.h"

using namespace std;

enum LEVEL{FIRST = 0, CHILD, SIBLING};

struct PopGeno
{
    string population;
    string genotype;
    
    inline PopGeno()
    {
        
    }
    
    inline PopGeno(const PopGeno *_obj)
    {
        this->genotype = _obj->genotype;
        this->population = _obj->population;
    }
};

struct PopGeno_ltr
{
    inline bool operator()(const PopGeno *_lhs, const PopGeno *_rhs)
    {
        if (_lhs->population != _rhs->population)
            return (_lhs->population < _rhs->population);
        return (_lhs->genotype < _rhs->genotype);
    }
};

class GblPopDownloader : public XmlParse
{
private:
    int pop_id;
    int sample_size;
    string pop_name;
    
    map<PopGeno*, unsigned, PopGeno_ltr> popgeno_map;
    map<PopGeno*, unsigned, PopGeno_ltr>::iterator popgeno_it;
    
    map<int, string> pop_map;
    map<int, string>::iterator pop_it;
    
    bool parse(
        const xercesc::DOMElement* _pa_elem,
        const xercesc::DOMElement* _ch_elem);

public:
    GblPopDownloader();
    ~GblPopDownloader();
    
    void download_xml(unsigned _rsid);
    
    bool get_record(string &_population,
        string &_genotype,
        unsigned &_sample_count);
    
    size_t size() const;
};

#endif	/* GBLPOP_H */

