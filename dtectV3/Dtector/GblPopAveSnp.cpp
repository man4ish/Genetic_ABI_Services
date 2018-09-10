/* 
 * File:   GblPop_AveSnp.cpp
 * Author: Manish
 * 
 * Created on June 22, 2012, 7:18 PM
 */

#include "GblPopAveSnp.h"
#include <assert.h>
#include <stdio.h>

GblPopAveSnp::GblPopAveSnp()
{
    this->assay_vf_map.clear();
}

GblPopAveSnp::~GblPopAveSnp()
{
}

void GblPopAveSnp::insert(
        const string& _assayid,
        const string& _vf_call,
        size_t _count)
{
    assert(_assayid.empty() == false);
    assert(_vf_call.empty() == false);
    
    this->assay_vf_it = this->assay_vf_map.find(_assayid);
    if (this->assay_vf_it == this->assay_vf_map.end())
        this->assay_vf_it = this->assay_vf_map.insert(pair<string, GblPopVfCall>(_assayid, GblPopVfCall())).first;
    assert(this->assay_vf_it != this->assay_vf_map.end());
    
    this->assay_vf_it->second.insert(_vf_call, _count);
}

size_t GblPopAveSnp::size() const
{
    return this->assay_vf_map.size();
}

void GblPopAveSnp::rewind()
{
    this->assay_vf_it = this->assay_vf_map.begin();
}

bool GblPopAveSnp::get_assayid(string& _assayid)
{
    if (this->assay_vf_it == this->assay_vf_map.end())
        return false;
    _assayid = this->assay_vf_it->first;
    ++this->assay_vf_it;
    return true;
}

const char *GblPopAveSnp::get_random_vf_call(const string &_assayid) const
{
    map<string, GblPopVfCall>::const_iterator const_assay_vf_it;
    const_assay_vf_it = this->assay_vf_map.find(_assayid);
    assert(const_assay_vf_it != this->assay_vf_map.end());
    return const_assay_vf_it->second.get_random_vf_call();

}

void GblPopAveSnp::print(FILE* _file) const
{
    if (_file == NULL) return;
    for (map<string, GblPopVfCall>::const_iterator cit = this->assay_vf_map.begin(); cit != this->assay_vf_map.end(); ++cit)
    {
        fprintf(_file, " %-10s\n", cit->first.c_str());
        cit->second.print(_file);
        fprintf(_file, "\n");
        fflush(_file);
    }
}