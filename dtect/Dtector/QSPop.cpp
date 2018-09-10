#include "QSPop.h"
#include <stdio.h>
#include <string>

struct PercentDelta
{
    double delta;
    double prcnt;
    
    inline PercentDelta(
        double _delta,
        double _prcnt)
    {
        this->delta = _delta;
        this->prcnt = _prcnt;
    }
};

struct PrcntDelta_ltr
{
    inline bool operator()(const PercentDelta *_lhs, const PercentDelta *_rhs)
    {
        assert(_lhs != NULL);
        assert(_rhs != NULL);
        if (_lhs->delta != _rhs->delta)
            return (_lhs->delta < _rhs->delta);
        return (_lhs->prcnt < _rhs->prcnt);
    }
};

Binner::Binner(
        size_t _max_bar,
        double _min_pct,
        bool _cluster_target)
{
    this->max_bar = _max_bar;
    if (this->max_bar < 3)
        this->max_bar = 3;
    this->total = 0;
    this->min_pct = _min_pct;
    this->first = true;
    this->count = 0;
    this->total_pct = 0.0;
    this->cluster_target = _cluster_target;
}

Binner::~Binner()
{
    for (this->clus_curr_it = this->clus_map.begin(); this->clus_curr_it != this->clus_map.end(); ++this->clus_curr_it)
        delete this->clus_curr_it->first;
    this->clus_map.clear();
}

void Binner::increment_total(size_t _total)
{
    this->total += _total;
}

void Binner::increment_count()
{
    this->count++;
}

void Binner::insert_risk_count(
        double _risk,
        size_t _count)
{
    this->first = true;
    this->bin_it = this->bin_map.find(_risk);
    if (this->bin_it == this->bin_map.end())
        this->bin_it = this->bin_map.insert(pair<double, size_t>(_risk, 0)).first;
    assert(this->bin_it != this->bin_map.end());
    this->bin_it->second += _count;
}

bool Binner::get(
        const double _risk,
        double& _min_risk,
        double& _max_risk,
        double& _percent,
        bool _print)
{
    if (this->first == true)
    {
        //
        // Clustering algorithm.
        // - Always cluster from the smallest segment and add it the adjacent smallest segment
        // - Keep clustering if number of segments is larger than max_bar (configurable limit)
        // - Keep clustering if minimum segment is lower than min_pct (configurable limit)
        // - Optionally prevent clustering of target (tgt) segment (configurable option)
        //
        // Example: max_bar = 5, min_pct = 10%, cluster_target = false
        //
        //        Segment: 7                    Segment: 6                    Segment: 5 (rule satisfied)  Segment: 4 (rule satisfied)
        //        Min: 1%                       Min: 4%                       Min: 8%                      Min: 13% (rule satisfied)
        //        +-------+                     +-------+                     +-------+                     +-------+
        //        |  34%  |                     |  34%  |                     |  34%  |                     |  34%  |
        //        |  20%  |                     |  20%  |                     |  20%  |                     |  20%  |
        //  tgt > | [13%] |                     | [13%] |                     | [13%] |                     | [13%] |
        //        |   3%  | < smallest adjacent |   4%  | < smallest          |  25%  | < smallest adjacent |  33%  |
        //        |   1%  | < smallest          |  21%  | < smallest adjacent |   8%  | < smallest          +-------+
        //        |  21%  |                     |   8%  |                     +-------+
        //        |   8%  |                     +-------+
        //        +-------+
        //
        
        this->total_pct = 0.0;
        this->first = false;
        assert(this->clus_map.empty() == true);
        bool has_sml_pct = true;
        double max_segment_pcnt = numeric_limits<double>::min();
        
        // Populate clus_map first
        for (this->bin_it = this->bin_map.begin(); this->bin_it != this->bin_map.end(); ++this->bin_it)
        {
            double percent = double(this->bin_it->second)/double(this->total) * 100.0;
            bool ret = this->clus_map.insert(pair<Bin*, double>(new Bin(this->bin_it->first, this->bin_it->first), percent)).second;
            assert(ret == true);
            if (percent > max_segment_pcnt)
                max_segment_pcnt = percent;
        }
        
        // Start clustering (binning) until we reach the desired segment limit
        while ((this->clus_map.size() > this->max_bar) || (has_sml_pct == true))
        {
            multimap<PercentDelta*, BinPair*, PrcntDelta_ltr> percent_bin_map;
            multimap<PercentDelta*, BinPair*, PrcntDelta_ltr>::iterator percent_bin_it;
            
            for (this->clus_curr_it = this->clus_map.begin(); this->clus_curr_it != this->clus_map.end(); ++this->clus_curr_it)
            {
                for (this->clus_next_it = this->clus_map.begin(); this->clus_next_it != this->clus_map.end(); ++this->clus_next_it)
                {
                    if (this->clus_next_it->first->head > this->clus_curr_it->first->tail)
                    {
                        double delta = this->clus_next_it->first->head - this->clus_curr_it->first->tail;
                        double prcnt = this->clus_next_it->second + this->clus_curr_it->second;
                        
                        if ((this->clus_next_it->first->head >= _risk) && (_risk >= this->clus_curr_it->first->tail))
                        {
                            if (this->cluster_target == true)
                            {
                                // if target risk is within cluster, double the percentage
                                // to decrease the chance of if being clustered
                                prcnt *= 2.0;
                            }
                            else
                            {
                                // set the percentage to the max percentage to prevent it from
                                // being clustered
                                prcnt = max_segment_pcnt;
                            }
                        }

                        PercentDelta *pct_del = new PercentDelta(delta, prcnt);
                        BinPair *bin_pair = new BinPair(this->clus_curr_it->first, this->clus_next_it->first);
                        percent_bin_map.insert(pair<PercentDelta*, BinPair*>(pct_del, bin_pair));
                    }
                }
            }
            for (percent_bin_it = percent_bin_map.begin(); percent_bin_it != percent_bin_map.end(); ++percent_bin_it)
            {
                this->clus_curr_it = this->clus_map.find(const_cast<Bin*>(percent_bin_it->second->curr_bin));
                this->clus_next_it = this->clus_map.find(const_cast<Bin*>(percent_bin_it->second->next_bin));

                if ((has_sml_pct == false) || (((this->clus_curr_it->second < this->min_pct) || (this->clus_next_it->second < this->min_pct))))
                {
                    Bin *t = new Bin(this->clus_curr_it->first->head, this->clus_next_it->first->tail);
                    double percent = this->clus_curr_it->second + this->clus_next_it->second;

                    delete this->clus_curr_it->first;
                    delete this->clus_next_it->first;
                    this->clus_map.erase(this->clus_curr_it);
                    this->clus_map.erase(this->clus_next_it);
                    bool ret = this->clus_map.insert(pair<Bin*, double>(t, percent)).second;
                    assert(ret == true);
                    break;
                }
            }
            
            has_sml_pct = false;
            this->clus_curr_it = this->clus_map.begin();
            while ((this->clus_curr_it != this->clus_map.end()) && (has_sml_pct == false))
            {
                if (this->clus_curr_it->second < this->min_pct)
                    has_sml_pct = true;
                ++this->clus_curr_it;
            }
            
            for (percent_bin_it = percent_bin_map.begin(); percent_bin_it != percent_bin_map.end(); ++percent_bin_it)
            {
                delete percent_bin_it->first;
                delete percent_bin_it->second;
            }
            percent_bin_map.clear();
        }
        this->clus_curr_it = this->clus_map.begin();
    }
    if (this->clus_curr_it == this->clus_map.end())
    {
        assert((this->total_pct > 99.95) && (this->total_pct < 100.05));
        return false;
    }
    
    this->total_pct += this->clus_curr_it->second;
    _min_risk = unsigned(this->clus_curr_it->first->head);
    _max_risk = unsigned(this->clus_curr_it->first->tail);
    _percent = this->clus_curr_it->second;
    ++this->clus_curr_it;
    return true;
}