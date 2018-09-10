/*
 * File:   DtectReport.cpp
 * Author: jameswong
 *
 * Created on February 8, 2013, 11:59 AM
 */

#include <assert.h>
#include <stdio.h>
#include "DtectReport.h"

DtectReport::DtectReport()
{
    this->reset();
}

DtectReport::~DtectReport()
{
    this->cleanup();
}

void DtectReport::cleanup()
{
    for (DTECT_DIS_DRUG_SET::iterator it = this->dis_drug_set.begin(); it != this->dis_drug_set.end(); ++it)
        delete (*it);
    this->dis_drug_set.clear();
}

void DtectReport::reset()
{
    time_t cur_time;
    struct tm *t_info;
    time(&cur_time);
    t_info = localtime(&cur_time);
    this->date_year = t_info->tm_year + 1900;
    this->date_month = t_info->tm_mon + 1;
    this->date_day = t_info->tm_mday;
    this->date_hour = t_info->tm_hour;
    this->date_min = t_info->tm_min;
    this->date_sec = t_info->tm_sec;

    this->dis_drug_it = this->dis_drug_set.begin();
    
    this->cleanup();
}

void DtectReport::insert_result(
        const char* _type,
        const char* _dis_drug_id,
        char _sex,
        const char* _effect_id,
        int _genotype_count,
        double _risk,
        unsigned _rsid,
        char _allele_one,
        char _allele_two,
        char _risk_allele,
        double _relative_risk,
        double _Incidence, //syhwah 2013-04-02
        char _trait,       //syhwah 2013-04-03
        size_t _pmid)
{
    assert((_sex == 'B') || (_sex == 'M') || (_sex == 'F'));
    assert((_trait == 'Y') || (_trait == 'N'));

    DtectDisDrug obj(_type, _dis_drug_id, _sex, _Incidence, _trait); //syhwah 2013-04-02 : added _Incidence
    DTECT_DIS_DRUG_SET::iterator it = this->dis_drug_set.find(&obj);
    if (it == this->dis_drug_set.end())
        it = this->dis_drug_set.insert(new DtectDisDrug(&obj)).first;
    assert(it != this->dis_drug_set.end());

    (*it)->insert_effect(_effect_id, _genotype_count, _risk, _rsid, _allele_one, _allele_two, _risk_allele, _relative_risk,_pmid);
}

void DtectReport::filter_effect()
{
    for (DTECT_DIS_DRUG_SET::iterator it = this->dis_drug_set.begin(); it != this->dis_drug_set.end(); ++it)
        (*it)->filter_effect();
}

void DtectReport::print(
        FILE* _file) const
{
    if (_file == NULL) return;
    for (DTECT_DIS_DRUG_SET::iterator it = this->dis_drug_set.begin(); it != this->dis_drug_set.end(); ++it)
    {
        fprintf(_file, "\n\n");
        fprintf(_file, "%s ", (*it)->get_type().c_str());
        fprintf(_file, "%s ", (*it)->get_dis_drug_id().c_str());
        fprintf(_file, "%c ", (*it)->get_sex());
        fprintf(_file, "\n");
        fprintf(_file, "     Relative: %.4f\n", (*it)->get_relative_risk());
        fprintf(_file, "     YourRisk: %.4f\n", (*it)->get_total_risk());
        fprintf(_file, "     Negative: %d\n", (*it)->get_negative_markers());
        fprintf(_file, "     Neutral: %d\n", (*it)->get_neutral_markers());
        fprintf(_file, "     Positive: %d\n", (*it)->get_positive_markers());

        (*it)->print(_file);
    }
}

void DtectReport::rewind()
{
    for (this->dis_drug_it = this->dis_drug_set.begin(); this->dis_drug_it != this->dis_drug_set.end(); ++this->dis_drug_it)
        (*this->dis_drug_it)->rewind();
    this->dis_drug_it = this->dis_drug_set.begin();
}

void DtectReport::rewind(
        const string& _type,
        const string& _dis_drug_id)
{
    DtectDisDrug obj(_type, _dis_drug_id);
    this->dis_drug_it = this->dis_drug_set.find(&obj);
    assert(this->dis_drug_it != this->dis_drug_set.end());
    (*this->dis_drug_it)->rewind();
}

bool DtectReport::get_result(
        string& _type,
        string& _dis_drug_id,
        double& _total_risk)
{
    if (this->dis_drug_it == this->dis_drug_set.end())
        return false;

    _type = (*this->dis_drug_it)->get_type();
    _dis_drug_id = (*this->dis_drug_it)->get_dis_drug_id();
    _total_risk = (*this->dis_drug_it)->get_total_risk();

    ++this->dis_drug_it;
    return true;
}

bool DtectReport::find_result(
        const string& _type,
        const string& _dis_drug_id,
        char& _sex,
        double& _Incidence, //syhwah 2013-04-02
        char& _trait,
        double& _total_risk,
        double& _relative_risk,
        int& _positive_markers,
        int& _neutral_markers,
        int& _negative_markers) const
{
    DtectDisDrug obj(_type, _dis_drug_id);
    DTECT_DIS_DRUG_SET::iterator cit = this->dis_drug_set.find(&obj);
    assert(cit != this->dis_drug_set.end());

    _sex = (*cit)->get_sex();   
    _Incidence = (*cit)->get_Incidence(); //syhwah 2013-04-02
    _trait = (*cit)->get_trait(); //syhwah 2013-04-03
    _total_risk = (*cit)->get_total_risk();
    _relative_risk = (*cit)->get_relative_risk();   
    _positive_markers = (*cit)->get_positive_markers();
    _neutral_markers = (*cit)->get_neutral_markers();
    _negative_markers = (*cit)->get_negative_markers();

    return true;
}

int DtectReport::get_date_year() const
{
    return this->date_year;
}
int DtectReport::get_date_month() const
{
    return this->date_month;
}

int DtectReport::get_date_day() const
{
    return this->date_day;
}
    
int DtectReport::get_date_hour() const
{
    return this->date_hour;
}
    
int DtectReport::get_date_min() const
{
    return this->date_min;
}
    
int DtectReport::get_date_sec() const
{
    return this->date_sec;
}

void DtectReport::get_date(string& _date) const
{
    char year[5] = {0};
    sprintf(year, "%04d", this->date_year);
    char month[3] = {0};
    sprintf(month, "%02d", this->date_month);
    char day[3] = {0};
    sprintf(day, "%02d", this->date_day);
    _date.clear();
    _date.append(year);
    _date.append("-");
    _date.append(month);
    _date.append("-");
    _date.append(day);
}

bool DtectReport::get_effect_genotype_citations(
        string& _effect_id,
        double& _risk)
{
    if (this->dis_drug_it == this->dis_drug_set.end())
        return false;
    return (*this->dis_drug_it)->get_effect_genotype_citation(_effect_id, _risk);
}

bool DtectReport::get_genotype_by_effect(
        const string& _effect_id,
        unsigned& _rsid,
        string& _alleles,
        string& _risk_allele,
        double& _relative_risk)
{
    if (this->dis_drug_it == this->dis_drug_set.end())
        return false;
    return (*this->dis_drug_it)->get_genotypes(_effect_id, _rsid, _alleles, _risk_allele, _relative_risk);
}

bool DtectReport::get_pmid_by_effect(
        const string& _effect_id,
        set<size_t>& _pmid_set) const
{
    if (this->dis_drug_it == this->dis_drug_set.end())
        return false;
    return (*this->dis_drug_it)->get_pmid_by_effect(_effect_id, _pmid_set);
}
