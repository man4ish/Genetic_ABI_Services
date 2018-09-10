/* 
 * File:   QSPop.h
 * Author: jameswong
 *
 * Created on June 14, 2012, 12:00 PM
 */

#ifndef QSPOP_H
#define	QSPOP_H

#include <string>
#include <mysql++/mysql++.h>
#include <assert.h>

using namespace std;

struct DisDrugRisk
{
    string type;
    string dis_drug_id;
    double risk;
    
    inline DisDrugRisk(
        const string& _type,
        const string& _dis_drug_id,
        double _risk)
    {
        this->type = _type;
        this->dis_drug_id = _dis_drug_id;
        this->risk = _risk;
    }
    
    inline DisDrugRisk(const DisDrugRisk *_obj)
    {
        assert(_obj != NULL);
        this->type = _obj->type;
        this->dis_drug_id = _obj->dis_drug_id;
        this->risk = _obj->risk;
    }
};

struct DisDrugRisk_ltr
{
    inline bool operator()(const DisDrugRisk *_lhs, const DisDrugRisk *_rhs)
    {
        assert(_lhs != NULL);
        assert(_rhs != NULL);
        if (_lhs->type != _rhs->type)
            return (_lhs->type < _rhs->type);
        if (_lhs->dis_drug_id != _rhs->dis_drug_id)
            return (_lhs->dis_drug_id < _rhs->dis_drug_id);
        return (_lhs->risk < _rhs->risk);
    }
};

struct Bin
{
    double head;
    double tail;
    
    inline Bin(double _head, double _tail)
    {
        this->head = _head;
        this->tail = _tail;
    }
};

struct Bin_ltr
{
    inline bool operator()(const Bin *_lhs, const Bin *_rhs)
    {
        assert(_lhs != NULL);
        assert(_rhs != NULL);
        if (_lhs->head != _rhs->head)
            return (_lhs->head < _rhs->head);
        return (_lhs->tail < _rhs->tail);
    }
};

struct BinPair
{
    const Bin *curr_bin;
    const Bin *next_bin;
    
    inline BinPair(const Bin *_curr, const Bin *_next)
    {
        assert(_curr != NULL);
        assert(_next != NULL);
        this->curr_bin = _curr;
        this->next_bin = _next;
    }
};

class Binner
{
private:
    size_t total;
    size_t count;
    bool first;
    size_t max_bar;
    double min_pct;
    double total_pct;
    bool cluster_target;
    
    map<double, size_t> bin_map;
    map<double, size_t>::iterator bin_it;
    
    map<Bin*, double, Bin_ltr> clus_map;
    map<Bin*, double, Bin_ltr>::iterator clus_curr_it;
    map<Bin*, double, Bin_ltr>::iterator clus_next_it;
    
public:
    Binner(
            size_t _max_bar,
            double _min_pct,
            bool _cluster_target);
    
    ~Binner();
    
    void increment_total(size_t _total);
    
    void increment_count();
    
    void insert_risk_count(
        double _risk,
        size_t _count);
    
    bool get(
        const double _risk,
        double &_min_risk,
        double &_max_risk,
        double &_percent,
        bool _print = false);
};

#endif	/* QSPOP_H */
