/*
 * File:   DtectDisDrug.cpp
 * Author: jameswong
 *
 * Created on February 8, 2013, 12:01 PM
 */

#include <stdio.h>
#include <assert.h>
#include "DtectDisDrug.h"

DtectDisDrug::DtectDisDrug(
        const string& _type,
        const string& _dis_drug_id)
{
    this->type = _type;
    this->dis_drug_id = _dis_drug_id;
    this->sex = 'B';
    this->trait = 'N';
    this->effect_set.clear();
    this->negative_markers = 0;
    this->neutral_markers = 0;
    this->positive_markers = 0;
    this->total_risk = 0.0;
    this->relative_risk = 1.0;
}

DtectDisDrug::DtectDisDrug(
        const string& _type,
        const string& _dis_drug_id,
        char _sex,
        double _Incidence, //syhwah 2013-04-02: added _Incidence
        char _trait)
{
    this->type = _type.c_str();
    this->dis_drug_id = _dis_drug_id.c_str();
    this->sex = _sex;
    this->Incidence = _Incidence; //syhwah 2013-04-02   
    this->trait = _trait;         //syhwah 2013-04-03
    this->effect_set.clear();
    this->negative_markers = 0;
    this->neutral_markers = 0;
    this->positive_markers = 0;
    this->total_risk = 0.0;
    this->relative_risk = 1.0;
}

DtectDisDrug::DtectDisDrug(
        const DtectDisDrug* _obj)
{
    this->type = _obj->type;
    this->dis_drug_id = _obj->dis_drug_id;
    this->sex = _obj->sex;
    this->Incidence = _obj->Incidence; //syhwah 2013-04-02 
    this->trait = _obj->trait;         //syhwah 2013-04-03
    this->effect_set = _obj->effect_set;
    this->negative_markers = _obj->negative_markers;
    this->neutral_markers = _obj->neutral_markers;
    this->positive_markers = _obj->positive_markers;
    this->total_risk = _obj->total_risk;
    this->relative_risk = _obj->relative_risk;
}

DtectDisDrug::~DtectDisDrug()
{
    for (DTECT_EFFECT_SET::iterator it = this->effect_set.begin(); it != this->effect_set.end(); ++it)
        delete (*it);
    this->effect_set.clear();
    
    this->dis_drug_id.clear();
    this->type.clear();
}

const string& DtectDisDrug::get_type() const
{
    return this->type;
}

const string& DtectDisDrug::get_dis_drug_id() const
{
    return this->dis_drug_id;
}

char DtectDisDrug::get_sex() const
{
    return this->sex;
}

double DtectDisDrug::get_Incidence() const
{
    return this->Incidence;
}

char DtectDisDrug::get_trait() const
{
    return this->trait;
}

int DtectDisDrug::get_negative_markers() const
{
    return this->negative_markers;
}

int DtectDisDrug::get_neutral_markers() const
{
    return this->neutral_markers;
}

int DtectDisDrug::get_positive_markers() const
{
    return this->positive_markers;
}

double DtectDisDrug::get_total_risk() const
{
    return this->total_risk;
}

double DtectDisDrug::get_relative_risk() const
{
    return this->relative_risk;
}

void DtectDisDrug::insert_effect(
        const char* _effect_id,
        int _genotype_count,
        double _risk,
        unsigned _rsid,
        char _allele_one,
        char _allele_two,
        char _risk_allele,
        double _relative_risk,
        size_t _pmid)
{
    DtectEffect obj(_effect_id, _genotype_count, _risk);
    DTECT_EFFECT_SET::iterator it = this->effect_set.find(&obj);
    if (it == this->effect_set.end())
        it = this->effect_set.insert(new DtectEffect(&obj)).first;
    assert(it != this->effect_set.end());

    (*it)->insert_genotype(_rsid, _allele_one, _allele_two, _risk_allele, _relative_risk, _pmid);
}

void DtectDisDrug::filter_effect()
{
    // erase the effect if the genotype count mismatch
    for (DTECT_EFFECT_SET::iterator it = this->effect_set.begin(); it != this->effect_set.end();)
    {
        DTECT_EFFECT_SET::iterator cit = it++;
        if ((*cit)->get_genotype_size() != size_t((*cit)->get_genotype_count()))
        {
            delete *cit;
            this->effect_set.erase(cit);
        }
    }

    // count risk and markers
    for (DTECT_EFFECT_SET::iterator it = this->effect_set.begin(); it != this->effect_set.end(); ++it)
    {
        if ((*it)->get_risk() < 0)
        {
            this->negative_markers++;
            this->total_risk += -(*it)->get_risk();
        }
        else if ((*it)->get_risk() == 0)
            this->neutral_markers++;
        else
            this->positive_markers++;

        this->relative_risk *= (*it)->get_relative_risk();
    }
}

void DtectDisDrug::rewind()
{
    for (this->effect_it = this->effect_set.begin(); this->effect_it != this->effect_set.end(); ++this->effect_it)
        (*this->effect_it)->rewind();
    this->effect_it = this->effect_set.begin();
}

bool DtectDisDrug::get_effect_genotype_citation(
        string& _effect_id,
        double& _risk)
{
    // CAUTION: Behaviour of this function is dependent on the state of member variable 'effect_it'
    if (this->effect_it == this->effect_set.end())
        return false;
    _effect_id = (*this->effect_it)->get_effect_id();
    _risk = (*this->effect_it)->get_risk();
    (*this->effect_it)->rewind();
    ++this->effect_it;
    return true;
}

bool DtectDisDrug::get_genotypes(
        const string& _effect_id,
        unsigned& _rsid,
        string& _alleles,
        string& _risk_allele,
        double& _relative_risk)
{
    DtectEffect obj(_effect_id);
    DTECT_EFFECT_SET::const_iterator cit = this->effect_set.find(&obj);
    assert(cit != this->effect_set.end());

    return (*cit)->get_genotypes(_rsid, _alleles, _risk_allele, _relative_risk);
}

bool DtectDisDrug::get_pmid_by_effect(
        const string& _effect_id,
        set<size_t>& _pmid_set) const
{
    DtectEffect obj(_effect_id);
    DTECT_EFFECT_SET::const_iterator cit = this->effect_set.find(&obj);
    assert(cit != this->effect_set.end());

    return (*cit)->get_pmid(_pmid_set);
}

void DtectDisDrug::print(FILE* _file) const
{
    if (_file == NULL) return;
    for (DTECT_EFFECT_SET::iterator it = this->effect_set.begin(); it != this->effect_set.end(); ++it)
    {
        fprintf(_file, "\n");
        fprintf(_file, "  %s", (*it)->get_effect_id().c_str());
        fprintf(_file, " (%02.0f)", (*it)->get_risk());
        (*it)->print(_file);
    }
}
