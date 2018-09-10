/*
 * File:   QSGenPop.cpp
 * Author: Manish
 *
 * Created on June 14, 2012, 12:39 PM
 */

#include <map>
#include <stdio.h>
#include "QSGenPop.h"

using namespace std;

QSGenPop::QSGenPop()
{
    this->pop_map.clear();
}

QSGenPop::~QSGenPop()
{
    this->clear();
}

void QSGenPop::clear()
{
    if (this->pop_map.empty() == false)
    {
        this->pop_it = this->pop_map.begin();
        while (this->pop_it != this->pop_map.end())
        {
            delete this->pop_it->first;
            ++this->pop_it;
        }
        this->pop_map.clear();
    }
}

void QSGenPop::insert(
        const string& _type,
        const string& _dis_drug_id,
        double _risk)
{
    DisDrugRisk obj(_type, _dis_drug_id, _risk);

    this->pop_it = this->pop_map.find(&obj);
    if (this->pop_it == this->pop_map.end())
        this->pop_it = this->pop_map.insert(pair<DisDrugRisk*, size_t>(new DisDrugRisk(&obj), 0)).first;
    assert(this->pop_it != this->pop_map.end());
    this->pop_it->second++;
}

double QSGenPop::average(
        const string& _dis_drug_id) const
{
    assert(_dis_drug_id.empty() == false);
    map<DisDrugRisk*, size_t, DisDrugRisk_ltr>::const_iterator const_pop_it;

    double sum_fx = 0.0;
    double sum_f = 0.0;

    for (const_pop_it = this->pop_map.begin(); const_pop_it != this->pop_map.end(); ++const_pop_it)
    {
        if (const_pop_it->first->dis_drug_id == _dis_drug_id)
        {
            sum_fx += double(const_pop_it->first->risk) * double(const_pop_it->second);
            sum_f += double(const_pop_it->second);
        }
    }
    return sum_fx/sum_f;
}

void QSGenPop::compute_n_bin(
        size_t _max_bar,
        double _min_pct,
        bool _cluster_target,
        string _type,
        string _dis_drug_id,
        double _risk,
        PopDistribution &_pop_dis) const
{
    map<DisDrugRisk*, size_t, DisDrugRisk_ltr>::const_iterator const_cur_pop_it;
    
    Binner binner(_max_bar, _min_pct, _cluster_target);
    bool risk_bin_match = false;
    
    assert(this->pop_map.empty() == false);
    
    for (const_cur_pop_it = this->pop_map.begin(); const_cur_pop_it != this->pop_map.end(); ++const_cur_pop_it)
    {
        if ((const_cur_pop_it->first->type == _type) && (const_cur_pop_it->first->dis_drug_id == _dis_drug_id))
        {
            if (const_cur_pop_it->first->risk == _risk)
                risk_bin_match = true;
            binner.increment_total(const_cur_pop_it->second);
            binner.increment_count();
            binner.insert_risk_count(const_cur_pop_it->first->risk, const_cur_pop_it->second);
        }
    }
    
    double min_risk = 0;
    double max_risk = 0;
    double percent;
    
    while (binner.get(_risk, min_risk, max_risk, percent, true) == true)
        _pop_dis.insert(_type, _dis_drug_id, min_risk, max_risk, percent, _risk);
}
