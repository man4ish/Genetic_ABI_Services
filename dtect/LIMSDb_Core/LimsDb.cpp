/* 
 * File:   LimsDb.cpp
 * Author: jameswong
 * 
 * Created on June 15, 2012, 4:55 PM
 */

#include <mysql++/query.h>
#include "LimsDb.h"

LimsDb::LimsDb(const char *_dbname) : DbBase(_dbname)
{
}

LimsDb::~LimsDb()
{
    this->disconnect();
}

bool LimsDb::init(
        const string& _host,
        const string& _user,
        const string& _pswd)
{
    assert(_host.empty() == false);
    assert(_user.empty() == false);
    
    //return this->connect(_host.c_str(), _user.c_str(), _pswd.c_str(), false);
    
    bool ret = this->connect(_host.c_str(), _user.c_str(), _pswd.c_str(), false);

    assert(ret == true);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SET NAMES utf8";
    ret = this->query_execute(qry);
    assert(ret == true);
    return ret; 
}

bool LimsDb::is_valid(
        const string& _sample_id,
        const string& _open_array_id,
        string& _test_code)
{
    assert(_sample_id.length() == 6);
    assert(_open_array_id.length() == 5);
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT SampleTestRun.TestCode "
            "FROM SampleTestRun "
            "WHERE BINARY "
            "SampleTestRun.SampleId=" << mysqlpp::quote << _sample_id << " AND "
            "SampleTestRun.OpenArrayID=" << mysqlpp::quote << _open_array_id;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    if (str_res.size() != 1) return false;
    
    _test_code.assign(str_res[0]["TestCode"].c_str());
    return ret;
}

bool LimsDb::set_status_in_sample_test_run(
        const string& _sample_id,
        const string& _open_array_id,
        const string& _testcode,
        char _status)
{
    assert(_sample_id.length() == 6);
    assert(_open_array_id.length() == 5);
    assert(_testcode.length() == 5);
    
    mysqlpp::sql_char lims_status;
    if (_status == 'O')
        lims_status = "Y";
    else
        lims_status = "N";
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "UPDATE SampleTestRun "
            "SET "
            "SampleTestRun.ReasonCode=" << mysqlpp::quote << _status << ", "
            "SampleTestRun.Status=" << mysqlpp::quote << lims_status << " "
            "WHERE "
            "SampleTestRun.SampleID=" << mysqlpp::quote << _sample_id << " AND "
            "SampleTestRun.OpenArrayID=" << mysqlpp::quote << _open_array_id << " AND "
            "SampleTestRun.TestCode=" << mysqlpp::quote << _testcode;
    return this->query_execute(qry);
}

bool LimsDb::set_status_in_sample_test(
        const string& _sample_id,
        const string& _testcode,
        char _status)
{
    assert(_sample_id.length() == 6);
    assert(_testcode.length() == 5);
    
    mysqlpp::sql_char lims_status;
    if (_status == 'O')
        lims_status = "Y";
    else
        lims_status = "N";
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "UPDATE SampleTest "
            "SET "
            "SampleTest.LastScreeningTestStatus=" << mysqlpp::quote << lims_status << " "
            "WHERE "
            "SampleTest.SampleID=" << mysqlpp::quote << _sample_id << " AND "
            "SampleTest.TestCode=" << mysqlpp::quote << _testcode;
    return (this->query_execute(qry));
}

bool LimsDb::set_report_gen_date_in_sample_test_run(
        const string& _sample_id,
        const string& _open_array_id,
        const string& _testcode)
{
    assert(_sample_id.length() == 6);
    assert(_open_array_id.length() == 5);
    assert(_testcode.length() == 5);
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "UPDATE SampleTestRun "
            "SET "
            "SampleTestRun.ReportGenerationDate=DATE(NOW()) "
            "WHERE "
            "SampleTestRun.SampleID=" << mysqlpp::quote << _sample_id << " AND "
            "SampleTestRun.OpenArrayID=" << mysqlpp::quote << _open_array_id << " AND "
            "SampleTestRun.TestCode=" << mysqlpp::quote << _testcode;
    return (this->query_execute(qry));
}

bool LimsDb::set_report_gen_date_in_sample_test(
        const string& _sample_id,
        const string& _testcode)
{
    assert(_sample_id.length() == 6);
    assert(_testcode.length() == 5);
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "UPDATE SampleTest "
            "SET "
            "SampleTest.LastReportGenerationDate=DATE(NOW()) "
            "WHERE "
            "SampleTest.SampleID=" << mysqlpp::quote << _sample_id << " AND "
            "SampleTest.TestCode=" << mysqlpp::quote << _testcode;
    return (this->query_execute(qry));
}

bool LimsDb::set_report_filename(
        const string& _sample_id,
        const string& _testcode,
        const string& _lang_code,
        const string& _filename)
{
    assert(_sample_id.length() == 6);
    assert(_testcode.length() == 5);
    assert(_lang_code.length() == 2);
    assert(_filename.empty() == false);
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "UPDATE SampleTestReport "
            "SET "
            "SampleTestReport.Filename=" << mysqlpp::quote << _filename << " "
            "WHERE "
            "SampleTestReport.SampleID=" << mysqlpp::quote << _sample_id << " AND "
            "SampleTestReport.TestCode=" << mysqlpp::quote << _testcode << " AND "
            "SampleTestReport.LanguageCode=" << mysqlpp::quote << _lang_code;
    return (this->query_execute(qry));
}

bool LimsDb::get_patient_details(
        const string &_sample_id,
        mysqlpp::sql_varchar& _patient_id,
        mysqlpp::sql_varchar& _patient_name,
        mysqlpp::sql_char& _gender,
        mysqlpp::sql_date& _dob,
        mysqlpp::sql_varchar& _address,
        mysqlpp::sql_varchar& _phone,
        mysqlpp::sql_varchar& _race)
{
    assert(_sample_id.length() == 6);
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SET NAMES utf8";
    bool ret = this->query_execute(qry);
    assert(ret == true);

    //mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT "
            "Patient.PatientID, "
            "Patient.Name, "
            "Patient.Gender, "
            "Patient.DOB, "
            "Patient.Address, "
            "Patient.Phone, "
            "Patient.Race, "
            "Patient.PostalCode, "
            "Patient.City, "
            "Patient.State, "     
            "Country.Name as CountryName "                
            "FROM Patient "
            "JOIN Sample "
            "ON Patient.PatientID=Sample.PatientID "
            ", Country "   
            "WHERE Sample.SampleID=" << mysqlpp::quote << _sample_id <<
            " AND Patient.Country = Country.CountryCode";

    mysqlpp::StoreQueryResult str_res;
    ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    
    assert(str_res.size() == 1);
    _patient_id.assign(str_res[0]["PatientID"].c_str());
    _patient_name.assign(str_res[0]["Name"].c_str());
    _gender.assign(str_res[0]["Gender"].c_str());
    _dob.convert(str_res[0]["DOB"].c_str());
    _address.assign(str_res[0]["Address"].c_str());

    int nLen = _address.size();
    if (_address[nLen-1] != '`') { _address.append("`"); } 
    
    if ((str_res[0]["PostalCode"].compare("NULL") !=0) && (!str_res[0]["PostalCode"].empty())) { 
        _address.append(str_res[0]["PostalCode"].c_str()); _address.append(" "); 
    }
      
    if ((str_res[0]["City"].compare("NULL") !=0) && (!str_res[0]["City"].empty())) { 
        _address.append(str_res[0]["City"].c_str());
    }
    
    _address.append("`");   

    if ((str_res[0]["State"].compare("NULL") !=0) && (!str_res[0]["State"].empty())) { 
        _address.append(str_res[0]["State"].c_str()); _address.append(", "); 
    }
    
    _address.append(str_res[0]["CountryName"].c_str()); 

    _phone.assign(str_res[0]["Phone"].c_str());
    _race.assign(str_res[0]["Race"].c_str());
    
    return ret;
}

bool LimsDb::get_physician_details(
        const string& _sample_id,
        mysqlpp::sql_varchar& _physician_name,
        mysqlpp::sql_varchar& _physician_add)
{
    assert(_sample_id.length() == 6);

    mysqlpp::Query qry = this->db_conn.query();
    qry << "SET NAMES utf8";
    bool ret = this->query_execute(qry);
    assert(ret == true);
    
    //mysqlpp::Query qry = this->db_conn.query();

    qry << "SELECT "
            "Doctor.DocName, "
            "Doctor.Address, "
            "Doctor.PostalCode, "
            "Doctor.City, "
            "Doctor.State, "   
            "Country.Name as CountryName " 
            "FROM Doctor "
            "JOIN OrderForm "
            "ON Doctor.DocName=OrderForm.DocName "
            "JOIN Sample "
            "ON OrderForm.OrderID=Sample.OrderID "
            ", Country " 
            "WHERE Sample.SampleID=" << mysqlpp::quote << _sample_id <<
            " AND Doctor.Country = Country.CountryCode";

    mysqlpp::StoreQueryResult str_res;
    ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    
    assert(str_res.size() == 1);
    _physician_name.assign(str_res[0]["DocName"].c_str());

    _physician_add.assign(str_res[0]["Address"].c_str());

    int nLen = _physician_add.size();
    if (_physician_add[nLen-1] != '`') { _physician_add.append("`"); } 
    
    if ((str_res[0]["PostalCode"].compare("NULL") != 0) && (!str_res[0]["PostalCode"].empty())) { 
        _physician_add.append(str_res[0]["PostalCode"].c_str()); _physician_add.append(" "); 
    }

    if ((str_res[0]["City"].compare("NULL") != 0) && (!str_res[0]["City"].empty())) {
        _physician_add.append(str_res[0]["City"].c_str()); 
    }
 
    _physician_add.append("`");

    if ((str_res[0]["State"].compare("NULL") !=0) && (!str_res[0]["State"].empty())) { 
        _physician_add.append(str_res[0]["State"].c_str()); _physician_add.append(", "); 
    }
    
    _physician_add.append(str_res[0]["CountryName"].c_str());

    return ret;
}

bool LimsDb::get_sample_collection_date(
        const string& _sample_id,
        mysqlpp::sql_date& _sample_collection_date)
{
    assert(_sample_id.length() == 6);
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT Sample.SampleCollectionDate "
            "FROM Sample "
            "WHERE Sample.SampleID=" << mysqlpp::quote << _sample_id;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    
    assert(str_res.size() == 1);
    _sample_collection_date.convert(str_res[0]["SampleCollectionDate"].c_str());
    
    return ret;
}

bool LimsDb::get_specimen_description(
        const string& _sample_id,
        mysqlpp::sql_varchar& _specimen_desc)
{
    assert(_sample_id.length() == 6);
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT SpecimenType.Description "
            "FROM SpecimenType "
            "JOIN Sample "
            "ON SpecimenType.TypeID=Sample.SpecimenType "
           "WHERE Sample.SampleID=" << mysqlpp::quote << _sample_id;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    
    assert(str_res.size() == 1);
    _specimen_desc.assign(str_res[0]["Description"].c_str());
    
    return ret;
}

bool LimsDb::get_received_date_and_InterpretResult(
        const string& _sample_id,
        mysqlpp::sql_date& _received_date,
        string& _InterpretResult) //syhwah 2013-03-13:- added _InterpretResult
{
    assert(_sample_id.length() == 6);
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT DATE(OrderForm.OrderReceivedDate),InterpretResult "
            "FROM OrderForm "
            "JOIN Sample "
            "ON OrderForm.OrderID=Sample.OrderID "
            "WHERE Sample.SampleID=" << mysqlpp::quote << _sample_id;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    
    assert(str_res.size() == 1);
    _received_date.convert(str_res[0]["DATE(OrderForm.OrderReceivedDate)"].c_str());
    _InterpretResult = str_res[0]["InterpretResult"].c_str();//syhwah 2013-03-13
    
    return ret;
}

bool LimsDb::get_test_code(
        const string& _open_array_id,
        mysqlpp::sql_char& _testcode)
{
    assert(_open_array_id.length() == 5);
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT DISTINCT SampleTestRun.TestCode "
            "FROM SampleTestRun "
            "WHERE SampleTestRun.OpenArrayID=" << mysqlpp::quote << _open_array_id;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    if (str_res.size() != 1) return false;
    
    _testcode.assign(str_res[0]["TestCode"].c_str());
    return ret;
}

bool LimsDb::get_lang_code(
        const string& _sample_id,
        set<string> &_lang_set)
{
    assert(_sample_id.empty() == false);
    _lang_set.clear();
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT DISTINCT ReportLanguage.LanguageCode "
            "FROM ReportLanguage "
            "JOIN Sample "
            "ON ReportLanguage.OrderID=Sample.OrderID "
            "WHERE "
            "Sample.SampleID=" << mysqlpp::quote << _sample_id;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    assert(str_res.empty() == false);
    for (size_t i = 0; i < str_res.size(); ++i)
        _lang_set.insert(str_res[i]["LanguageCode"].c_str());
    
    return ret;
}

//syhwah 2013-04-18
bool LimsDb::get_partner_code(
        const string& _sample_id,
        mysqlpp::sql_char& _partnercode)
{   

    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT o.PartnerCode FROM Sample s, OrderForm o "
            "WHERE s.SampleID=" << mysqlpp::quote << _sample_id << 
            " AND s.OrderID=o.OrderID" ;

    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return ret;
    if (str_res.size() != 1) return false;

    _partnercode.assign(str_res[0]["PartnerCode"].c_str());
    return ret;
}
         
