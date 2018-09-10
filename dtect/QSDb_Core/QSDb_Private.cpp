/* 
 * File:   QSDb_Private.h
 * Author: jameswong
 *
 * Created on June 4, 2012, 4:09 PM
 */

#include <stdio.h>
#include <iostream>
#include "QSDb.h"
#include "QSDb_Func.h"

using namespace std;

bool QSDb::check_table()
{
    mysqlpp::Query qry = this->db_conn.query();
    bool ret;
    
    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "OpenArray("
            "ArraySK BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,"
            "OpenArrayIdNK VARCHAR(16) NOT NULL,"
            "TestDate DATE NOT NULL,"
            "ChipType CHAR(3) NOT NULL,"
            "ChipVer CHAR(2) NOT NULL,"
            "PRIMARY KEY (ArraySK),"
            "UNIQUE(OpenArrayIdNK, TestDate, ChipType, ChipVer)"
            ")";
    ret = this->query_execute(qry);
    assert(ret == true);
    
    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "SampleRun("
            "SampleSK BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,"
            "ArrayFK BIGINT UNSIGNED NOT NULL,"
            "SampleId VARCHAR(64) NOT NULL,"
            "Status CHAR(1) DEFAULT NULL,"
            "ValidCalls INT UNSIGNED DEFAULT 0,"
            "InvalidCalls INT UNSIGNED DEFAULT 0,"
            "AmbCalls INT UNSIGNED DEFAULT 0,"
            "OmittedCalls INT UNSIGNED DEFAULT 0,"
            "Ready BOOLEAN,"
            "PRIMARY KEY (SampleSK),"
            "FOREIGN KEY (ArrayFK) REFERENCES OpenArray(ArraySK),"
            "UNIQUE (ArrayFK, SampleId)"
            ")";
    ret = this->query_execute(qry);
    assert(ret == true);
    
    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "RunResult("
            "ResultSK BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,"
            "SampleFK BIGINT UNSIGNED NOT NULL,"
            "AssayId VARCHAR(16) NOT NULL,"
            "VicFamCall VARCHAR(8) NOT NULL,"
            "PRIMARY KEY(ResultSK),"
            "FOREIGN KEY(SampleFK) REFERENCES SampleRun(SampleSK),"
            "UNIQUE (SampleFK, AssayId, VicFamCall)"
            ")";
    ret = this->query_execute(qry);
    assert(ret == true);
    
    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "AssayStats("
            "ResultFK BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,"
            "Well CHAR(5) NOT NULL,"
            "Manual ENUM('TRUE', 'FALSE') NOT NULL,"
            "Quality DOUBLE NOT NULL,"
            "VicRn DOUBLE NOT NULL,"
            "FamRn DOUBLE NOT NULL,"
            "Rox DOUBLE NOT NULL,"
            "FOREIGN KEY (ResultFK) REFERENCES RunResult(ResultSK),"
            "UNIQUE (Well, Manual, Quality, VicRn, FamRn, Rox)"
            ")";
    ret = this->query_execute(qry);
    assert(ret == true);
    
    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "GlobalPopulation("
            "AssayId VARCHAR(16) NOT NULL,"
            "VicFamCall VARCHAR(8) NOT NULL,"
            "PopulationName VARCHAR(64) NOT NULL,"
            "PopulationSize INT UNSIGNED NOT NULL,"
            "PRIMARY KEY (AssayId, VicFamCall, PopulationName)"
            ")";
    ret = this->query_execute(qry);
    assert(ret == true);
    
    return ret;
}
