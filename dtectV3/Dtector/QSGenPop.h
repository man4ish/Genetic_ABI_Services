/* 
 * File:   QSGenPop.h
 * Author: jameswong
 *
 * Created on June 14, 2012, 12:39 PM
 */

#ifndef QSGENPOP_H
#define	QSGENPOP_H

#include <string>
#include <map>
#include <stdio.h>
#include "QSPop.h"

using namespace std;

struct DisDrugPercent
{
    string type;
    double your_risk;
    
    map<pair<unsigned, unsigned>, double> range_map;
    map<pair<unsigned, unsigned>, double>::iterator range_it;
    
    bool get_segment(unsigned &_min, unsigned &_max, double &_percent)
    {
        if (this->range_it == this->range_map.end())
            return false;
        _min = this->range_it->first.first;
        _max = this->range_it->first.second;
        _percent = this->range_it->second;
        ++this->range_it;
        return true;
    }
};

struct PopDistribution
{
    map<string, DisDrugPercent>distribution_map;
    map<string, DisDrugPercent>::iterator distribution_it;
    
    void insert(
        string _type,
        string _dis_drug_id,
        unsigned _min_risk,
        unsigned _max_risk,
        double _percent,
        double _your_risk)
    {
        this->distribution_it = this->distribution_map.find(_dis_drug_id);
        if (this->distribution_it == this->distribution_map.end())
            this->distribution_it = this->distribution_map.insert(pair<string, DisDrugPercent>(_dis_drug_id, DisDrugPercent())).first;
        assert(this->distribution_it != this->distribution_map.end());
        
        this->distribution_it->second.type = _type;
        this->distribution_it->second.your_risk = _your_risk;
        
        this->distribution_it->second.range_map.insert(pair<pair<unsigned,unsigned>,double>(pair<unsigned,unsigned>(_min_risk, _max_risk), _percent));
        this->distribution_it->second.range_it = this->distribution_it->second.range_map.begin();
    }
    
    void print(FILE *_file) const
    {
        if (_file == NULL) return;
        map<string, DisDrugPercent>::const_iterator it;
        for (it = this->distribution_map.begin(); it != this->distribution_map.end(); ++it)
        {
            fprintf(_file, "%s\n", it->first.c_str());
            fprintf(_file, "  %s\n", it->second.type.c_str());
            fprintf(_file, "  %.4f\n", it->second.your_risk);
            
            map<pair<unsigned, unsigned>, double>::const_iterator c_it;
            for (c_it = it->second.range_map.begin(); c_it != it->second.range_map.end(); ++c_it)
                fprintf(_file, "  %u-%u  %.4f\n", c_it->first.first, c_it->first.second, c_it->second);
        }
    }
    
    double get_risk(const string &_dis_drug_id) const
    {
        assert(_dis_drug_id.empty() == false);
        assert(this->distribution_map.find(_dis_drug_id) != this->distribution_map.end());
        return this->distribution_map.find(_dis_drug_id)->second.your_risk;
    }
    
    bool get_segment(const string &_dis_drug_id, unsigned &_min, unsigned &_max, double &_percent)
    {
        return this->distribution_map.find(_dis_drug_id)->second.get_segment(_min, _max, _percent);
    }
};

class QSGenPop
{
private:
    map<DisDrugRisk*, size_t, DisDrugRisk_ltr> pop_map;
    map<DisDrugRisk*, size_t, DisDrugRisk_ltr>::iterator pop_it;
    
public:
    QSGenPop();
    ~QSGenPop();
    
    void clear();
    
    void insert(
        const string& _type,
        const string& _dis_drug_id,
        double _risk);
    
    double average(
        const string& _dis_drug_id) const;
    
    void compute_n_bin(
        size_t _max_bar,
        double _min_pct,
        bool _cluster_target,
        string _type,
        string _dis_drug_id,
        double _risk,
        PopDistribution &_pop_dis) const;
};

#endif	/* QSGENPOP_H */

