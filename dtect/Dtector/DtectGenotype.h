/*
 * File:   DtectGenotype.h
 * Author: jameswong
 *
 * Created on February 8, 2013, 1:01 PM
 */

#ifndef DTECTGENOTYPE_H
#define	DTECTGENOTYPE_H

#include <set>

using namespace std;

class DtectGenotype
{
private:
    unsigned rsid;
    char allele_one;
    char allele_two;
    char risk_allele;
    double relative_risk;
    set<size_t> pmid_set;
    set<size_t>::iterator pmid_it;

public:
    DtectGenotype(
            unsigned _rs_id,
            char _allele_one,
            char _allele_two,
            char _risk_allele,
            double _relative_risk);

    DtectGenotype(
            const DtectGenotype *_obj);

    ~DtectGenotype();

    unsigned get_rsid() const;
    char get_allele_one() const;
    char get_allele_two() const;
    char get_risk_allele() const;
    double get_relative_risk() const;

    void insert_pmid(size_t _pmid);
    void rewind_pmid();

    bool get_pmid(set<size_t>& _pmid_set) const;
};

struct DtectGenotype_ltr
{
    inline bool operator()(const DtectGenotype *_lhs, const DtectGenotype *_rhs) const
    {
        if (_lhs->get_rsid() != _rhs->get_rsid())
            return (_lhs->get_rsid() < _rhs->get_rsid());
        if (_lhs->get_allele_one() != _rhs->get_allele_one())
            return (_lhs->get_allele_one() < _rhs->get_allele_one());
        return (_lhs->get_allele_two() < _rhs->get_allele_two());
    }
};

typedef set<DtectGenotype*, DtectGenotype_ltr> DTECT_GENOTYPE_SET;

#endif	/* DTECTGENOTYPE_H */

