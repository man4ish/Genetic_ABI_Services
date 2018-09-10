/*
 * File:   QSDb_Public.h
 * Author: jameswong
 *
 * Created on June 4, 2012, 4:09 PM
 */

#define EXPAND_MY_SSQLS_STATICS
#include "QSDb.h"
#include "QSDb_Func.h"

#include <stdio.h>
#include <iostream>
#include <mysql++/connection.h>

using namespace std;

QSDb::QSDb(const char *_dbname) : DbBase(_dbname)
{
}

QSDb::~QSDb()
{
}

bool QSDb::init(
        const char* _host,
        const char* _user,
        const char* _pswd)
{
    if (this->connect(_host, _user, _pswd, false) == false)
        return false;
    return (this->check_table());
}

bool QSDb::update_status(
        const string& _sample_id,
        const string& _chip_type,
        const string& _chip_ver,
        const string& _open_array_id,
        char _status,
        int _valid_calls,
        int _invalid_calls,
        int _amb_calls,
        int _omitted_calls) //syhwah - 20130404 : added _omitted_calls
{
    mysqlpp::Query qry = this->db_conn.query();
    qry << "UPDATE SampleRun, OpenArray "
            "SET "
            "SampleRun.Status=" << mysqlpp::quote << _status << ", "
            "SampleRun.ValidCalls=" << mysqlpp::quote << _valid_calls << ", "
            "SampleRun.InvalidCalls=" << mysqlpp::quote << _invalid_calls << ", "
            "SampleRun.AmbCalls=" << mysqlpp::quote << _amb_calls << ", "
            "SampleRun.OmittedCalls=" << mysqlpp::quote << _omitted_calls << " " //syhwah - 20130404 
            "WHERE "
            "SampleRun.SampleId=" << mysqlpp::quote << _sample_id << " AND "
            "OpenArray.OpenArrayIdNK=" << mysqlpp::quote << _open_array_id << " AND "
            "OpenArray.ChipType=" << mysqlpp::quote << _chip_type << " AND "
            "OpenArray.ChipVer=" << mysqlpp::quote << _chip_ver << " AND "
            "OpenArray.ArraySK=SampleRun.ArrayFK";
    mysqlpp::SimpleResult sim_res;
    bool ret = this->query_execute(qry, sim_res);
    if (ret == false) return false;
    if ((sim_res.rows() != 1) && (sim_res.rows() != 0)) return false;
    return ret;
}

bool QSDb::insert_open_array(
        mysqlpp::sql_bigint_unsigned& _array_key,
        mysqlpp::sql_varchar _open_array_id,
        mysqlpp::sql_date _test_date,
        mysqlpp::sql_char _chip_type,
        mysqlpp::sql_char _chip_ver)
{
    _array_key = 0;
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO OpenArray "
            "SET "
            "OpenArray.OpenArrayIdNK=" << mysqlpp::quote << _open_array_id << ", "
            "OpenArray.TestDate=" << mysqlpp::quote << _test_date << ", "
            "OpenArray.ChipType=" << mysqlpp::quote << _chip_type << ", "
            "OpenArray.ChipVer=" << mysqlpp::quote << _chip_ver;
    bool ret = this->query_execute(qry, _array_key);
    if (ret == false) return false;
    if (_array_key != 0) return ret;
    
    qry.reset();
    qry << "SELECT OpenArray.ArraySK "
            "FROM OpenArray "
            "WHERE "
            "OpenArray.OpenArrayIdNK=" << mysqlpp::quote << _open_array_id << " AND "
            "OpenArray.TestDate=" << mysqlpp::quote << _test_date << " AND "
            "OpenArray.ChipType=" << mysqlpp::quote << _chip_type << " AND "
            "OpenArray.ChipVer=" << mysqlpp::quote << _chip_ver;
    mysqlpp::StoreQueryResult str_res;
    ret = this->query_store(qry, str_res);
    if (ret == false) return false;
    if (str_res.size() != 1) return false;
    _array_key = atol(str_res[0][0].c_str());
    return ret;
}

bool QSDb::insert_samples(
        mysqlpp::sql_bigint_unsigned& _sample_sk,
        mysqlpp::sql_bigint_unsigned _array_key,
        mysqlpp::sql_char _sample_id,
        mysqlpp::sql_char_null _status,
        mysqlpp::sql_int_unsigned_null _valid_calls,
        mysqlpp::sql_int_unsigned_null _invalid_calls,
        mysqlpp::sql_int_unsigned_null _amb_calls,
        mysqlpp::sql_boolean _ready)
{
    _sample_sk = 0;
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO SampleRun "
            "SET "
            "SampleRun.ArrayFK=" << mysqlpp::quote << _array_key << ", "
            "SampleRun.SampleId=" << mysqlpp::quote << _sample_id << ", "
            "SampleRun.Status=" << mysqlpp::quote << _status << ", "
            "SampleRun.ValidCalls=" << mysqlpp::quote << _valid_calls << ", "
            "SampleRun.InvalidCalls=" << mysqlpp::quote << _invalid_calls << ", "
            "SampleRun.AmbCalls=" << mysqlpp::quote << _amb_calls << ", "
            "SampleRun.Ready=" << mysqlpp::quote << _ready;
    bool ret = this->query_execute(qry, _sample_sk);
    if (ret == false) return false;
    if (_sample_sk != 0) return ret;
    
    qry.reset();
    qry << "SELECT SampleRun.SampleSK "
            "FROM SampleRun "
            "WHERE "
            "SampleRun.ArrayFK=" << mysqlpp::quote << _array_key << " AND "
            "SampleRun.SampleId=" << mysqlpp::quote << _sample_id;
    mysqlpp::StoreQueryResult str_res;
    ret = this->query_store(qry, str_res);
    if (ret == false) return false;
    if (str_res.size() != 1) return false;
    _sample_sk = atol(str_res[0][0].c_str());
    return ret;
}

bool QSDb::update_sample(
        mysqlpp::sql_bigint_unsigned _sample_key,
        mysqlpp::sql_boolean _ready)
{
    assert(_sample_key != 0);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "UPDATE SampleRun "
            "SET SampleRun.Ready=" << mysqlpp::quote << _ready << " "
            "WHERE "
            "SampleRun.SampleSK=" << mysqlpp::quote << _sample_key;
    return this->query_execute(qry);
}

bool QSDb::insert_results(
        mysqlpp::sql_bigint_unsigned &_results_sk,
        mysqlpp::sql_bigint_unsigned _sample_fk,
        mysqlpp::sql_varchar _assay_id,
        mysqlpp::sql_varchar _vic_fam_call)
{
    assert(_sample_fk != 0);
    _results_sk = 0;
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO RunResult "
            "SET "
            "RunResult.SampleFK=" << mysqlpp::quote << _sample_fk << ", "
            "RunResult.AssayId=" << mysqlpp::quote << _assay_id << ", "
            "RunResult.VicFamCall=" << mysqlpp::quote << _vic_fam_call;
    bool ret = this->query_execute(qry, _results_sk);
    if (ret == false) return false;
    if (_results_sk != 0) return ret;
    
    qry.reset();
    qry << "SELECT RunResult.ResultSK "
            "FROM RunResult "
            "WHERE "
            "RunResult.SampleFK=" << mysqlpp::quote << _sample_fk << " AND "
            "RunResult.AssayId=" << mysqlpp::quote << _assay_id << " AND "
            "RunResult.VicFamCall=" << mysqlpp::quote << _vic_fam_call;
    mysqlpp::StoreQueryResult str_res;
    ret = this->query_store(qry, str_res);
    if (ret == false) return false;
    if (str_res.size() != 1) return false;
    _results_sk = atol(str_res[0][0].c_str());
    return ret;
}

bool QSDb::insert_run_stats(
        mysqlpp::sql_bigint_unsigned _result_key,
        mysqlpp::sql_char _well,
        mysqlpp::sql_enum _manual,
        mysqlpp::sql_double _quality,
        mysqlpp::sql_double _vic_rn,
        mysqlpp::sql_double _fam_rn,
        mysqlpp::sql_double _rox)
{
    assert(_result_key != 0);
    assert(_well.length() <= 5);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO AssayStats "
            "SET "
            "AssayStats.ResultFK=" << mysqlpp::quote << _result_key << ", "
            "AssayStats.Well=" << mysqlpp::quote << _well << ", "
            "AssayStats.Manual=" << mysqlpp::quote << _manual << ", "
            "AssayStats.Quality=" << mysqlpp::quote << _quality << ", "
            "AssayStats.VicRn=" << mysqlpp::quote << _vic_rn << ", "
            "AssayStats.FamRn=" << mysqlpp::quote << _fam_rn << ", "
            "AssayStats.Rox=" << mysqlpp::quote << _rox;
    return this->query_execute(qry);
}

bool QSDb::get_gbl_assay(
        mysqlpp::StoreQueryResult& _res)
{
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT DISTINCT "
            "GlobalPopulation.AssayId, "
            "GlobalPopulation.VicFamCall, "
            "GlobalPopulation.PopulationSize, "
            "GlobalPopulation.PopulationName "
            "FROM GlobalPopulation";
    return this->query_store(qry, _res);
}

bool QSDb::insert_population(
        mysqlpp::sql_varchar _assay_id,
        mysqlpp::sql_varchar _vicfam_allele,
        mysqlpp::sql_varchar _population_name,
        mysqlpp::sql_int _population_size)
{
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO GlobalPopulation "
            "SET "
            "GlobalPopulation.AssayId=" << mysqlpp::quote << _assay_id << ", "
            "GlobalPopulation.VicFamCall=" << mysqlpp::quote << _vicfam_allele << ", "
            "GlobalPopulation.PopulationName=" << mysqlpp::quote << _population_name << ", "
            "GlobalPopulation.PopulationSize=" << mysqlpp::quote << _population_size;
    return this->query_execute(qry);
}
