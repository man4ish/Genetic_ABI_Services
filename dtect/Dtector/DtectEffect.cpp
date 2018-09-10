/*
 * File:   DtectEffect.cpp
 * Author: jameswong
 *
 * Created on February 8, 2013, 12:40 PM
 */

#include <stdio.h>
#include <assert.h>
#include "DtectEffect.h"

DtectEffect::DtectEffect(
        const string& _effect_id)
{
    this->effect_id = _effect_id;
    this->genotype_count = 0;
    this->risk = 0.0;
    this->genotype_set.clear();
}

DtectEffect::DtectEffect(
        const string& _effect_id,
        int _genotype_count,
        double _risk)
{
    this->effect_id.assign(_effect_id.c_str());
    this->genotype_count = _genotype_count;
    this->risk = _risk;
    this->genotype_set.clear();
}

DtectEffect::DtectEffect(
        const DtectEffect* _obj)
{
    this->effect_id = _obj->effect_id;
    this->genotype_count = _obj->genotype_count;
    this->risk = _obj->risk;
    this->genotype_set = _obj->genotype_set;
}

DtectEffect::~DtectEffect()
{
    for (DTECT_GENOTYPE_SET::iterator it = this->genotype_set.begin(); it != this->genotype_set.end(); ++it)
        delete (*it);
    this->genotype_set.clear();
}

const string& DtectEffect::get_effect_id() const
{
    return this->effect_id;
}

int DtectEffect::get_genotype_count() const
{
    return this->genotype_count;
}

double DtectEffect::get_risk() const
{
    return this->risk;
}

void DtectEffect::insert_genotype(
        unsigned _rsid,
        char _allele_one,
        char _allele_two,
        char _risk_allele,
        double _relative_risk,
        size_t _pmid)
{
    DtectGenotype obj(_rsid, _allele_one, _allele_two, _risk_allele, _relative_risk);
    DTECT_GENOTYPE_SET::iterator it = this->genotype_set.find(&obj);
    if (it == this->genotype_set.end())
        it = this->genotype_set.insert(new DtectGenotype(&obj)).first;
    assert(it != this->genotype_set.end());

    (*it)->insert_pmid(_pmid);
}

size_t DtectEffect::get_genotype_size() const
{
    return size_t(this->genotype_set.size());
}

double DtectEffect::get_relative_risk() const
{
    double relative_risk = 1.0;
    for (DTECT_GENOTYPE_SET::const_iterator it = this->genotype_set.begin(); it != this->genotype_set.end(); ++it)
        relative_risk *= (*it)->get_relative_risk();
    return relative_risk;
}

void DtectEffect::rewind()
{
    for (this->genotype_it = this->genotype_set.begin(); this->genotype_it != this->genotype_set.end(); ++this->genotype_it)
        (*this->genotype_it)->rewind_pmid();
    this->genotype_it = this->genotype_set.begin();
}

bool DtectEffect::get_genotypes(
        unsigned& _rsid,
        string& _alleles,
        string& _risk_allele,
        double& _relative_risk)
{
    // CAUTION: Behaviour of this function is dependent on the state of member variable 'genotype_it'
    if (this->genotype_it == this->genotype_set.end())
        return false;

    _rsid = (*this->genotype_it)->get_rsid();
    _alleles += (*this->genotype_it)->get_allele_one();
    _alleles += (*this->genotype_it)->get_allele_two();
    _risk_allele = (*this->genotype_it)->get_risk_allele();
    assert(_risk_allele.length() == 1);
    if (_risk_allele[0] == 'N')
        _risk_allele[0] = 'n';
    _relative_risk = (*this->genotype_it)->get_relative_risk();

    ++this->genotype_it;
    return true;
}

bool DtectEffect::get_pmid(
        set<size_t>& _pmid_set) const
{
    for (DTECT_GENOTYPE_SET::const_iterator cit = this->genotype_set.begin(); cit != this->genotype_set.end(); ++cit)
        (*cit)->get_pmid(_pmid_set);
    return true;
}

void DtectEffect::print(FILE* _file) const
{
    if (_file == NULL) return;
    for (DTECT_GENOTYPE_SET::const_iterator it = this->genotype_set.begin(); it != this->genotype_set.end(); ++it)
    {
        fprintf(_file, "\n");
        fprintf(_file, "      rs%d (%c%c)", (*it)->get_rsid(), (*it)->get_allele_one(), (*it)->get_allele_two());
    }
}