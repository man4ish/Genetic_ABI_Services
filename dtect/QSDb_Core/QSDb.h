/* 
 * File:   QSDb.h
 * Author: jameswong
 *
 * Created on June 4, 2012, 4:09 PM
 */

#ifndef QSDB_H
#define	QSDB_H

#include "DbBase.h"

class QSDb : public DbBase
{
private:        // private member functions
    bool check_table();
    
public:
    QSDb(const char *_dbname);
    ~QSDb();
    
    bool init(
        const char *_host,
        const char *_user,
        const char *_pswd);
    
    bool update_status(
        const string &_sample_id,
        const string &_chip_type,
        const string &_chip_ver,
        const string &_open_array_id,
        char _status,
        int _valid_calls,
        int _invalid_calls,
        int _amb_calls,
        int _omitted_calls); //syhwah 20130404 : added _omitted_calls
    
    bool insert_open_array(
        mysqlpp::sql_bigint_unsigned &_array_key,
        mysqlpp::sql_varchar _open_array_id,
        mysqlpp::sql_date _test_date,
        mysqlpp::sql_char _chip_type,
        mysqlpp::sql_char _chip_ver);
    
    bool insert_samples(
        mysqlpp::sql_bigint_unsigned &_sample_sk,
        mysqlpp::sql_bigint_unsigned _array_key,
        mysqlpp::sql_char _sample_id,
        mysqlpp::sql_char_null _status,
        mysqlpp::sql_int_unsigned_null _valid_calls,
        mysqlpp::sql_int_unsigned_null _invalid_calls,
        mysqlpp::sql_int_unsigned_null _amb_calls,
        mysqlpp::sql_boolean _ready);
    
    bool update_sample(
        mysqlpp::sql_bigint_unsigned _sample_key,
        mysqlpp::sql_boolean _ready);
    
    bool insert_results(
        mysqlpp::sql_bigint_unsigned &_results_sk,
        mysqlpp::sql_bigint_unsigned _sample_fk,
        mysqlpp::sql_varchar _assay_id,
        mysqlpp::sql_varchar _vic_fam_call);
    
    bool insert_run_stats(
        mysqlpp::sql_bigint_unsigned _result_key,
        mysqlpp::sql_char _well,
        mysqlpp::sql_enum _manual,
        mysqlpp::sql_double _quality,
        mysqlpp::sql_double _vic_rn,
        mysqlpp::sql_double _fam_rn,
        mysqlpp::sql_double _rox);
    
    bool get_gbl_assay(
        mysqlpp::StoreQueryResult &_res);
    
    bool insert_population(
        mysqlpp::sql_varchar _assay_id,
        mysqlpp::sql_varchar _vicfam_allele,
        mysqlpp::sql_varchar _population_name,
        mysqlpp::sql_int _population_size);
};

#endif	/* QSDB_H */

