/* 
 * File:   DtectGenotype.cpp
 * Author: Manish
 * 
 * Created on February 8, 2013, 1:01 PM
 */

#include "DtectGenotype.h"

DtectGenotype::DtectGenotype(
        unsigned _rs_id,
        char _allele_one,
        char _allele_two,
        char _risk_allele,
        double _relative_risk)
{
    this->rsid = _rs_id;
    this->allele_one = _allele_one;
    this->allele_two = _allele_two;
    this->risk_allele = _risk_allele;
    this->relative_risk = _relative_risk;
    this->pmid_set.clear();
}

DtectGenotype::DtectGenotype(
        const DtectGenotype* _obj)
{
    this->rsid = _obj->rsid;
    this->allele_one = _obj->allele_one;
    this->allele_two = _obj->allele_two;
    this->risk_allele = _obj->risk_allele;
    this->relative_risk = _obj->relative_risk;
    this->pmid_set = _obj->pmid_set;
}

DtectGenotype::~DtectGenotype()
{
    if (this->pmid_set.empty() == false)
        this->pmid_set.clear();
}

unsigned DtectGenotype::get_rsid() const
{
    return this->rsid;
}

char DtectGenotype::get_allele_one() const
{
    return this->allele_one;    
}

char DtectGenotype::get_allele_two() const
{
    return this->allele_two;
}

char DtectGenotype::get_risk_allele() const
{
    return this->risk_allele;
}

double DtectGenotype::get_relative_risk() const
{
    return this->relative_risk;
}

void DtectGenotype::insert_pmid(size_t _pmid)
{
    this->pmid_set.insert(_pmid);
}

void DtectGenotype::rewind_pmid()
{
    this->pmid_it = this->pmid_set.begin();
}

bool DtectGenotype::get_pmid(set<size_t>& _pmid_set) const
{
    for (set<size_t>::const_iterator cit = this->pmid_set.begin(); cit != this->pmid_set.end(); ++cit)
        _pmid_set.insert(*cit);
    return true;
}
