/* 
 * File:   OpenDb.cpp
 * Author: jameswong
 * 
 * Created on February 23, 2013, 10:15 PM
 */

#include "OpenDb.h"

OpenDb::OpenDb()
{
}

OpenDb::~OpenDb()
{
    this->disconnect();
}

bool OpenDb::init(
        const string& _host,
        const string& _user,
        const string& _pswd)
{
    return this->connect(_host.c_str(), _user.c_str(), _pswd.c_str());
}

bool OpenDb::get_outstanding_samples_from_limsdb_n_qsdb(
        mysqlpp::StoreQueryResult& _res)
{
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT DISTINCT "
            "LIMS_DB.SampleTest.SampleID, "
            "LIMS_DB.SampleTest.TestCode, "
            "LIMS_DB.SampleTest.LastOpenArrayID "
            "FROM LIMS_DB.SampleTest "
            "JOIN QSDb.SampleRun "
            "ON LIMS_DB.SampleTest.SampleID=QSDb.SampleRun.SampleID "
            "JOIN QSDb.OpenArray "
            "ON QSDb.SampleRun.ArrayFK=QSDb.OpenArray.ArraySK "
            ""
            "WHERE "
            "LIMS_DB.SampleTest.LastReportGenerationDate IS NULL "
            "AND "
            "LIMS_DB.SampleTest.LastScreeningTestStatus IN ('R', 'Y') "
            "AND "
            "QSDb.SampleRun.Ready=1 "
            "AND CONCAT(QSDb.OpenArray.ChipType, QSDb.OpenArray.ChipVer)=LIMS_DB.SampleTest.TestCode";
    return this->query_store(qry, _res);
}

bool OpenDb::map_sample_from_qsdb_against_gssdb(
        const string& _sample_id,
        const string& _open_array_id,
        const string& _chip_type,
        const string& _chip_ver,
        mysqlpp::StoreQueryResult& _res)
{
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT DISTINCT "
            "QSDb.RunResult.AssayId, "
            "QSDb.RunResult.VicFamCall, "
            "GSSDb.Assay.VicAllele, "
            "GSSDb.Assay.FamAllele "
            "FROM GSSDb.Assay "
            "JOIN GSSDb.Chip "
            "ON GSSDb.Assay.ChipFK=GSSDb.Chip.ChipSK "
            "JOIN QSDb.RunResult "
            "ON GSSDb.Assay.AssayIdNK=QSDb.RunResult.AssayId "
            "JOIN QSDb.SampleRun "
            "ON QSDb.RunResult.SampleFK=QSDb.SampleRun.SampleSK "
            "JOIN QSDb.OpenArray "
            "ON QSDb.SampleRun.ArrayFK=QSDb.OpenArray.ArraySK "
            ""
            "WHERE "
            "GSSDb.Chip.ChipType=" << mysqlpp::quote << _chip_type <<" AND "
            "GSSDb.Chip.ChipVer=" << mysqlpp::quote << _chip_ver << " AND "
            "QSDb.SampleRun.SampleId=" << mysqlpp::quote << _sample_id << " AND "
            "QSDb.OpenArray.OpenArrayIdNK=" << mysqlpp::quote << _open_array_id << " "
            "ORDER BY QSDb.RunResult.AssayId";
    return this->query_store(qry, _res);
}