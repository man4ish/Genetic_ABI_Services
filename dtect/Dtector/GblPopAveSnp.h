/* 
 * File:   GblPop_AveSnp.h
 * Author: jameswong
 *
 * Created on June 22, 2012, 7:18 PM
 */

#ifndef GBLPOP_AVESNP_H
#define	GBLPOP_AVESNP_H

#include <string>
#include <map>
#include "GblPopVfCall.h"

using namespace std;

class GblPopAveSnp
{
private:
    map<string, GblPopVfCall> assay_vf_map;
    map<string, GblPopVfCall>::iterator assay_vf_it;
    
public:
    GblPopAveSnp();
    ~GblPopAveSnp();
    
    void insert(
        const string& _assayid,
        const string& _vf_call,
        size_t _count);
    
    size_t size() const;
    
    void rewind();
    
    bool get_assayid(string &_assayid);
    
    const char *get_random_vf_call(const string &_assayid) const;
    
    void print(FILE *_file) const;
};

#endif	/* GBLPOP_AVESNP_H */

