/* 
 * File:   GSSDb.h
 * Author: jameswong
 *
 * Created on May 21, 2012, 2:43 PM
 */

#ifndef GSSDB_H
#define	GSSDB_H

#include <cstdarg>
#include "DbBase.h"

class GSSDb : public DbBase
{
private:        // private member functions
    bool check_table();
    
public:         // public member functions
    GSSDb(const char *_dbname);
    ~GSSDb();
    
    bool init(
        const char *_host,
        const char *_user,
        const char *_pswd);
    
    bool insert_chip(
        mysqlpp::sql_int_unsigned &_chip_sk,
        mysqlpp::sql_char _chip_type,
        mysqlpp::sql_char _chip_ver);
    
    bool get_chip_key(
        mysqlpp::sql_int_unsigned &_chip_key,
        mysqlpp::sql_char _chip_type,
        mysqlpp::sql_char _chip_ver);
    
    bool insert_assay(
        mysqlpp::sql_int_unsigned &_assay_sk,
        mysqlpp::sql_int_unsigned _chip_fk,
        mysqlpp::sql_int_unsigned _rs_id,
        mysqlpp::sql_varchar _assay_id,
        mysqlpp::sql_enum _vic_allele,
        mysqlpp::sql_enum _fam_allele);
    
    bool insert_risk_factor(
        mysqlpp::sql_int_unsigned _assay_sk,
        mysqlpp::sql_varchar _dis_drug_id,
        mysqlpp::sql_char _risk_allele,
        double _rr_factor,
        double _rn_factor,
        double _nn_factor);
    
    bool get_assay_key(
        mysqlpp::sql_int_unsigned &_assay_sk,
        mysqlpp::sql_int_unsigned _chip_fk,
        mysqlpp::sql_int_unsigned _rs_id);
    
    bool get_assay_key(
        mysqlpp::sql_int_unsigned &_assay_key,
        mysqlpp::sql_varchar _assay_id,
        mysqlpp::sql_char _chip_type,
        mysqlpp::sql_char _chip_ver);
    
    bool insert_genotype(
        mysqlpp::sql_int_unsigned &_genotype_sk,
        mysqlpp::sql_int_unsigned _assay_fk,
        mysqlpp::sql_enum _allele_one,
        mysqlpp::sql_enum _allele_two);
    
    bool insert_genolink(
        mysqlpp::sql_int_unsigned &_genolink_sk,
        mysqlpp::sql_int_unsigned _geno_one_fk,
        mysqlpp::sql_int_unsigned _geno_two_fk);
    
    bool insert_dis_drug(
        mysqlpp::sql_varchar _dis_drug_id,
        mysqlpp::sql_enum _type,
        mysqlpp::sql_enum _sex,
        double _Incidence, //syhwah 2013-04-02: added _Incidence
        mysqlpp::sql_enum _trait); //syhwah 2013-04-02: added _trait
    
    bool insert_effect(
        mysqlpp::sql_varchar _effect_id,
        mysqlpp::sql_varchar _dis_drug_id,
        mysqlpp::sql_tinyint_unsigned _genotype_cnt,
        mysqlpp::sql_double _risk);
    
    bool insert_geno_effect(
        mysqlpp::sql_int_unsigned &_geno_effect_sk,
        mysqlpp::sql_int_unsigned _genolink_fk,
        mysqlpp::sql_varchar _effect_id);
    
    bool insert_citation(
        mysqlpp::sql_int_unsigned _geno_effect_fk,
        mysqlpp::sql_int_unsigned _pm_id);
    
    bool assert_dis_drug_id(
        mysqlpp::sql_varchar _dis_drug_id,
        mysqlpp::sql_enum _type);
    
    bool get_dis_drug_gender(
        mysqlpp::sql_varchar _dis_drug_id,
        char &_gender);
    
public: // search function
    bool search_dis_drug(
        set<string>& _dis_drug_set,
        const string& _chip_type,
        const string& _chip_ver,
        const set<string>* _assay_set);
    
    bool get_dis_drug_cnt(
        size_t& _count,
        const string& _chip_type,
        const string& _chip_ver);
    
    bool get_assay_cnt(
        int& _count,
        const string& _chip_type,
        const string& _chip_ver);
    
    bool get_alleles(
        const string& _chip_type,
        const string& _chip_ver,
        mysqlpp::StoreQueryResult& _res);
    
    bool get_all(
        const string& _chip_type,
        const string& _chip_ver,
        mysqlpp::StoreQueryResult& _res);
    
    bool get_rsid(
        mysqlpp::StoreQueryResult& _res);
};

#endif	/* GSSDB_H */

