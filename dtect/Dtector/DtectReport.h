/* 
 * File:   DtectReport.h
 * Author: jameswong
 *
 * Created on February 8, 2013, 11:59 AM
 */

#ifndef DTECTREPORT_H
#define	DTECTREPORT_H

#include "DtectDisDrug.h"

class DtectReport
{
private:
    int date_year, date_month, date_day, date_hour, date_min, date_sec;
    DTECT_DIS_DRUG_SET dis_drug_set;
    DTECT_DIS_DRUG_SET::iterator dis_drug_it;
    
    void cleanup();
    
public:
    DtectReport();
    ~DtectReport();
    
    void reset();
    
    void insert_result(
        const char *_type,
        const char *_dis_drug_id,
        char _sex,
        const char *_effect_id,
        int _genotype_count,
        double _risk,
        unsigned _rsid,
        char _allele_one,
        char _allele_two,
        char _risk_allele,
        double _relative_risk,
        double _Incidence, //syhwah 2013-04-02 
        char _trait,       //syhwah 2013-04-03 
        size_t _pmid);
    
    void filter_effect();
    
    void print(FILE *_file) const;
    
    void rewind();
    
    void rewind(
        const string& _type,
        const string& _dis_drug_id);
    
    bool get_result(
        string& _type,
        string& _dis_drug_id,
        double& _total_risk);
    
    bool find_result(
        const string& _type,
        const string& _dis_drug_id,
        char& _sex,              //syhwah 2013-04-02
        double& _Incidence,
        char& _trait,            //syhwah 2013-04-03
        double& _total_risk,
        double& _relative_risk,
        int& _positive_markers,
        int& _neutral_markers,
        int& _negative_markers) const;
    
    int get_date_year() const;
    int get_date_month() const;
    int get_date_day() const;
    int get_date_hour() const;
    int get_date_min() const;
    int get_date_sec() const;
    
    void get_date(string& _date) const;
    
    bool get_effect_genotype_citations(
        string& _effect_id,
        double& _risk);
    
    bool get_genotype_by_effect(
        const string& _effect_id,
        unsigned& _rsid,
        string& _alleles,
        string& _risk_allele,
        double& _relative_risk);
    
    bool get_pmid_by_effect(
        const string& _effect_id,
        set<size_t>& _pmid_set) const;
};

#endif	/* DTECTREPORT_H */

