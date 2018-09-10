/* 
 * File:   GSSDb_Search.cpp
 * Author: jameswong
 * 
 * Created on May 25, 2012, 10:43 PM
 */

#include "GSSDb.h"
#include <iostream>
#include <stdio.h>

using namespace std;

bool GSSDb::search_dis_drug(
        set<string>& _dis_drug_set,
        const string& _chip_type,
        const string& _chip_ver,
        const set<string>* _assay_set)
{
    assert(_chip_type.length() == 3);
    assert(_chip_ver.length() == 2);
    assert(_assay_set->empty() == false);
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT DISTINCT DisDrug.DisDrugID FROM DisDrug "
            "JOIN Effects "
            "ON DisDrug.DisDrugID=Effects.DisDrugID "
            "JOIN GenoEffect "
            "ON Effects.EffectID=GenoEffect.EffectID "
            "JOIN GenoLink "
            "ON GenoEffect.GenoLinkFK=GenoLink.GenoLinkSK "
            "JOIN Genotype "
            "ON (GenoLink.GenoCurrFK=Genotype.GenotypeSK OR GenoLink.GenoNextFK=Genotype.GenotypeSK) "
            "JOIN Assay "
            "ON Genotype.AssayFK=Assay.AssaySK "
            "JOIN Chip "
            "ON Assay.ChipFK=Chip.ChipSK "
            "WHERE "
            "Chip.ChipType=" << mysqlpp::quote << _chip_type << " AND "
            "Chip.ChipVer=" << mysqlpp::quote << _chip_ver << " AND "
            "Assay.AssayIdNK IN (";
    for (set<string>::const_iterator cit = _assay_set->begin();;)
    {
        qry << mysqlpp::quote << *cit;
        ++cit;
        if (cit == _assay_set->end())
            break;
        qry << ", ";
    }
    qry << ")";
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    for (size_t i = 0; i < str_res.size(); ++i)
        _dis_drug_set.insert(str_res[i][0].c_str());
    return ret;
}

bool GSSDb::get_dis_drug_cnt(
        size_t& _count,
        const string &_chip_type,
        const string &_chip_ver)
{
    assert(_chip_type.length() == 3);
    assert(_chip_ver.length() == 2);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT COUNT(DISTINCT(DisDrug.DisDrugID)) FROM DisDrug "
            "JOIN Effects "
            "ON DisDrug.DisDrugID=Effects.DisDrugID "
            "JOIN GenoEffect "
            "ON Effects.EffectID=GenoEffect.EffectID "
            "JOIN GenoLink "
            "ON GenoEffect.GenoLinkFK=GenoLink.GenoLinkSK "
            "JOIN Genotype "
            "ON (GenoLink.GenoCurrFK=Genotype.GenotypeSK OR GenoLink.GenoNextFK=Genotype.GenotypeSK) "
            "JOIN Assay "
            "ON Genotype.AssayFK=Assay.AssaySK "
            "JOIN Chip "
            "ON Assay.ChipFK=Chip.ChipSK "
            "WHERE "
            "Chip.ChipType=" << mysqlpp::quote << _chip_type << " AND "
            "Chip.ChipVer=" << mysqlpp::quote << _chip_ver;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    if (str_res.size() != 1) return false;
    _count = size_t(atol(str_res[0][0].c_str()));
    return ret;
}

bool GSSDb::get_assay_cnt(
        int& _count,
        const string& _chip_type,
        const string& _chip_ver)
{
    assert(_chip_type.length() == 3);
    assert(_chip_ver.length() == 2);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT COUNT(DISTINCT(Assay.AssayIdNK)) "
            "FROM Assay "
            "JOIN Chip "
            "ON Assay.ChipFK=Chip.ChipSK "
            "WHERE "
            "Chip.ChipType=" << mysqlpp::quote << _chip_type << " AND "
            "Chip.ChipVer=" << mysqlpp::quote << _chip_ver;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    if (str_res.size() != 1) return false;
    _count = atoi(str_res[0][0].c_str());
    return ret;
}
