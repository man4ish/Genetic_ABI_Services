/*
 * File:   DtectDisDrug.h
 * Author: jameswong
 *
 * Created on February 8, 2013, 12:01 PM
 */

#ifndef DTECTDISDRUG_H
#define	DTECTDISDRUG_H

#include <string>
#include <set>
#include "DtectEffect.h"

using namespace std;

class DtectDisDrug
{
private:
    string type;
    string dis_drug_id;
    char sex;
    double Incidence;  //syhwah 2013-04-02
    char trait;        //syhwah 2013-04-03 
    int negative_markers;
    int neutral_markers;
    int positive_markers;
    double total_risk;
    double relative_risk;

    
    DTECT_EFFECT_SET effect_set;
    DTECT_EFFECT_SET::iterator effect_it;

public:
    DtectDisDrug(
            const string& _type,
            const string& _dis_drug_id);

    DtectDisDrug(
            const string& _type,
            const string& _dis_drug_id,
            char _sex,
            double _Incidence, //syhwah 2013-04-02: added _Incidence
            char _trait);      //syhwah 2013-04-03   

    DtectDisDrug(
            const DtectDisDrug *_obj);

    ~DtectDisDrug();

    const string& get_type() const;
    const string& get_dis_drug_id() const;
    char get_sex() const;
    double get_Incidence() const; //syhwah 2013-04-02
    char get_trait() const;       //syhwah 2013-04-03

    int get_negative_markers() const;
    int get_neutral_markers() const;
    int get_positive_markers() const;
    double get_total_risk() const;
    double get_relative_risk() const;

    void insert_effect(
        const char *_effect_id,
        int _genotype_count,
        double _risk,
        unsigned _rsid,
        char _allele_one,
        char _allele_two,
        char _risk_allele,
        double _relative_risk,
        size_t _pmid);

    void filter_effect();

    void rewind();

    bool get_effect_genotype_citation(
        string& _effect_id,
        double& _risk);

    bool get_genotypes(
        const string& _effect_id,
        unsigned& _rsid,
        string& _alleles,
        string& _risk_allele,
        double& _relative_risk);

    bool get_pmid_by_effect(
        const string& _effect_id,
        set<size_t>& _pmid_set) const;

    void print(FILE *_file) const;
};

struct DtectDisDrug_ltr
{
    inline bool operator()(const DtectDisDrug *_lhs, const DtectDisDrug *_rhs) const
    {
        if (_lhs->get_type() != _rhs->get_type())
            return (_lhs->get_type() < _rhs->get_type());
        return (_lhs->get_dis_drug_id() < _rhs->get_dis_drug_id());
    }
};

typedef set<DtectDisDrug*, DtectDisDrug_ltr> DTECT_DIS_DRUG_SET;

#endif	/* DTECTDISDRUG_H */

