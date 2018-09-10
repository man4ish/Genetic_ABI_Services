/* 
 * File:   LimsDb.h
 * Author: jameswong
 *
 * Created on June 15, 2012, 4:55 PM
 */

#ifndef LIMSDB_H
#define	LIMSDB_H

#include "DbBase.h"
#include <string>

using namespace std;

class LimsDb : public DbBase
{
public:
    LimsDb(const char *_dbname);
    ~LimsDb();
    
    bool init(
        const string& _host,
        const string& _user,
        const string& _pswd);
    
    bool is_valid(
        const string& _sample_id,
        const string& _open_array_id,
        string& _test_code);
    
    bool set_status_in_sample_test_run(
        const string& _sample_id,
        const string& _open_array_id,
        const string& _testcode,
        char _status);
    
    bool set_status_in_sample_test(
        const string& _sample_id,
        const string& _testcode,
        char _status);
    
    bool set_report_gen_date_in_sample_test_run(
        const string& _sample_id,
        const string& _open_array_id,
        const string& _testcode);
    
    bool set_report_gen_date_in_sample_test(
        const string& _sample_id,
        const string& _testcode);
    
    bool set_report_filename(
        const string& _sample_id,
        const string& _testcode,
        const string& _lang_code,
        const string& _filename);
    
    bool get_patient_details(
        const string &_sample_id,
        mysqlpp::sql_varchar &_patient_id,
        mysqlpp::sql_varchar &_patient_name,
        mysqlpp::sql_char &_gender,
        mysqlpp::sql_date &_dob,
        mysqlpp::sql_varchar &_address,
        mysqlpp::sql_varchar &_phone,
        mysqlpp::sql_varchar &_ethnic);
    
    bool get_physician_details(
        const string& _sample_id,
        mysqlpp::sql_varchar &_physician_name,
        mysqlpp::sql_varchar &_physician_add);
    
    bool get_sample_collection_date(
        const string& _sample_id,
        mysqlpp::sql_date &_sample_collection_date);
    
    bool get_specimen_description(
        const string& _sample_id,
        mysqlpp::sql_varchar &_specimen_desc);
    
    /*
    bool get_received_date(
        const string& _sample_id,
        mysqlpp::sql_date &_received_date);
    */

    bool get_received_date_and_InterpretResult( //syhwah 2013-03-13
        const string& _sample_id,
        mysqlpp::sql_date &_received_date,
        string& _InterpretResult);

    bool get_test_code(
        const string& _open_array_id,
        mysqlpp::sql_char &_testcode);
    
    bool get_lang_code(
        const string& _sample_id,
        set<string> &_lang_set);

    //syhwah 2013-04-18
    bool get_partner_code(
        const string& _sample_id,
        mysqlpp::sql_char& _partnercode);
};

#endif	/* LIMSDB_H */
