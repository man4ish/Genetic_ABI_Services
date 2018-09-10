/*
 * File:   DtectEffect.h
 * Author: jameswong
 *
 * Created on February 8, 2013, 12:40 PM
 */

#ifndef DTECTEFFECT_H
#define	DTECTEFFECT_H

#include <set>
#include <string>
#include "DtectGenotype.h"

using namespace std;

class DtectEffect
{
private:
    string effect_id;
    int genotype_count;
    double risk;

    DTECT_GENOTYPE_SET genotype_set;
    DTECT_GENOTYPE_SET::iterator genotype_it;

public:
    DtectEffect(
            const string& _effect_id);

    DtectEffect(
            const string& _effect_id,
            int _genotype_count,
            double _risk);

    DtectEffect(
            const DtectEffect* _obj);

    ~DtectEffect();

    const string& get_effect_id() const;
    int get_genotype_count() const;
    double get_risk() const;

    void insert_genotype(
            unsigned _rsid,
            char _allele_one,
            char _allele_two,
            char _risk_allele,
            double _relative_risk,
            size_t _pmid);

    size_t get_genotype_size() const;

    double get_relative_risk() const;

    void rewind();

    bool get_genotypes(
            unsigned& _rsid,
            string& _alleles,
            string& _risk_allele,
            double& _relative_risk);

    bool get_pmid(
            set<size_t>& _pmid_set) const;

    void print(FILE *_file) const;
};

struct DtectEffect_ltr
{
    inline bool operator()(const DtectEffect *_lhs, const DtectEffect *_rhs) const
    {
        return (_lhs->get_effect_id() < _rhs->get_effect_id());
    }
};

typedef set<DtectEffect*, DtectEffect_ltr> DTECT_EFFECT_SET;

#endif	/* DTECTEFFECT_H */

