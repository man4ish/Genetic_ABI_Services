/* 
 * File:   GblPopSimPatient.cpp
 * Author: jameswong
 * 
 * Created on June 22, 2012, 10:33 PM
 */

#include "GblPopSimPatient.h"
#include <assert.h>

GblPopSimPatient::GblPopSimPatient()
{
    this->assay_vf_map.clear();
}

GblPopSimPatient::~GblPopSimPatient()
{
}

void GblPopSimPatient::insert(
        string _assayid,
        string _vf_call)
{
    assert(_assayid.empty() == false);
    assert(_vf_call.empty() == false);
    
    bool ret = this->assay_vf_map.insert(pair<string, string>(_assayid, _vf_call)).second;
    assert(ret == true);
}

void GblPopSimPatient::rewind()
{
    this->assay_vf_it = this->assay_vf_map.begin();
}

bool GblPopSimPatient::get_record(
        string& _assayid,
        string& _vf_call)
{
    if (this->assay_vf_it == this->assay_vf_map.end())
        return false;
    
    _assayid = this->assay_vf_it->first;
    _vf_call = this->assay_vf_it->second;
    
    ++this->assay_vf_it;
    return true;
}