/* 
 * File:   GblPopVfCall.h
 * Author: jameswong
 *
 * Created on June 22, 2012, 7:25 PM
 */

#ifndef GBLPOPVFCALL_H
#define	GBLPOPVFCALL_H

#include <string>
#include <map>

using namespace std;

class GblPopVfCall
{
private:
    size_t total_call;
    map<string, size_t> vf_call_map;
    map<string, size_t>::iterator vf_call_it;
    
public:
    GblPopVfCall();
    ~GblPopVfCall();
    
    void insert(
        const string& _vf_call,
        size_t _count);
    
    const char *get_random_vf_call() const;
    
    void print(FILE *_file) const;
};

#endif	/* GBLPOPVFCALL_H */

