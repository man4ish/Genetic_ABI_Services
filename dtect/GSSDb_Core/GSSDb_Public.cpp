/* 
 * File:   GSSDb_Public.cpp
 * Author: jameswong
 * 
 * Created on May 21, 2012, 16:43 PM
 */

#include <mysql++/result.h>
#define EXPAND_MY_SSQLS_STATICS
#include <stdio.h>
#include <iostream>
#include <vector>
#include "GSSDb.h"
#include "GSSDb_Func.h"


using namespace std;

GSSDb::GSSDb(const char *_dbname) : DbBase(_dbname)
{
}

GSSDb::~GSSDb()
{
}

bool GSSDb::init(
        const char* _host,
        const char* _user,
        const char* _pswd)
{
    if (this->connect(_host, _user, _pswd, true) == false)
        return false;
    return (this->check_table());
}

bool GSSDb::insert_chip(
        mysqlpp::sql_int_unsigned& _chip_sk,
        mysqlpp::sql_char _chip_type,
        mysqlpp::sql_char _chip_ver)
{
    assert(_chip_type.length() == 3);
    assert(_chip_ver.length() == 2);
    _chip_sk = 0;
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO Chip "
            "SET "
            "Chip.ChipType=" << mysqlpp::quote << _chip_type << ", "
            "Chip.ChipVer=" << mysqlpp::quote << _chip_ver;
    bool ret = this->query_execute(qry, _chip_sk);
    if (ret == false) return false;
    if (_chip_sk != 0) return ret;
    
    qry.reset();
    qry << "SELECT ChipSK FROM Chip WHERE "
            "Chip.ChipType=" << mysqlpp::quote << _chip_type << " AND "
            "Chip.ChipVer=" << mysqlpp::quote << _chip_ver;
    mysqlpp::StoreQueryResult str_res;
    ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    assert(str_res.size() == 1);
    _chip_sk = mysqlpp::sql_int_unsigned(atol(str_res[0][0].c_str()));
    
    return ret;
}

bool GSSDb::get_chip_key(
        mysqlpp::sql_int_unsigned &_chip_key,
        mysqlpp::sql_char _chip_type,
        mysqlpp::sql_char _chip_ver)
{
    assert(_chip_type.length() == 3);
    assert(_chip_ver.length() == 2);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT ChipSK FROM Chip WHERE "
            "Chip.ChipType=" << mysqlpp::quote << _chip_type << " AND "
            "Chip.ChipVer=" << mysqlpp::quote << _chip_ver;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    assert(str_res.size() == 1);
    _chip_key = mysqlpp::sql_int_unsigned(atol(str_res[0][0].c_str()));
    return ret;
}

bool GSSDb::insert_assay(
        mysqlpp::sql_int_unsigned &_assay_sk,
        mysqlpp::sql_int_unsigned _chip_fk,
        mysqlpp::sql_int_unsigned _rs_id,
        mysqlpp::sql_varchar _assay_id,
        mysqlpp::sql_enum _vic_allele,
        mysqlpp::sql_enum _fam_allele)
{
    assert(_chip_fk != 0);
    assert(_rs_id != 0);
    assert(_assay_id.empty() == false);
    assert(_vic_allele.empty() == false);
    assert(_fam_allele.empty() == false);
    _assay_sk = 0;
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO Assay "
            "SET "
            "Assay.ChipFK=" << mysqlpp::quote << _chip_fk << ", "
            "Assay.RsId=" << mysqlpp::quote << _rs_id << ", "
            "Assay.AssayIdNK=" << mysqlpp::quote << _assay_id << ", "
            "Assay.VicAllele=" << mysqlpp::quote << _vic_allele << ", "
            "Assay.FamAllele=" << mysqlpp::quote << _fam_allele;
    bool ret = this->query_execute(qry, _assay_sk);
    if (ret == false) return false;
    if (_assay_sk != 0) return ret;
    qry.reset();
    qry << "SELECT Assay.AssaySK FROM Assay WHERE "
            "Assay.ChipFK=" << mysqlpp::quote << _chip_fk << " AND "
            "Assay.RsId=" << mysqlpp::quote << _rs_id << " AND "
            "Assay.AssayIdNK=" << mysqlpp::quote << _assay_id << " AND "
            "Assay.VicAllele=" << mysqlpp::quote << _vic_allele << " AND "
            "Assay.FamAllele=" << mysqlpp::quote << _fam_allele;
    mysqlpp::StoreQueryResult str_res;
    ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    assert(str_res.size() == 1);
    _assay_sk = mysqlpp::sql_int_unsigned(atol(str_res[0][0].c_str()));
    return ret;
}

bool GSSDb::insert_risk_factor(
        mysqlpp::sql_int_unsigned _assay_sk,
        mysqlpp::sql_varchar _dis_drug_id,
        mysqlpp::sql_char _risk_allele,
        double _rr_factor,
        double _rn_factor,
        double _nn_factor)
{
    if (_assay_sk == 0) return false;
    if (_risk_allele.empty()) return false;
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO RelativeRisk "
            "SET "
            "RelativeRisk.AssayFK=" << mysqlpp::quote << _assay_sk << ","
            "RelativeRisk.DisDrugID=" << mysqlpp::quote << _dis_drug_id << ","
            "RelativeRisk.RiskAllele=" << mysqlpp::quote << _risk_allele << ","
            "RelativeRisk.RR_Factor=" << mysqlpp::quote << _rr_factor << ","
            "RelativeRisk.RN_Factor=" << mysqlpp::quote << _rn_factor << ","
            "RelativeRisk.NN_Factor=" << mysqlpp::quote << _nn_factor;
    return this->query_execute(qry);
}

bool GSSDb::get_assay_key(
        mysqlpp::sql_int_unsigned& _assay_sk,
        mysqlpp::sql_int_unsigned _chip_fk,
        mysqlpp::sql_int_unsigned _rs_id)
{
    assert(_chip_fk != 0);
    assert(_rs_id != 0);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT Assay.AssaySK FROM Assay WHERE "
            "Assay.ChipFK=" << mysqlpp::quote << _chip_fk << " AND "
            "Assay.RsId=" << mysqlpp::quote << _rs_id;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    assert(str_res.size() == 1);
    _assay_sk = mysqlpp::sql_int_unsigned(atol(str_res[0][0].c_str()));
    return ret;
}

bool GSSDb::get_assay_key(
        mysqlpp::sql_int_unsigned& _assay_key,
        mysqlpp::sql_varchar _assay_id,
        mysqlpp::sql_char _chip_type,
        mysqlpp::sql_char _chip_ver)
{
    assert(_assay_id.empty() == false);
    assert(_chip_type.length() == 3);
    assert(_chip_ver.length() == 2);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT Assay.AssaySK FROM Assay "
            "JOIN Chip "
            "ON Chip.ChipSK=Assay.ChipFK "
            "WHERE "
            "Assay.AssayIdNK=" << mysqlpp::quote << _assay_id << " AND "
            "Chip.ChipType=" << mysqlpp::quote << _chip_type << " AND "
            "Chip.ChipVer=" << mysqlpp::quote << _chip_ver;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    if (str_res.empty()) return false;
    assert(str_res.size() == 1);
    _assay_key = mysqlpp::sql_int_unsigned(atol(str_res[0][0].c_str()));
    return ret;
}

bool GSSDb::insert_genotype(
        mysqlpp::sql_int_unsigned& _genotype_sk,
        mysqlpp::sql_int_unsigned _assay_fk,
        mysqlpp::sql_enum _allele_one,
        mysqlpp::sql_enum _allele_two)
{
    assert(_assay_fk != 0);
    assert(_allele_one.empty() == false);
    assert(_allele_two.empty() == false);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT Assay.VicAllele, Assay.FamAllele FROM Assay"
            " WHERE Assay.AssaySK=" << mysqlpp::quote << _assay_fk;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return false;
    assert(str_res.empty() == false);
    
    for (size_t i = 0; i < str_res.size(); i++)
    {
        char allele_one = _allele_one.c_str()[0];
        char allele_two = _allele_two.c_str()[0];
        char vic_allele = str_res[i]["VicAllele"].c_str()[0];
        char fam_allele = str_res[i]["FamAllele"].c_str()[0];
        assert(is_valid_nucleotide(allele_one));
        assert(is_valid_nucleotide(allele_two));
        assert(is_valid_nucleotide(vic_allele));
        assert(is_valid_nucleotide(fam_allele));
        
        // vic/vic
        if ((allele_one == vic_allele) && (allele_two == vic_allele))
        {
        }
        // vic/fam
        else if ((allele_one == vic_allele) && (allele_two == fam_allele))
        {
        }
        // fam/fam
        else if ((allele_one == fam_allele) && (allele_two == fam_allele))
        {
        }
        else
        {
            fprintf(stderr, "\nERROR: Invalid genotype alleles\n");
            return false;
        }
    }
    qry.reset();
    qry << "INSERT IGNORE "
            "INTO Genotype "
            "SET "
            "Genotype.AssayFK=" << mysqlpp::quote << _assay_fk << ", "
            "Genotype.AlleleOne=" << mysqlpp::quote << _allele_one << ", "
            "Genotype.AlleleTwo=" << mysqlpp::quote << _allele_two;
    ret = this->query_execute(qry, _genotype_sk);
    if (ret == false) return false;
    if (_genotype_sk != 0) return ret;
    
    qry.reset();
    qry << "SELECT Genotype.GenotypeSK FROM Genotype "
            "JOIN Assay "
            "ON Assay.AssaySK=Genotype.AssayFK "
            "WHERE "
            "Assay.AssaySK=" << mysqlpp::quote << _assay_fk << " AND "
            "Genotype.AlleleOne=" << mysqlpp::quote << _allele_one << " AND "
            "Genotype.AlleleTwo=" << mysqlpp::quote << _allele_two;
    ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    assert(str_res.size() == 1);
    _genotype_sk = mysqlpp::sql_int_unsigned(atol(str_res[0][0].c_str()));
    return ret;
}

bool GSSDb::insert_genolink(
        mysqlpp::sql_int_unsigned &_genolink_sk,
        mysqlpp::sql_int_unsigned _geno_curr_fk,
        mysqlpp::sql_int_unsigned _geno_next_fk)
{
    assert(_geno_curr_fk != 0);
    assert(_geno_next_fk != 0);
    _genolink_sk = 0;
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO GenoLink "
            "SET "
            "GenoLink.GenoCurrFK=" << mysqlpp::quote << _geno_curr_fk << ", "
            "GenoLink.GenoNextFK=" << mysqlpp::quote << _geno_next_fk;
    bool ret = this->query_execute(qry, _genolink_sk);
    if (ret == false) return ret;
    if (_genolink_sk != 0) return ret;
    qry.reset();
    qry << "SELECT GenoLink.GenoLinkSK FROM GenoLink WHERE "
            "GenoLink.GenoCurrFK=" << mysqlpp::quote << _geno_curr_fk << " AND "
            "GenoLink.GenoNextFK=" << mysqlpp::quote << _geno_next_fk;
    mysqlpp::StoreQueryResult str_res;
    ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    assert(str_res.size() == 1);
    _genolink_sk = mysqlpp::sql_int_unsigned(atol(str_res[0][0].c_str()));
    return ret;
}

bool GSSDb::insert_dis_drug(
        mysqlpp::sql_varchar _dis_drug_id,
        mysqlpp::sql_enum _type,
        mysqlpp::sql_enum _sex,
        double _Incidence, //syhwah 2013-04-02: added _Incidence
        mysqlpp::sql_enum _trait) //syhwah 2013-04-03: added _trait  
{       
    assert(_type.empty() == false);
    mysqlpp::Query qry = this->db_conn.query();
    
    qry << "INSERT IGNORE "
            "INTO DisDrug "
            "VALUES ("
            << mysqlpp::quote << _dis_drug_id << ","
            << mysqlpp::quote << _type << ","
            << mysqlpp::quote << _sex << ","
            << mysqlpp::quote << _Incidence << ","
            << mysqlpp::quote << _trait << ")";
    return this->query_execute(qry);
}

bool GSSDb::insert_effect(
        mysqlpp::sql_varchar _effect_id,
        mysqlpp::sql_varchar _dis_drug_id,
        mysqlpp::sql_tinyint_unsigned _genotype_cnt,
        mysqlpp::sql_double _risk)
{
    assert(_effect_id.empty() == false);
    assert((_genotype_cnt > mysqlpp::sql_tinyint_unsigned(0)) &&
            (_genotype_cnt <= mysqlpp::sql_tinyint_unsigned(2)));
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO Effects "
            "SET "
            "Effects.EffectID=" << mysqlpp::quote << _effect_id << ", "
            "Effects.DisDrugID=" << mysqlpp::quote << _dis_drug_id << ", "
            "Effects.GenotypeCount=" << mysqlpp::quote << _genotype_cnt << ", "
            "Effects.Risk=" << mysqlpp::quote << _risk;
    return this->query_execute(qry);
}

bool GSSDb::insert_geno_effect(
        mysqlpp::sql_int_unsigned &_geno_effect_sk,
        mysqlpp::sql_int_unsigned _genolink_fk,
        mysqlpp::sql_varchar _effect_id)
{
    assert(_effect_id.empty() == false);
    assert(_genolink_fk != 0);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO GenoEffect "
            "SET "
            "GenoEffect.GenoLinkFK=" << mysqlpp::quote << _genolink_fk << ", "
            "GenoEffect.EffectID=" << mysqlpp::quote << _effect_id;
    bool ret = this->query_execute(qry, _geno_effect_sk);
    if (ret == false) return false;
    if (_geno_effect_sk != 0) return ret;
    
    qry.reset();
    qry << "SELECT GenoEffect.GenoEffectSK "
            "FROM GenoEffect "
            "WHERE "
            "GenoEffect.GenoLinkFK=" << mysqlpp::quote << _genolink_fk << " AND "
            "GenoEffect.EffectID=" << mysqlpp::quote << _effect_id;
    mysqlpp::StoreQueryResult str_res;
    ret = this->query_store(qry, str_res);
    if (ret == false) return false;
    assert(str_res.size() == 1);
    _geno_effect_sk = mysqlpp::sql_int_unsigned(atol(str_res[0][0].c_str()));
    return ret;
}

bool GSSDb::insert_citation(
        mysqlpp::sql_int_unsigned _geno_effect_fk,
        mysqlpp::sql_int_unsigned _pm_id)
{
    assert(_geno_effect_fk != 0);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO Citation "
            "SET "
            "Citation.GenoEffectFK=" << mysqlpp::quote << _geno_effect_fk << ", "
            "Citation.PmId=" << mysqlpp::quote << _pm_id;
    return this->query_execute(qry);
}

bool GSSDb::assert_dis_drug_id(
        mysqlpp::sql_varchar _dis_drug_id,
        mysqlpp::sql_enum _type)
{
    assert(_dis_drug_id.empty() == false);
    assert(_type.empty() == false);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT DisDrug.DisDrugID "
            "FROM DisDrug WHERE "
            "DisDrug.DisDrugID=" << mysqlpp::quote << _dis_drug_id << " AND "
            "DisDrug.Type=" << mysqlpp::quote << _type;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    if (str_res.size() != 1) return false;
    return ret;
}

bool GSSDb::get_alleles(
        const string& _chip_type,
        const string& _chip_ver,
        mysqlpp::StoreQueryResult& _res)
{
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT DISTINCT "
            "Assay.AssayIdNK, "
            "Assay.VicAllele, "
            "Assay.FamAllele "
            "FROM Assay "
            "JOIN Chip "
            "ON Assay.ChipFK=Chip.ChipSK "
            "WHERE "
            "Chip.ChipType=" << mysqlpp::quote << _chip_type << " AND "
            "Chip.ChipVer=" << mysqlpp::quote << _chip_ver << " "
            "ORDER BY Assay.AssayIdNK";
    return this->query_store(qry, _res);
}

bool GSSDb::get_dis_drug_gender(
        mysqlpp::sql_varchar _dis_drug_id,
        char &_gender)
{
    assert(_dis_drug_id.empty() == false);
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT "
            "DisDrug.Sex "
            "FROM DisDrug "
            "WHERE "
            "DisDrug.DisDrugID=" << mysqlpp::quote << _dis_drug_id;    
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return false;
    if (str_res.size() != 1) return false;
    _gender = str_res[0][0].c_str()[0];
    return ret;
}

bool GSSDb::get_all(
        const string& _chip_type,
        const string& _chip_ver,
        mysqlpp::StoreQueryResult &_res)
{
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT DISTINCT "
            "DisDrug.DisDrugID, "
            "DisDrug.Type, "
            "DisDrug.Sex, "
            "DisDrug.Incidence, "      //syhwah 2013-04-02
            "DisDrug.Trait, "          //syhwah 2013-04-03
            "Effects.EffectID, "
            "Effects.Risk, "
            "Effects.GenotypeCount, "
            "Assay.AssayIdNK, "
            "Assay.RsId, "
            "Genotype.AlleleOne, "
            "Genotype.AlleleTwo, "
            "RelativeRisk.RiskAllele, "
            "RelativeRisk.RR_Factor, "
            "RelativeRisk.RN_Factor, "
            "RelativeRisk.NN_Factor, "
            "Citation.PmId "
            ""
            "FROM DisDrug "
            ""
            "JOIN Effects "
            "ON DisDrug.DisDrugID=Effects.DisDrugID "
            "JOIN GenoEffect "
            "ON Effects.EffectID=GenoEffect.EffectID "
            "JOIN Citation "
            "ON GenoEffect.GenoEffectSK=Citation.GenoEffectFK "
            "JOIN GenoLink "
            "ON GenoEffect.GenoLinkFK=GenoLink.GenoLinkSK "
            "JOIN Genotype "
            "ON (GenoLink.GenoCurrFK=Genotype.GenotypeSK OR GenoLink.GenoNextFK=Genotype.GenotypeSK) "
            "JOIN Assay "
            "ON Genotype.AssayFK=Assay.AssaySK "
            "JOIN Chip "
            "ON Assay.ChipFK=Chip.ChipSK "
            "LEFT OUTER JOIN RelativeRisk "
            "ON (Assay.AssaySK=RelativeRisk.AssayFK) AND (DisDrug.DisDrugID=RelativeRisk.DisDrugID) "
            ""
            "WHERE "
            // remove for G2 report "Assay.RsId not in (1867277,2241712,41291556,2241712,28493229) AND " //syhwah - 20140416 -quick and dirty solution
            "Chip.ChipType=" << mysqlpp::quote << _chip_type << " AND "
            "Chip.ChipVer=" << mysqlpp::quote << _chip_ver;
    bool ret = this->query_store(qry, _res);
    if (ret == false) return false;
    return ret;
}

bool GSSDb::get_rsid(
        mysqlpp::StoreQueryResult& _res)
{
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT DISTINCT "
            "GSSDb.RsId, "
            "GSSDb.AssayIdNK, "
            "GSSDb.VicAllele, "
            "GSSDb.FamAllele "
            "FROM "
            "GSSDb.Assay";
    return this->query_store(qry, _res);
}
