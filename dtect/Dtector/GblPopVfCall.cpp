/* 
 * File:   GblPopVfCall.cpp
 * Author: jameswong
 * 
 * Created on June 22, 2012, 7:25 PM
 */

#include "GblPopVfCall.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

GblPopVfCall::GblPopVfCall()
{
    this->vf_call_map.clear();
    this->total_call = 0;
}

GblPopVfCall::~GblPopVfCall()
{
}

void GblPopVfCall::insert(
        const string& _vf_call,
        size_t _count)
{
    this->vf_call_it = this->vf_call_map.find(_vf_call);
    if (this->vf_call_it == this->vf_call_map.end())
        this->vf_call_it = this->vf_call_map.insert(pair<string, size_t>(_vf_call, 0)).first;
    assert(this->vf_call_it != this->vf_call_map.end());
    this->vf_call_it->second += _count;
    this->total_call += _count;
}

const char *GblPopVfCall::get_random_vf_call() const
{
    map<string, size_t>::const_iterator const_call_it;
    long random_idx = (rand() % long(this->total_call)) + 1;
    
    size_t loc_total_call = 0;
    const char* ret = NULL;
    assert(this->vf_call_map.size() <= 3);
    
    for (const_call_it = this->vf_call_map.begin(); const_call_it != this->vf_call_map.end(); ++const_call_it)
    {
        loc_total_call += const_call_it->second;
        random_idx -= long(const_call_it->second);
        if ((random_idx <= 0) && (ret == NULL))
            ret = const_call_it->first.c_str();
    }
    assert(loc_total_call == this->total_call);
    assert(ret != NULL);
    return ret;
}

void GblPopVfCall::print(FILE* _file) const
{
    if (_file == NULL) return;
    for (map<string, size_t>::const_iterator cit = this->vf_call_map.begin(); cit != this->vf_call_map.end(); ++cit)
        fprintf(_file, "   %s %lu\n", cit->first.c_str(), cit->second);
    fflush(_file);
}